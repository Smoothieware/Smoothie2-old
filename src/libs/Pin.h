#ifndef PIN_H
#define PIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string>

//#include "libs/LPC17xx/sLPC17xx.h" // smoothed mbed.h lib
#include "PinNames.h"

namespace mbed {
class PwmOut;
class InterruptIn;
}

#include "mbed.h"

class Pin {
public:
	Pin();

	Pin* from_string(std::string value);

	inline bool connected(){
		return this->valid;
	}

	inline bool equals(const Pin& other) const {
		//return (this->pin == other.pin) && (this->port == other.port); //TODO this needs to be fixed
		return false;
	}

	inline Pin* as_output(){
		if (this->valid)
			this->mbed_pin->output();
		return this;
	}

	inline Pin* as_input(){
		if (this->valid)
			this->mbed_pin->input();
		return this;
	}

	Pin* as_open_drain(void);

	Pin* as_repeater(void);

	Pin* pull_up(void);

	Pin* pull_down(void);

	Pin* pull_none(void);

	inline bool get(){
		if (!this->valid) return false;
		return this->inverting ^ ( (bool)this->mbed_pin->read() );
	}

	inline void set(bool value)
	{
		if (!this->valid) return;
		if ( this->inverting ^ value ){
			this->mbed_pin->write(1);
		}else{
			this->mbed_pin->write(0);
		}
	}

	mbed::PwmOut *hardware_pwm();

	mbed::InterruptIn *interrupt_pin();

	bool is_inverting() const { return inverting; }
	void set_inverting(bool f) { inverting= f; }

	// these should be private, and use getters
	DigitalInOut* mbed_pin;

	struct {
		bool inverting:1;
		bool valid:1;
		bool adc_only:1;    //true if adc only pin
		int adc_channel:8;    //adc channel
	};

	//getter to access to mbed pinName
	PinName getPinName() {return pinName; }

	int port_number;
    int pin_number;

private:
	//Added pinName
	PinName pinName;
};

#endif
