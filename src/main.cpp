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

int main() {

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

    // TOADDBACK 
    // Create all TemperatureControl modules. Note order is important here must be after extruder so Tn as a parameter will get executed first
    // TemperatureControlPool *tp= new TemperatureControlPool();
    // tp->load_tools();
    // delete tp;

    // Clear the configuration cache as it is no longer needed
    kernel->config->config_cache_clear();

    // Main loop
    while(1){
        THEKERNEL->call_event(ON_MAIN_LOOP);
        THEKERNEL->call_event(ON_IDLE);
    }

}
