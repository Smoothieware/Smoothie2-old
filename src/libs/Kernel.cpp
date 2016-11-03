/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#include "libs/Kernel.h"
#include "libs/Module.h"
#include "libs/Config.h"
#include "libs/nuts_bolts.h"
#include "libs/SlowTicker.h"
#include "libs/Adc.h"
#include "libs/StreamOutputPool.h"
//#include <mri.h>
#include "checksumm.h"
#include "ConfigValue.h"

#include "libs/StepTicker.h"
#include "libs/PublicData.h"
#include "modules/communication/SerialConsole.h"
#include "modules/communication/GcodeDispatch.h"
#include "modules/robot/Planner.h"
#include "modules/robot/Robot.h"
#include "modules/robot/Conveyor.h"
#include "StepperMotor.h"
#include "BaseSolution.h"
#include "EndstopsPublicAccess.h"
//#include "Configurator.h"
//#include "SimpleShell.h"

//#include "platform_memory.h"

#include <malloc.h>
#include <array>
#include <string>

#define baud_rate_setting_checksum CHECKSUM("baud_rate")
#define uart0_checksum             CHECKSUM("uart0")

#define base_stepping_frequency_checksum            CHECKSUM("base_stepping_frequency")
#define microseconds_per_step_pulse_checksum        CHECKSUM("microseconds_per_step_pulse")
#define acceleration_ticks_per_second_checksum      CHECKSUM("acceleration_ticks_per_second")
#define disable_leds_checksum                       CHECKSUM("leds_disable")
#define grbl_mode_checksum                          CHECKSUM("grbl_mode")
#define ok_per_line_checksum                        CHECKSUM("ok_per_line")

Kernel* Kernel::instance;

// The kernel is the central point in Smoothie : it stores modules, and handles event calls
Kernel::Kernel(){
    halted= false;
    feed_hold= false;

    instance= this; // setup the Singleton instance of the kernel

    // All streams register to the Kernel so we can broadcast messages
    this->streams = new StreamOutputPool();

    // Create the default UART Serial Console interface
    if (SMOOTHIE_UART_PRIMARY_ENABLE) {
        this->serial = new SerialConsole(SMOOTHIE_UART_PRIMARY_TX, SMOOTHIE_UART_PRIMARY_RX, 9600);
        this->add_module( this->serial );
    }
    if (SMOOTHIE_UART_SECONDARY_ENABLE) {
        this->secondary_serial = new SerialConsole(SMOOTHIE_UART_SECONDARY_TX, SMOOTHIE_UART_SECONDARY_RX, 9600);
        this->add_module( this->secondary_serial );
    }

    // Config next, but does not load cache yet
    this->config = new Config();

    // Pre-load the config cache, do after setting up serial so we can report errors to serial
    this->config->config_cache_load();

    // ADC reading
    this->adc = new Adc();

    // STEP TICKER
    this->step_ticker = new StepTicker();

    // For slow repeteative tasks
    this->add_module( this->slow_ticker = new SlowTicker());

    this->current_path   = "/";

    //some boards don't have leds.. TOO BAD!
    this->use_leds= !this->config->value( disable_leds_checksum )->by_default(false)->as_bool();
    this->grbl_mode= this->config->value( grbl_mode_checksum )->by_default(false)->as_bool();
    this->ok_per_line= this->config->value( ok_per_line_checksum )->by_default(true)->as_bool();

    // Configure the step ticker
    this->base_stepping_frequency = this->config->value(base_stepping_frequency_checksum)->by_default(100000)->as_number();
    float microseconds_per_step_pulse = this->config->value(microseconds_per_step_pulse_checksum)->by_default(1)->as_number();

    // Configure the step ticker
    this->step_ticker->set_frequency( this->base_stepping_frequency );
    this->step_ticker->set_unstep_time( microseconds_per_step_pulse );


    // TODO : These should go into platform-specific files
    // LPC17xx-specific
    NVIC_SetPriorityGrouping(0);
    NVIC_SetPriority(TIMER0_IRQn, 2);
    NVIC_SetPriority(TIMER1_IRQn, 1);
    NVIC_SetPriority(TIMER2_IRQn, 4);
    NVIC_SetPriority(PendSV_IRQn, 3);
    NVIC_SetPriority(RITIMER_IRQn, 3); // we make acceleration tick the same prio as pendsv so it can't be pre-empted by end of block

    // Core modules
    this->add_module( this->conveyor       = new Conveyor()      );
    this->add_module( this->gcode_dispatch = new GcodeDispatch() );
    this->add_module( this->robot          = new Robot()         );
    this->add_module( this->conveyor       = new Conveyor()      );
    // TOADDBACK this->add_module( this->simpleshell    = new SimpleShell()   );

    this->planner = new Planner();

    // TOADDBACK this->configurator   = new Configurator();
}

// return a GRBL-like query string for serial ?
std::string Kernel::get_query_string()
{
    std::string str;
    str.append("<");
    if(halted) {
        str.append("Alarm,");
    }else if(this->conveyor->is_queue_empty()) {
        str.append("Idle,");
    }else{
        str.append("Run,");
    }

    // get real time current actuator position in mm
    ActuatorCoordinates current_position{
        robot->actuators[X_AXIS]->get_current_position(),
        robot->actuators[Y_AXIS]->get_current_position(),
        robot->actuators[Z_AXIS]->get_current_position()
    };

    // get machine position from the actuator position using FK
    float mpos[3];
    robot->arm_solution->actuator_to_cartesian(current_position, mpos);

    char buf[64];
    // machine position
    size_t n= snprintf(buf, sizeof(buf), "%f,%f,%f,", mpos[0], mpos[1], mpos[2]);
    str.append("MPos:").append(buf, n);

    // work space position
    Robot::wcs_t pos= robot->mcs2wcs(mpos);
    n= snprintf(buf, sizeof(buf), "%f,%f,%f", robot->from_millimeters(std::get<X_AXIS>(pos)), robot->from_millimeters(std::get<Y_AXIS>(pos)), robot->from_millimeters(std::get<Z_AXIS>(pos)));
    str.append("WPos:").append(buf, n);
    str.append(">\r\n");
    return str;
}

// Add a module to Kernel. We don't actually hold a list of modules we just call its on_module_loaded
void Kernel::add_module(Module* module){
    module->on_module_loaded();
}

// Adds a hook for a given module and event
void Kernel::register_for_event(_EVENT_ENUM id_event, Module *mod){
    this->hooks[id_event].push_back(mod);
}

// Call a specific event with an argument
void Kernel::call_event(_EVENT_ENUM id_event, void * argument){
    bool was_idle= true;
    if(id_event == ON_HALT) {
        this->halted= (argument == nullptr);
        was_idle= conveyor->is_queue_empty(); // see if we were doing anything like printing
    }

    // send to all registered modules
    for (auto m : hooks[id_event]) {
        (m->*kernel_callback_functions[id_event])(argument);
    }

    if(id_event == ON_HALT && this->halted && !was_idle) {
        // we need to try to correct current positions if we were running
        this->robot->reset_position_from_current_actuator_position();
    }
}

// These are used by tests to test for various things. basically mocks
bool Kernel::kernel_has_event(_EVENT_ENUM id_event, Module *mod)
{
    for (auto m : hooks[id_event]) {
        if(m == mod) return true;
    }
    return false;
}

void Kernel::unregister_for_event(_EVENT_ENUM id_event, Module *mod)
{
    for (auto i = hooks[id_event].begin(); i != hooks[id_event].end(); ++i) {
        if(*i == mod) {
            hooks[id_event].erase(i);
            return;
        }
    }
}
