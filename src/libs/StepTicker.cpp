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
// TOADDBACK #include "system_LPC17xx.h" // mbed.h lib
#include <math.h>
#include <mri.h>

#include "mbed.h"

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

    // MBed ticker
    this->ticker = new Ticker();

    // MBed timeout for unstepping
    this->unstep_timeout = new Timeout();

    // Default start values
    this->a_move_finished = false;
    this->do_move_finished = 0;
    this->unstep.reset();
    this->set_frequency(100000);
    this->set_reset_delay(100);
    this->set_acceleration_ticks_per_second(1000);
    this->num_motors= 0;
    this->active_motor.reset();
    this->tick_cnt= 0;
}

StepTicker::~StepTicker() {
}

void StepTicker::start() {
}

// Set the base stepping frequency
void StepTicker::set_frequency( float frequency ){
    frequency = 1000; //TOADDBACK
    this->period = (float)1 / frequency;
    //this->ticker->attach_us( this, &StepTicker::TIMER0_IRQHandler, roundf( (float)1000000 / frequency ));
}

// Set the reset delay
void StepTicker::set_reset_delay( float microseconds ){}

// this is the number of acceleration ticks per second
void StepTicker::set_acceleration_ticks_per_second(uint32_t acceleration_ticks_per_second) {}

// Synchronize the acceleration timer, and optionally schedule it to fire now
void StepTicker::synchronize_acceleration(bool fire_now) {}

// slightly lower priority than TIMER0, the whole end of block/start of block is done here allowing the timer to continue ticking
void StepTicker::PendSV_IRQHandler (void) {}

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

// run in RIT lower priority than PendSV
void  StepTicker::acceleration_tick() {
    // call registered acceleration handlers
    for (size_t i = 0; i < acceleration_tick_handlers.size(); ++i) {
        acceleration_tick_handlers[i]();
    }
}

void StepTicker::TIMER0_IRQHandler (void){
    
    tick_cnt++; // count number of ticks

    // Step pins 
    // NOTE ( on LPC1769 ) takes 1.2us when nothing to step, 1.8-2us for one motor stepped and 2.6us when two motors stepped, 3.167us when three motors stepped
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
        this->unstep_timeout->attach_us( this, &StepTicker::unstep_tick, 2 );
    }

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
int StepTicker::register_motor(StepperMotor* motor){
    this->motor.push_back(motor);
    this->num_motors= this->motor.size();
    return this->num_motors-1;
}

// activate the specified motor, must have been registered
void StepTicker::add_motor_to_active_list(StepperMotor* motor){
    bool enabled= active_motor.any(); // see if interrupt was previously enabled
    active_motor[motor->index]= 1;
    if(!enabled) {
        // TOADDBACK LPC_TIM0->TCR = 1;               // Enable interrupt
    }
}

// Remove a stepper from the list of active motors
void StepTicker::remove_motor_from_active_list(StepperMotor* motor){
    active_motor[motor->index]= 0;
    // If we have no motor to work on, disable the whole interrupt
    if(this->active_motor.none()){
        // TOADDBACK LPC_TIM0->TCR = 0;               // Disable interrupt
        tick_cnt= 0;
    }
}
