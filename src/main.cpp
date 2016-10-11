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
#ifdef TARGET_BAMBINO210E
        DigitalOut(P6_11),
        DigitalOut(P2_5),
        DigitalOut(P6_1),
        DigitalOut(P6_2)

#elif defined(TARGET_BAMBINO200E)
        DigitalOut(P6_11),
        DigitalOut(P2_5),
        DigitalOut(P6_11), // only has 2 leds
        DigitalOut(P2_5)

#elif defined(TARGET_SMOOTHIE2_PROTO1)
        // smoothie 2 proto1
        DigitalOut(P1_1),
        DigitalOut(P1_2),
        DigitalOut(P2_8),
        DigitalOut(P2_9)
#endif
};

int main() {

	int cnt = 0;

	// Kernel creates modules, and receives and dispatches events between them
	Kernel* kernel = new Kernel();

	// Say hello ( TODO: Add back version and all )
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
