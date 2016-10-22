/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/


#include "StepTicker.h"

#include "libs/nuts_bolts.h"
#include "libs/Module.h"
#include "libs/Kernel.h"
#include "StepperMotor.h"
#include "StreamOutputPool.h"
//#include "system_LPC17xx.h" // mbed.h lib
#include <math.h>
#include <mri.h>
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT_Conf.h"

#ifdef STEPTICKER_DEBUG_PIN
#include "gpio.h"
extern GPIO stepticker_debug_pin;
#endif


// StepTicker handles the base frequency ticking for the Stepper Motors / Actuators
// It has a list of those, and calls their tick() functions at regular intervals
// They then do Bresenham stuff themselves

StepTicker* StepTicker::global_step_ticker;

StepTicker::StepTicker(){
    StepTicker::global_step_ticker = this;

    uint32_t PCLK = SystemCoreClock;
    uint32_t prescale = PCLK / 10000000;

    /* Enable timer 0 clock and reset it */
    LPC_CCU1->CLKCCU[CLK_MX_TIMER0].CFG |= 1;
    LPC_RGU->RESET_CTRL1 = 1 << (RGU_TIMER0_RST & 31);	//Trigger a peripheral reset for the timer
    while (!(LPC_RGU->RESET_ACTIVE_STATUS1 & (1 << (RGU_TIMER0_RST & 31)))){}
    /* Configure Timer 0 */
    LPC_TIMER0->CTCR = 0x0;    // timer mode
	LPC_TIMER0->TCR = 0;    // Disable interrupt
	LPC_TIMER0->PR = prescale - 1;
	LPC_TIMER0->MR[0] = 10000000;    // Initial dummy value for Match Register
	LPC_TIMER0->MCR |= 3;    // Match on MR0, reset on MR0
	NVIC_SetPriority(TIMER0_IRQn,0);

	/* Enable timer 1 clock and reset it */
	LPC_CCU1->CLKCCU[CLK_MX_TIMER1].CFG |= 1;
	LPC_RGU->RESET_CTRL1 = 1 << (RGU_TIMER1_RST & 31);	//Trigger a peripheral reset for the timer
	while (!(LPC_RGU->RESET_ACTIVE_STATUS1 & (1 << (RGU_TIMER1_RST & 31)))){}
	/* Configure Timer 0 */
	LPC_TIMER1->CTCR = 0x0;    // timer mode
	LPC_TIMER1->TCR = 0;    // Disable interrupt
	LPC_TIMER1->PR = prescale - 1;
	LPC_TIMER1->MR[0] = 10000000;    // Initial dummy value for Match Register
	LPC_TIMER1->MCR |= 5;    // match on Mr0, stop on match

	//Configure RIT Timer for 1ms acceleration ticks
	LPC_CCU1->CLKCCU[CLK_MX_RITIMER].CFG = 0x07;
	LPC_RITIMER->COMPVAL = 0xFFFFFFFF;
	LPC_RITIMER->MASK  = 0x00000000;
	LPC_RITIMER->CTRL  = 0x0C;
	LPC_RITIMER->COUNTER   = 0x00000000;
	LPC_RITIMER->COMPVAL = (uint32_t)(((SystemCoreClock / 1000000L) * 1000)-1); // 1ms period
	LPC_RITIMER->CTRL |= 0; //disable RIT Timer

    // Default start values
    this->a_move_finished = false;
    this->do_move_finished = 0;
    this->unstep.reset();
    this->set_frequency(100000);
    this->set_reset_delay(5);
    this->set_acceleration_ticks_per_second(1000);
    this->num_motors= 0;
    this->active_motor.reset();
    this->tick_cnt= 0;

    /* Enable Timers IRQ */
	NVIC_EnableIRQ(TIMER1_IRQn);     // Enable interrupt handler
	NVIC_EnableIRQ(RITIMER_IRQn);

}

StepTicker::~StepTicker() {
}

// Called when everything is setup and interrupts can start
void StepTicker::start() {

}

// Set the base stepping frequency
void StepTicker::set_frequency( float frequency ){
    this->frequency = frequency;
    this->period_us = (int)lrint(1000000/frequency);

    /* Update Timer 0 Match register and reset timer */
    LPC_TIMER0->MR[0] = this->period_us;
    LPC_TIMER0->TCR = 3;  // Reset
    LPC_TIMER0->TCR = 1; // start
}

// Set the reset delay
void StepTicker::set_reset_delay( float microseconds ){
    this->reset_delay_us = (int)lrint(microseconds);
    /* Update Timer 1 Match register */
    LPC_TIMER1->MR[0] = this->reset_delay_us;
}

// This is the number of acceleration ticks per second
void StepTicker::set_acceleration_ticks_per_second(uint32_t acceleration_ticks_per_second) {
    this->acceleration_period_us = (int)lrint(1000000/acceleration_ticks_per_second);
    this->synchronize_acceleration(false);
}

// Synchronize the acceleration timer, and optionally schedule it to fire now
void StepTicker::synchronize_acceleration(bool fire_now) {
	LPC_RITIMER->COUNTER = 0;
	if(fire_now){
		NVIC_SetPendingIRQ(RITIMER_IRQn);
	}else{
		if(NVIC_GetPendingIRQ(RITIMER_IRQn)) {
			// clear pending interrupt so it does not interrupt immediately
			LPC_RITIMER->CTRL |= 1L; // also clear the interrupt in case it fired
			NVIC_ClearPendingIRQ(RITIMER_IRQn);
		}
	}
}


// Call signal_move_finished() on each active motor that asked to be signaled. We do this instead of inside of tick() so that
// all tick()s are called before we do the move finishing
void StepTicker::signal_a_move_finished(){
     for (int motor = 0; motor < num_motors; motor++){
        if (this->active_motor[motor] && this->motor[motor]->is_move_finished){
            this->motor[motor]->signal_move_finished();
        }
    }
}

// Reset step pins on any motor that was stepped
inline void StepTicker::unstep_tick(){
    for (int i = 0; i < num_motors; i++) {
        if(this->unstep[i]){
            this->motor[i]->unstep();
        }
    }
    this->unstep.reset();
}

// Acceleration timer interrupt handler
extern "C" void RITIMER_IRQHandler (void)
{

	LPC_RITIMER->CTRL |= 1L;
	//LPC_TIMER2->IR |= 1 << 0;
	StepTicker::global_step_ticker->acceleration_tick();
	//NVIC_ClearPendingIRQ(TIMER2_IRQn);
	LPC_RITIMER->COUNTER = 0;
	SEGGER_SYSVIEW_RecordExitISR();
}

// Unstep timer interrupt handler
extern "C" void TIMER1_IRQHandler (void)
{
//
	LPC_TIMER1->IR |= 1 << 0;
	StepTicker::global_step_ticker->unstep_tick();
	NVIC_ClearPendingIRQ(TIMER1_IRQn);
//	SEGGER_SYSVIEW_RecordExitISR();
}

// Step timer interrupt handler
extern "C" void TIMER0_IRQHandler (void)
{
//	SEGGER_RTT_LOCK();
//	SEGGER_SYSVIEW_RecordEnterISR();
	LPC_TIMER0->IR |= 1 << 0;
	StepTicker::global_step_ticker->step_tick();
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
//	SEGGER_SYSVIEW_RecordExitISR();
//	SEGGER_RTT_UNLOCK();
}

// RIT Timer interrupt handler
void  StepTicker::acceleration_tick() {
    // call registered acceleration handlers
    for (size_t i = 0; i < acceleration_tick_handlers.size(); ++i) {
        acceleration_tick_handlers[i]();
    }
}

void StepTicker::step_tick(void){
    // Reset interrupt register
    tick_cnt++; // count number of ticks

    // Step pins NOTE takes 1.2us when nothing to step, 1.8-2us for one motor stepped and 2.6us when two motors stepped, 3.167us when three motors stepped
    for (uint32_t motor = 0; motor < num_motors; motor++){
        // send tick to all active motors
        if(this->active_motor[motor] && this->motor[motor]->tick()){
            // we stepped so schedule an unstep
            this->unstep[motor]= 1;
        }
    }

    // We may have set a pin on in this tick, now we reset the timer to set it off
    // Note there could be a race here if we run another tick before the unsteps have happened,
    // right now it takes about 3-4us but if the unstep were near 10uS or greater it would be an issue
    // also it takes at least 2us to get here so even when set to 1us pulse width it will still be about 3us
    if( this->unstep.any()){
    	/* Start Unstep timer (Timer 1) */
    	LPC_TIMER1->TCR = 0x3;  // reset
    	LPC_TIMER1->TCR = 1; // enable = 1, reset = 0
    }
   
    // Check if any moves finished 
    if(this->a_move_finished) {
        this->a_move_finished= false;
        this->do_move_finished++; // Note this is an atomic variable because it is updated in two interrupts of different priorities so can be pre-empted
    }

    // If a move finished in this tick, we have to tell the actuator to act accordingly
    if(this->do_move_finished.load() > 0){
        if(this->do_move_finished.load() > 0) {
            this->do_move_finished--;
            this->signal_a_move_finished();
        }
    }
}

// returns index of the stepper motor in the array and bitset
int StepTicker::register_motor(StepperMotor* motor)
{
    this->motor.push_back(motor);
    this->num_motors= this->motor.size();
    return this->num_motors-1;
}

// activate the specified motor, must have been registered
void StepTicker::add_motor_to_active_list(StepperMotor* motor)
{
    bool enabled= active_motor.any(); // see if interrupt was previously enabled
    active_motor[motor->index]= 1;
    if(!enabled) {
    	/* Enable Step Timer (Timer 0) */
    	LPC_TIMER0->TCR = 0x2;  // reset
		LPC_TIMER0->TCR = 0x1;  // enable
		NVIC_EnableIRQ(TIMER0_IRQn);
    }
}

// Remove a stepper from the list of active motors
void StepTicker::remove_motor_from_active_list(StepperMotor* motor){
    active_motor[motor->index]= 0;
    // If we have no motor to work on, disable the whole interrupt
    if(this->active_motor.none()){
        /* Disable step Timer (Timer 0) */
    	NVIC_DisableIRQ(TIMER0_IRQn);
        tick_cnt= 0;
    }
}

