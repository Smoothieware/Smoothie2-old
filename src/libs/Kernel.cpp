/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Kernel.h"
#include "Module.h"
#include "StreamOutputPool.h"
#include "SerialConsole.h"
#include "SlowTicker.h"
#include "Blinker.h"
#include "Robot.h"
#include "Conveyor.h"
#include "Planner.h"
#include "GcodeDispatch.h"
#include "Config.h"
#include <malloc.h>
#include <array>
#include "mbed.h"

Kernel* Kernel::instance;

// The kernel is the central point in Smoothie : it stores modules, and handles event calls
Kernel::Kernel(){
    instance= this; // setup the Singleton instance of the kernel

    // All streams register to the Kernel so we can broadcast messages
    this->streams = new StreamOutputPool();

    // Create the default UART Serial Console interface
    this->serial = new SerialConsole(P3_4, P3_5, 9600);
    this->add_module( this->serial );

    // Config next, but does not load cache yet
    this->config = new Config();
    
    // Pre-load the config cache, do after setting up serial so we can report errors to serial
    this->config->config_cache_load();

    // For slow repeteative tasks
    this->add_module( this->slow_ticker = new SlowTicker());

    // The Blinker module blinks a GPIO pin for testing purposes
    this->blinker = new Blinker();
    this->add_module( this->blinker );

    // Core modules
    this->add_module( new GcodeDispatch() );
    this->add_module( this->robot = new Robot() );
    this->add_module( this->conveyor = new Conveyor() );
    this->planner = new Planner();

}

// Add a module to Kernel. We don't actually hold a list of modules we just call its on_module_loaded
void Kernel::add_module(Module* module){
    module->on_module_loaded();
}

// Adds a hook for a given module and event
void Kernel::register_for_event(_EVENT_ENUM id_event, Module *mod){
    this->hooks[id_event].push_back(mod);
}

// Call a specific event without arguments
void Kernel::call_event(_EVENT_ENUM id_event){
    for (auto m : hooks[id_event]) {
        (m->*kernel_callback_functions[id_event])(this);
    }
}

// Call a specific event with an argument
void Kernel::call_event(_EVENT_ENUM id_event, void * argument){
    for (auto m : hooks[id_event]) {
        (m->*kernel_callback_functions[id_event])(argument);
    }
}
