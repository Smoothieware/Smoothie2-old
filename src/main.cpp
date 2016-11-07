/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#include "libs/Kernel.h"

#include "modules/tools/laser/Laser.h"
#include "modules/tools/extruder/ExtruderMaker.h"
#include "modules/tools/temperaturecontrol/TemperatureControlPool.h"
#include "modules/tools/endstops/Endstops.h"
#include "modules/tools/switch/SwitchPool.h"
#include "modules/tools/drillingcycles/Drillingcycles.h"

#include "modules/robot/Conveyor.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "StepTicker.h"
#include "SlowTicker.h"
#include "Robot.h"

// #include "libs/ChaNFSSD/SDFileSystem.h"
#include "libs/nuts_bolts.h"
#include "libs/utils.h"

// Debug
#include "libs/SerialMessage.h"
/*
#include "libs/USBDevice/USB.h"
#include "libs/USBDevice/USBMSD/USBMSD.h"
#include "libs/USBDevice/USBMSD/SDCard.h"
#include "libs/USBDevice/USBSerial/USBSerial.h"
#include "libs/USBDevice/DFU.h"
#include "libs/SDFAT.h"
*/
#include "StreamOutputPool.h"
#include "ToolManager.h"

#include "system_LPC43xx.h"
#include "platform_memory.h"

#include "mbed.h"

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

	// start the timers and interrupts
	kernel->conveyor->start(THEROBOT->get_number_registered_motors());
	kernel->step_ticker->start();
	THEKERNEL->slow_ticker->start();


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

