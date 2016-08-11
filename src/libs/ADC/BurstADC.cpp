/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
 */


#include "BurstADC.h"

//using namespace mbed;

BurstADC *BurstADC::instance;

static inline int div_round_up(int x, int y) {
	return (x + (y - 1)) / y;
}

static const PinMap PinMap_ADC[] = {
		{P4_3,  ADC0_0, 0},
		{P4_1,  ADC0_1, 0},
		{PF_8,  ADC0_2, 0},
		{P7_5,  ADC0_3, 0},
		{P7_4,  ADC0_4, 0},
		{PF_10, ADC0_5, 0},
		{PB_6,  ADC0_6, 0},
		{PC_3,  ADC1_0, 0},
		{PC_0,  ADC1_1, 0},
		{PF_9,  ADC1_2, 0},
		{PF_6,  ADC1_3, 0},
		{PF_5,  ADC1_4, 0},
		{PF_11, ADC1_5, 0},
		{P7_7,  ADC1_6, 0},
		{PF_7,  ADC1_7, 0},
		{adc0_0,  ADC_pin0_0, 0},
		{adc0_1,  ADC_pin0_1, 0},
		{adc0_2,  ADC_pin0_2, 0},
		{adc0_3,  ADC_pin0_3, 0},
		{adc0_4,  ADC_pin0_4, 0},
		{adc0_5,  ADC_pin0_5, 0},
		{adc0_6,  ADC_pin0_6, 0},
		{adc0_7,  ADC_pin0_7, 0},
		{adc1_0,  ADC_pin1_0, 0},
		{adc1_1,  ADC_pin1_1, 0},
		{adc1_2,  ADC_pin1_2, 0},
		{adc1_3,  ADC_pin1_3, 0},
		{adc1_4,  ADC_pin1_4, 0},
		{adc1_5,  ADC_pin1_5, 0},
		{adc1_6,  ADC_pin1_6, 0},
		{adc1_7,  ADC_pin1_7, 0},
		{NC,    NC,     0   }
};

BurstADC::BurstADC(int sample_rate, int cclk_div, int adcNumber)
{
	int adc_clk_freq;

	adc_number = adcNumber;
	adc = (LPC_ADC_T *) (adc_number > 0) ? LPC_ADC1 : LPC_ADC0;

	//Check adc clock
	adc_clk_freq=CLKS_PER_SAMPLE*sample_rate;
	if(adc_clk_freq > MAX_ADC_CLOCK)
		adc_clk_freq = MAX_ADC_CLOCK;


	// calculate minimum clock divider
	int PCLK = SystemCoreClock;
	int clkdiv = 0;
	clkdiv = div_round_up(PCLK, adc_clk_freq) - 1;

	if (clkdiv > 0xFF) {
		printf("ADC Warning: Clock division is %u which is above 255 limit. Re-Setting at limit.\n", clkdiv);
		clkdiv=0xFF;
	}
	if (clkdiv == 0) {
		printf("ADC Warning: Clock division is 0. Re-Setting to 1.\n");
		clkdiv=1;
	}


	// Set the generic software-controlled ADC settings
	adc->CR = (0 << 0)      // SEL: 0 = no channels selected
							  | (clkdiv << 8) // CLKDIV:
							  | (0 << 16)     // BURST: 0 = software control
							  | (0 << 17)     // CLKS: 0 = 11 Clocks/10Bits
							  | (1 << 21)     // PDN: 1 = operational
							  | (0 << 24)     // START: 0 = no start
							  | (0 << 27);    // EDGE: not applicable

	//Default no channels enabled
	adc->CR &= ~0xFF;
	//Default NULL global custom isr
	_adc_g_isr = NULL;
	//Initialize arrays
	for (int i=7; i>=0; i--) {
		_adc_data[i] = 0;
		_adc_isr[i] = NULL;
	}


	//* Attach IRQ
	instance = this;
	NVIC_SetVector((adc_number>0)?ADC1_IRQn:ADC0_IRQn, (uint32_t)&_adcisr);

	//Disable global interrupt
	adc->INTEN &= ~0x100;



}

void BurstADC::_adcisr(void)
{
	instance->adcisr();
}


void BurstADC::adcisr(void)
{
	uint32_t stat;
	int chan;

	// Read status
	stat = adc->STAT;
	//Scan channels for over-run or done and update array
	if (stat & 0x0101) _adc_data[0] = adc->DR[0];
	if (stat & 0x0202) _adc_data[1] = adc->DR[1];
	if (stat & 0x0404) _adc_data[2] = adc->DR[2];
	if (stat & 0x0808) _adc_data[3] = adc->DR[3];
	if (stat & 0x1010) _adc_data[4] = adc->DR[4];
	if (stat & 0x2020) _adc_data[5] = adc->DR[5];
	if (stat & 0x4040) _adc_data[6] = adc->DR[6];
	if (stat & 0x8080) _adc_data[7] = adc->DR[7];

	// Channel that triggered interrupt
	chan = (adc->GDR >> 24) & 0x07;
	//User defined interrupt handlers
	if (_adc_isr[chan] != NULL)
		_adc_isr[chan](_adc_data[chan]);
	if (_adc_g_isr != NULL)
		_adc_g_isr(chan, _adc_data[chan]);

	return;
}

int BurstADC::_pin_to_channel(PinName pin) {

	ADCName name;

	name = (ADCName)pinmap_peripheral(pin, PinMap_ADC);

	int ch = name % (ADC0_7 + 1);

	return ch;
}

PinName BurstADC::channel_to_pin(int chan) {
	//LPC43xx has 2 ADC with 8 pins each + 8 adc only pins. Current structure of this function is not suitable
	PinName a;
	return a;
}


int BurstADC::channel_to_pin_number(int chan) {

	//LPC43xx has 2 ADC with 8 pins each + 8 adc only pins. Current structure of this function is not suitable
	return 0;
}


uint32_t BurstADC::_data_of_pin(PinName pin) {
	//If in burst mode and at least one interrupt enabled then
	//take all values from _adc_data
	if (burst() && (adc->INTEN & 0xFF)) {
		return(_adc_data[_pin_to_channel(pin)]);
	} else {
		//Return current register value or last value from interrupt
		int adcChn = _pin_to_channel(pin);

		switch (adcChn) {
		case 0:
			return(adc->INTEN & 0x01?_adc_data[0]:adc->DR[0]);
		case 1:
			return(adc->INTEN & 0x02?_adc_data[0]:adc->DR[1]);
		case 2:
			return(adc->INTEN & 0x04?_adc_data[0]:adc->DR[2]);
		case 3:
			return(adc->INTEN & 0x08?_adc_data[0]:adc->DR[3]);
		case 4:
			return(adc->INTEN & 0x10?_adc_data[0]:adc->DR[4]);
		case 5:
			return(adc->INTEN & 0x20?_adc_data[0]:adc->DR[5]);
		case 6:
			return(adc->INTEN & 0x40?_adc_data[0]:adc->DR[6]);
		case 7:
			return(adc->INTEN & 0x80?_adc_data[0]:adc->DR[7]);
		default:
			return 0;
		}

	}

}

//Enable or disable an ADC pin
void BurstADC::setup(PinName pin, int state) {

	int chan;
	ADCName name;

	name = (ADCName)pinmap_peripheral(pin, PinMap_ADC);
	MBED_ASSERT(adc != (LPC_ADC_T *)NC);

	// Set ADC number
	if(name < ADC1_0)
	{
		adc_number = 0;
	} else if(name < ADC_pin0_0 && name > ADC0_6)
	{
		adc_number = 1;
	} else if(name < ADC_pin1_1 && name > ADC1_7)
	{
		adc_number = 0;
	} else if(name > ADC_pin0_7)
	{
		adc_number = 1;
	}

	// Set ADC register, number and channel
	adc_number = (name >> ADC0_7) ? 1 : 0;
	chan = name % (ADC0_7 + 1);
	adc = (LPC_ADC_T *) (adc_number > 0) ? LPC_ADC1 : LPC_ADC0;


	if ((state & 1) == 1) {
		// Reset pin function to GPIO if it is a GPIO pin.
		if(name < ADC_pin0_0)
		{
			gpio_set(pin);
			// Select ADC on analog function select register in SCU
			LPC_SCU->ENAIO[adc_number] |= (1 << chan);
		} else {//If adc only pin configure GPIO as digital function
			LPC_SCU->ENAIO[adc_number] &= ~(1 << chan);
		}
		//Only one channel can be selected at a time if not in burst mode
		if (!burst()) adc->CR &= ~0xFF;
		//Select channel
		adc->CR |= (1 << chan);
	}
	else {
		// Reset pin function to GPIO if it is a GPIO pin.
		if(name < ADC_pin0_0)
		{
			gpio_set(pin);
			// Select digital on analog function select register in SCU
			LPC_SCU->ENAIO[adc_number] &= ~(1 << chan);
		}
	}

}

//Return channel enabled/disabled state
int BurstADC::setup(PinName pin) {
	int chan;

	chan = _pin_to_channel(pin);
	return((adc->CR & (1 << chan)) >> chan);
}

//Select channel already setup
void BurstADC::select(PinName pin) {
	int chan;

	//Only one channel can be selected at a time if not in burst mode
	if (!burst()) adc->CR &= ~0xFF;
	//Select channel
	chan = _pin_to_channel(pin);
	adc->CR |= (1 << chan);
}

//Enable or disable burst mode
void BurstADC::burst(int state) {

	if ((state & 1) == 1) {
		if (startmode(0) != 0)
			fprintf(stderr, "ADC Warning. startmode is %u. Must be 0 for burst mode.\n", startmode(0));
		adc->CR |= (1 << 16);
	}
	else
		adc->CR &= ~(1 << 16);
}
//Return burst mode state
int  BurstADC::burst(void) {
	return((adc->CR & (1 << 16)) >> 16);
}

//Set startmode and edge
void BurstADC::startmode(int mode, int edge) {
	int lpc_adc_temp;

	//Reset start mode and edge bit,
	lpc_adc_temp = adc->CR & ~(0x0F << 24);
	//Write with new values
	lpc_adc_temp |= ((mode & 7) << 24) | ((edge & 1) << 27);
	adc->CR = lpc_adc_temp;
}

//Return startmode state according to mode_edge=0: mode and mode_edge=1: edge
int BurstADC::startmode(int mode_edge){
	switch (mode_edge) {
	case 0:
	default:
		return((adc->CR >> 24) & 0x07);
	case 1:
		return((adc->CR >> 27) & 0x01);
	}
}

//Start ADC conversion
void BurstADC::start(void) {
	startmode(1,0);
}


//Set interrupt enable/disable for pin to state
void BurstADC::interrupt_state(PinName pin, int state) {
	int chan;

	chan = _pin_to_channel(pin);
	if (state == 1) {
		adc->INTEN &= ~0x100;
		adc->INTEN |= 1 << chan;
		/* Enable the ADC Interrupt */
		NVIC_EnableIRQ((adc_number>0)?ADC1_IRQn:ADC0_IRQn);
	} else {
		adc->INTEN &= ~( 1 << chan );
		//Disable interrrupt if no active pins left
		if ((adc->INTEN & 0xFF) == 0)
			NVIC_DisableIRQ((adc_number>0)?ADC1_IRQn:ADC0_IRQn);
	}
}

//Return enable/disable state of interrupt for pin
int BurstADC::interrupt_state(PinName pin) {
	int chan;

	chan = _pin_to_channel(pin);
	return((adc->INTEN >> chan) & 0x01);
}


//Attach custom interrupt handler replacing default
void BurstADC::attach(void(*fptr)(void)) {
	//* Attach IRQ
	NVIC_SetVector((adc_number>0)?ADC1_IRQn:ADC0_IRQn, (uint32_t)fptr);
}

//Restore default interrupt handler
void BurstADC::detach(void) {
	//* Attach IRQ
	instance = this;
	NVIC_SetVector((adc_number>0)?ADC1_IRQn:ADC0_IRQn, (uint32_t)&_adcisr);
}


//Append interrupt handler for pin to function isr
void BurstADC::append(PinName pin, void(*fptr)(uint32_t value)) {
	int chan;

	chan = _pin_to_channel(pin);
	_adc_isr[chan] = fptr;
}

//Append interrupt handler for pin to function isr
void BurstADC::unappend(PinName pin) {
	int chan;

	chan = _pin_to_channel(pin);
	_adc_isr[chan] = NULL;
}

//Unappend global interrupt handler to function isr
void BurstADC::append(void(*fptr)(int chan, uint32_t value)) {
	_adc_g_isr = fptr;
}

//Detach global interrupt handler to function isr
void BurstADC::unappend() {
	_adc_g_isr = NULL;
}

//Set ADC offset
void BurstADC::offset(int offset) {
	printf("Warnign no ADC Trim support");//TODO ADC Trim in LPC43xx???
}

//Return current ADC offset
int BurstADC::offset(void) {
	printf("Warnign no ADC Trim support");//TODO ADC Trim in LPC43xx???
	return 0;
}

//Return value of ADC on pin
int BurstADC::read(PinName pin) {
	//Reset DONE and OVERRUN flags of interrupt handled ADC data
	_adc_data[_pin_to_channel(pin)] &= ~(((uint32_t)0x01 << 31) | ((uint32_t)0x01 << 30));
	//Return value
	int pinData = ((_data_of_pin(pin) >> 6) & 0x3FF);
	return (pinData << 6) | ((pinData >> 4) & 0x003F); // 10 bit;
}

//Return DONE flag of ADC on pin
int BurstADC::done(PinName pin) {
	return((_data_of_pin(pin) >> 31) & 0x01);
}

//Return OVERRUN flag of ADC on pin
int BurstADC::overrun(PinName pin) {
	return((_data_of_pin(pin) >> 30) & 0x01);
}

int BurstADC::actual_adc_clock(void) {
	return(_adc_clk_freq);
}

int BurstADC::actual_sample_rate(void) {
	return(_adc_clk_freq / CLKS_PER_SAMPLE);
}


