/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BURST_ADC_H
#define BURST_ADC_H


#include "PinNames.h"
#include "mbed.h"
#include "pinmap.h"

#define MAX_ADC_CLOCK   4500000
#define CLKS_PER_SAMPLE 11

class BurstADC {
public:

	//Initialize ADC with ADC maximum sample rate of
	//sample_rate and system clock divider of cclk_div
	//Maximum recommened sample rate is .... TODO: what is the max recommended dample rate
	BurstADC(int sample_rate, int cclk_div,int adcNumber);

	//Enable/disable ADC on pin according to state
	//and also select/de-select for next conversion
	void setup(PinName pin, int state);

	//Return enabled/disabled state of ADC on pin
	int setup(PinName pin);

	//Enable/disable burst mode according to state
	void burst(int state);

	//Select channel already setup
	void select(PinName pin);

	//Return burst mode enabled/disabled
	int burst(void);

	/*Set start condition and edge according to mode:
	    0 - No start (this value should be used when clearing PDN to 0).
	    1 - Start conversion now.
	    2 - Start conversion when the edge selected by bit 27 occurs on the P2.10 / EINT0 / NMI pin.
	    3 - Start conversion when the edge selected by bit 27 occurs on the P1.27 / CLKOUT /
	        USB_OVRCRn / CAP0.1 pin.
	    4 - Start conversion when the edge selected by bit 27 occurs on MAT0.1. Note that this does
	        not require that the MAT0.1 function appear on a device pin.
	    5 - Start conversion when the edge selected by bit 27 occurs on MAT0.3. Note that it is not
	        possible to cause the MAT0.3 function to appear on a device pin.
	    6 - Start conversion when the edge selected by bit 27 occurs on MAT1.0. Note that this does
	        not require that the MAT1.0 function appear on a device pin.
	    7 - Start conversion when the edge selected by bit 27 occurs on MAT1.1. Note that this does
	        not require that the MAT1.1 function appear on a device pin.
	    When mode >= 2, conversion is triggered by edge:
	    0 - Rising edge
	    1 - Falling edge
	 */
	void startmode(int mode, int edge);

	//Return startmode state according to mode_edge=0: mode and mode_edge=1: edge
	int startmode(int mode_edge);

	//Start ADC conversion
	void start(void);

	//Set interrupt enable/disable for pin to state
	void interrupt_state(PinName pin, int state);

	//Return enable/disable state of interrupt for pin
	int interrupt_state(PinName pin);

	//Attach custom interrupt handler replacing default
	void attach(void(*fptr)(void));

	//Restore default interrupt handler
	void detach(void);

	//Append custom interrupt handler for pin
	void append(PinName pin, void(*fptr)(uint32_t value));

	//Unappend custom interrupt handler for pin
	void unappend(PinName pin);

	//Append custom global interrupt handler
	void append(void(*fptr)(int chan, uint32_t value));

	//Unappend custom global interrupt handler
	void unappend(void);

	//Set ADC offset to a value 0-7
	void offset(int offset);

	//Return current ADC offset
	int offset(void);

	//Return value of ADC on pin
	int read(PinName pin);

	//Return DONE flag of ADC on pin
	int done(PinName pin);

	//Return OVERRUN flag of ADC on pin
	int overrun(PinName pin);

	//Return actual ADC clock
	int actual_adc_clock(void);

	//Return actual maximum sample rate
	int actual_sample_rate(void);

	//Return pin ID of ADC channel
	PinName channel_to_pin(int chan);

	//Return pin number of ADC channel
	int channel_to_pin_number(int chan);

	int _pin_to_channel(PinName pin);


private:


	uint32_t _data_of_pin(PinName pin);

	int _adc_clk_freq;
	void adcisr(void);
	static void _adcisr(void);
	static BurstADC *instance;

	LPC_ADC_T *adc;
	int adc_number;

	uint32_t _adc_data[8];
	void(*_adc_isr[8])(uint32_t value);
	void(*_adc_g_isr)(int chan, uint32_t value);
	void(*_adc_m_isr)(void);

};


#endif
