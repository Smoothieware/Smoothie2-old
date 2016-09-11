/*
 * InfraredProbe.cpp
 *
 *  Created on: 4/09/2016
 *      Author: douglaspearless
 */
#include "libs/Kernel.h"
#include <math.h>
#include "InfraredProbe.h"
#include "VCNL40x0.h"
#include "libs/Pin.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"
#include "DistanceSensor.h"

#define chip_sda_checksum CHECKSUM("i2c_sda_pin") 	//of the TCA9548A
#define chip_scl_checksum CHECKSUM("i2c_scl_pin") 	//of the TCA9548A
#define chip_addr_checksum CHECKSUM("i2c_addr")		//of the TCA9548A
#define i2c_channel_checksum CHECKSUM("i2c_channel")//of the TCA9548A for a given VCNL40x0 which has a fixed address of VCNL40x0_ADDRESS

InfraredProbe::InfraredProbe() : i2c(nullptr), VCNL40x0_Device(this->i2c_sda_pin.getPinName(),this->i2c_scl_pin.getPinName(),this->i2c_addr)
{
	i2c_channel = 0;
	// TODO : check the state of the i2c and the VCNL40x0_ADDRESS as they are probably junk
}

InfraredProbe::~InfraredProbe()
{
	VCNL40x0_Device.SetCurrent (20);     // Set current to 0mA
	delete i2c;
}

void InfraredProbe::UpdateConfig(uint16_t module_checksum, uint16_t name_checksum)
{
	this->i2c_sda_pin.from_string(THEKERNEL->config->value(module_checksum, name_checksum, chip_sda_checksum)->by_default("2.3")->as_string());
	this->i2c_scl_pin.from_string(THEKERNEL->config->value(module_checksum, name_checksum, chip_scl_checksum)->by_default("2.4")->as_string());
	this->i2c_addr = (THEKERNEL->config->value(module_checksum, name_checksum, chip_addr_checksum)->by_default(VCNL40x0_ADDRESS)->as_int());
	this->i2c_channel = (THEKERNEL->config->value(module_checksum, name_checksum, chip_addr_checksum)->by_default(0)->as_int());

	VCNL40x0_Device.Init_VCNL40x0(this->i2c_sda_pin.getPinName(),this->i2c_scl_pin.getPinName(), this->i2c_addr); //SDA,SCL,address
}

float InfraredProbe::get_distance()
{
	return(0.0);
}


/* Powers up all the VCNL4020. */
void InfraredProbe::init_Infrared()
{
	VCNL40x0_Device.SetCurrent (20);     // Set current to 200mA
}

float InfraredProbe::read_sensors()
{
    //this->i2c_sda_pin.set(false);
    wait_us(1); // Must wait for first bit valid
    VCNL40x0_Device.SetCurrent (20);     // Set current to 200mA

    // Read 16 bits (writing something as well is required by the api)
    uint16_t data = i2c->write(0);
	//  Read next 16 bits (diagnostics)


    //this->i2c_sda_pin.set(true);

    float distance;

    //Process temp
    if (data & 0x0001)
    {
        // Error flag.
        distance = infinityf();
        // TODO: Interpret data2 for more diagnostics.
    }
    else
    {
        data = data >> 2;
        distance = (data & 0x1FFF) / 4.f;

        if (data & 0x2000)
        {
            data = ~data;
            distance = ((data & 0x1FFF) + 1) / -4.f;
        }
    }
    return distance;
}
