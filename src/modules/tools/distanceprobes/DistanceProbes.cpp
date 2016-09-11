/*
 * DistanceProbes.cpp
 *
 *  Created on: 7/09/2016
 *      Author: douglaspearless
 */
#include "DistanceProbes.h"
#include "libs/Module.h"
#include "libs/Kernel.h"
#include "VCNL40x0.h"
#include "modules/communication/utils/Gcode.h"
//#include "modules/robot/Conveyor.h"
//#include "modules/robot/ActuatorCoordinates.h"
//#include "libs/nuts_bolts.h"
//#include "libs/Pin.h"
//#include "wait_api.h" // mbed.h lib
//#include "Robot.h"
//#include "Stepper.h"
#include "Config.h"
//#include "SlowTicker.h"
//#include "Planner.h"
#include "checksumm.h"
//#include "utils.h"
#include "ConfigValue.h"
//#include "libs/StreamOutput.h"
#include "PublicDataRequest.h"
//#include "EndstopsPublicAccess.h"
//#include "StreamOutputPool.h"
//#include "StepTicker.h"
//#include "BaseSolution.h"
//#include "SerialMessage.h"
#include "Gcode.h"

#define distanceprobes_module_enable_checksum         CHECKSUM("distanceprobes_enable")
#define sensor_checksum                   			  CHECKSUM("sensor")
#define get_m_code_checksum               			  CHECKSUM("get_m_code")
#define set_m_code_checksum        			          CHECKSUM("set_m_code")

#define corexy_homing_checksum                		  CHECKSUM("corexy_homing")
#define delta_homing_checksum   		              CHECKSUM("delta_homing")
#define rdelta_homing_checksum  		              CHECKSUM("rdelta_homing")
#define scara_homing_checksum                   	  CHECKSUM("scara_homing")

DistanceProbes::DistanceProbes()
{
	sensor[0]= nullptr;
	sensor[1]= nullptr;
	sensor[2]= nullptr;
}

DistanceProbes::~DistanceProbes()
{
    delete sensor[0];
    delete sensor[1];
    delete sensor[2];
}

void DistanceProbes::on_module_loaded()
{
    // Do not do anything if not enabled
    if ( THEKERNEL->config->value( distanceprobes_module_enable_checksum )->by_default(true)->as_bool() == false ) {
        delete this;
        return;
    }

    this->set_m_code          = THEKERNEL->config->value(distance_probes_checksum, this->name_checksum, set_m_code_checksum)->by_default(104)->as_number();
    this->get_m_code          = THEKERNEL->config->value(distance_probes_checksum, this->name_checksum, get_m_code_checksum)->by_default(105)->as_number();


    register_for_event(ON_GCODE_RECEIVED);
    register_for_event(ON_GET_PUBLIC_DATA);
    register_for_event(ON_SET_PUBLIC_DATA);

    // Settings
    this->load_config();
}

// Get config
void DistanceProbes::load_config()
{
    //this->pins[0].from_string( THEKERNEL->config->value(alpha_min_endstop_checksum          )->by_default("nc" )->as_string())->as_input();
    this->is_corexy                 =  THEKERNEL->config->value(corexy_homing_checksum)->by_default(false)->as_bool();
    this->is_delta                  =  THEKERNEL->config->value(delta_homing_checksum)->by_default(false)->as_bool();
    this->is_rdelta                 =  THEKERNEL->config->value(rdelta_homing_checksum)->by_default(false)->as_bool();
    this->is_scara                  =  THEKERNEL->config->value(scara_homing_checksum)->by_default(false)->as_bool();

    std::string sensor_type = THEKERNEL->config->value(distance_probes_checksum, this->name_checksum, sensor_checksum)->by_default("vcnl40x0")->as_string();

    // Instantiate correct sensor (TODO: DistanceSensor factory?)
    delete sensor[0];
    delete sensor[1];
    delete sensor[2];
    sensor[0] = nullptr; // In case we fail to create a new sensor.
    sensor[1] = nullptr; // In case we fail to create a new sensor.
    sensor[2] = nullptr; // In case we fail to create a new sensor.
    if(sensor_type.compare("vcnl40x0") == 0) {
    	//Read the config details for the three VCNL40x0 I2C devices and the I2C channel and TDA9
//        sensor[0] = new VCNL40x0();
//    } else if(sensor_type.compare("max31855") == 0) {
//        sensor = new Max31855();
    } else {
        sensor[0] = new DistanceSensor(); // A dummy implementation
        sensor[1] = new DistanceSensor(); // A dummy implementation
        sensor[2] = new DistanceSensor(); // A dummy implementation
    }


}

// Start homing sequences by response to GCode commands
void DistanceProbes::on_gcode_received(void *argument)
{
    Gcode *gcode = static_cast<Gcode *>(argument);
    if ( gcode->has_g && gcode->g == 28) {
       // process_home_command(gcode);

    } else if (gcode->has_m) {

        switch (gcode->m) {
            case 119: {
//                for (int i = 0; i < 6; ++i) {
//                    if(this->pins[i].connected())
//                        gcode->stream->printf("%s:%d ", endstop_names[i], this->pins[i].get());
//                }
                gcode->add_nl = true;

            }
            break;


            }
        }
    }


void DistanceProbes::on_get_public_data(void* argument)
{
    PublicDataRequest* pdr = static_cast<PublicDataRequest*>(argument);

//    if(!pdr->starts_with(DistanceProbes_checksum)) return;

//    if(pdr->second_element_is(trim_checksum)) {
//        pdr->set_data_ptr(&this->trim_mm);
//        pdr->set_taken();

//    } else if(pdr->second_element_is(home_offset_checksum)) {
//        pdr->set_data_ptr(&this->home_offset);
//        pdr->set_taken();

//    } else if(pdr->second_element_is(saved_position_checksum)) {
//        pdr->set_data_ptr(&this->saved_position);
//        pdr->set_taken();

//    } else if(pdr->second_element_is(get_homing_status_checksum)) {
//        bool *homing = static_cast<bool *>(pdr->get_data_ptr());
//        *homing = this->status != NOT_HOMING;
        pdr->set_taken();
//    }
}

void DistanceProbes::on_set_public_data(void* argument)
{
    PublicDataRequest* pdr = static_cast<PublicDataRequest*>(argument);

//    if(!pdr->starts_with(DistanceProbes_checksum)) return;

//    if(pdr->second_element_is(trim_checksum)) {
//        float *t = static_cast<float*>(pdr->get_data_ptr());
//        this->trim_mm[0] = t[0];
//        this->trim_mm[1] = t[1];
//        this->trim_mm[2] = t[2];
        pdr->set_taken();

//    } else if(pdr->second_element_is(home_offset_checksum)) {
//        float *t = static_cast<float*>(pdr->get_data_ptr());
//        if(!isnan(t[0])) this->home_offset[0] = t[0];
//        if(!isnan(t[1])) this->home_offset[1] = t[1];
//        if(!isnan(t[2])) this->home_offset[2] = t[2];
//    }
}

