/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Adc.h"
#include "libs/nuts_bolts.h"
#include "libs/Kernel.h"
#include "libs/Pin.h"
#include "libs/ADC/BurstADC.h"
#include "libs/Pin.h"


using namespace std;
#include <vector>

Adc *Adc::instance;

static void sample_isr(int chan, uint32_t value)
{
	Adc::instance->new_sample(chan, value);
}

Adc::Adc(){
	instance = this;
	// ADC sample rate need to be fast enough to be able to read the enabled channels within the thermistor poll time
	// even though ther maybe 32 samples we only need one new one within the polling time


	const uint32_t sample_rate= 400000; // 400KHz sample rate
	this->adc = new BurstADC(sample_rate, 8, 0);
	this->adc->append(sample_isr);
}

// Keeps the last 8 values for each channel
// This is called in an ISR, so sample_buffers needs to be accessed atomically
void Adc::new_sample(int chan, uint32_t value)
{
	// Shuffle down and add new value to the end
	if(chan < num_channels) {
		memmove(&sample_buffers[chan][0], &sample_buffers[chan][1], sizeof(sample_buffers[0]) - sizeof(sample_buffers[0][0]));
		//sample_buffers[chan][num_samples - 1] = (value >> 4) & 0xFFF; // the 12 bit ADC reading
		sample_buffers[chan][num_samples - 1] = (value >> 6) & 0x3FF; // the 10 bit ADC reading
	}
}

// Enables ADC on a given pin
void Adc::enable_pin(Pin* pin){
	/*
			 PinName pin_name = this->_pin_to_pinname(pin);
			this->analogin = new AnalogIn(pin_name);
	 */

	PinName pin_name = this->_pin_to_pinname(pin);
	int channel = adc->_pin_to_channel(pin_name);
	memset(sample_buffers[channel], 0, sizeof(sample_buffers[0]));

	this->adc->burst(1);
	this->adc->setup(pin_name, 1);
	this->adc->interrupt_state(pin_name, 0);
}

// Read the last value ( burst mode ) on a given pin
unsigned int Adc::read(Pin* pin){
	return this->adc->read(this->_pin_to_pinname(pin));
}

// Convert a smoothie Pin into a mBed Pin
PinName Adc::_pin_to_pinname(Pin* pin){
	return pin->getPinName();

}

