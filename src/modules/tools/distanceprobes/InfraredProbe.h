/*
 * InfraredProbe.h
 *
 *  Created on: 4/09/2016
 *      Author: douglaspearless
 */

#ifndef SRC_MODULES_TOOLS_INFRAREDPROBES_INFRAREDPROBE_H_
#define SRC_MODULES_TOOLS_INFRAREDPROBES_INFRAREDPROBE_H_

#include "DistanceSensor.h"
#include "VCNL40x0.h"
#include <libs/Pin.h>
#include <mbed.h>
#include "RingBuffer.h"

class InfraredProbe : public DistanceSensor
{
public:
    InfraredProbe();
    ~InfraredProbe();
    void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum);
    float get_distance();

private:
    mbed::I2C *i2c;
    float read_sensors();
	void init_Infrared();
	//as VCNL40x0 all have the same hard wired address, we use a TCA9548A multiplexer and need to know what channel is is on
    VCNL40x0 VCNL40x0_Device;
    Pin i2c_sda_pin; 			// Of the TCA9548A multiplexer (and therefore the VCNL40x0)
    Pin i2c_scl_pin; 			// Of the TCA9548A multiplexer (and therefore the VCNL40x0)
    unsigned char i2c_addr;		// Of the TCA9548A multiplexer
    unsigned char i2c_channel;	//which channel the VCNL40x0 is attached to on the TCA9548A
    RingBuffer<float,16> readings;
};



#endif /* SRC_MODULES_TOOLS_INFRAREDPROBES_INFRAREDPROBE_H_ */
