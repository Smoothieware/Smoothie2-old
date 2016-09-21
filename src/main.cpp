/* Test which brings default HelloWorld project from mbed online compiler
   to be built under GCC.
 */
#include "mbed.h"
#include "Kernel.h"
#include "SwitchPool.h"
#include "TemperatureControlPool.h"
#include "Endstops.h"
#include "Laser.h"
#include "Config.h"
#include "StreamOutputPool.h"

DigitalOut leds[4] = {
		DigitalOut(LED1),
		DigitalOut(LED2),
		DigitalOut(LED3),
		DigitalOut(LED4)
};

DigitalOut st = DigitalOut(P3_2);

LPC_TIMER_T* testTimer;
int t_cnt = 0;
bool l_state = false;

/*
extern "C" void TIMER1_IRQHandler (void)
{
/*
    //LPC_TIM1->IR |= 1 << 0;
    //StepTicker::getInstance()->unstep_tick();
	//StepTicker::global_step_ticker->unstep_tick();
	/*
	if(l_state){
		st = 0;
		l_state = false;
	} else {
		st = 1;
		l_state = true;
	}
	*/
/*
	t_cnt++;
	testTimer->IR = 1;
	__DSB();
	//testTimer->TCR = 0x2;  // reset
	//testTimer->TCR = 1; // enable = 1, reset = 0
	st= (t_cnt++ & 0x1000) ? 1 : 0;
*/

/*
	testTimer->CTCR = 0x0; // timer mode
	uint32_t PCLK = SystemCoreClock;
	testTimer->TCR = 0x2;  // reset

	uint32_t prescale = PCLK / 10000000;
	testTimer->PR = prescale - 1;
	testTimer->TCR = 1; // enable = 1, reset = 0

	testTimer->MR[0] = 10;
	testTimer->MCR |= 1;
	NVIC_EnableIRQ(TIMER0_IRQn);
*/
/*
}
*/

int main() {

	int cnt = 0;

	// Kernel creates modules, and receives and dispatches events between them
	Kernel* kernel = new Kernel();

	// Say hello ( TODO : Add back version and all )
	kernel->streams->printf("Smoothie2 dev\n");

	// Create and add main modules
	kernel->add_module( new Endstops() );
	kernel->add_module( new Laser() );

	// Create all Switch modules
	SwitchPool *sp= new SwitchPool();
	sp->load_tools();
	delete sp;

	// Create all TemperatureControl modules. Note order is important here must be after extruder so Tn as a parameter will get executed first
	TemperatureControlPool *tp= new TemperatureControlPool();
	tp->load_tools();
	delete tp;

	// Clear the configuration cache as it is no longer needed
	kernel->config->config_cache_clear();
/*
	testTimer = ((LPC_TIMER_T *)LPC_TIMER1_BASE);

	testTimer->CTCR = 0x0; // timer mode
	uint32_t PCLK = SystemCoreClock;
	testTimer->TCR = 0x2;  // reset

	uint32_t prescale = PCLK / 1000000; // default to 1MHz (1 us ticks)
	//testTimer->PR = prescale - 1;
	testTimer->PR = 158;
	testTimer->TCR = 0; // enable = 1, reset = 0

	testTimer->MR[0] = 1;
	testTimer->MCR |= 1;
	//NVIC_SetPriority(TIMER1_IRQn,1);
	//NVIC_EnableIRQ(TIMER1_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
*/

	// Main loop
	while(1){


		if(THEKERNEL->is_using_leds()) {
			// flash led 2 to show we are alive
			leds[0]= (cnt++ & 0x1000) ? 1 : 0;
		}
		THEKERNEL->call_event(ON_MAIN_LOOP);
		THEKERNEL->call_event(ON_IDLE);

	}

}
