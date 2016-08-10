#include "Pin.h"
#include "utils.h"

// mbed libraries for hardware pwm
#include "PwmOut.h"
#include "InterruptIn.h"
#include "PinNames.h"
#include "port_api.h"
#include "mbed.h"
#include <map>
using namespace std;

Pin::Pin(){
    this->inverting= false;
    this->valid= false;
    this->mbed_pin= NULL;
}

// Make a new pin object from a string
Pin* Pin::from_string(std::string value){
    // Port and pin for when we find them
    int port_number;
    int pin;

    // cs is the current position in the string
    const char* cs = value.c_str();
    // cn is the position of the next char after the number we just read
    char* cn = NULL;
    valid= true;

    //initialize adc only vars
    adc_only = false;
    adc_channel = 0xff;

    //verify if it is an analog only pin
    std::size_t found = value.find("adc");
    if (found!=std::string::npos) adc_only = true;

    if(adc_only)
    {
    	std::string channelStr = value.substr (4);
    	cs = channelStr.c_str();
    	adc_channel = strtol(cs, &cn, 10);

    	PinName adc_pins[16] = {adc0_0,adc0_1,adc0_2,adc0_3,adc0_4,adc0_5,adc0_6,adc0_7,adc1_0,adc1_1,adc1_2,adc1_3,adc1_4,adc1_5,adc1_6,adc1_7};
    	this->pinName = adc_pins[adc_channel];

    	return this;
    }


    // grab first integer as port. pointer to first non-digit goes in cn
    port_number = strtol(cs, &cn, 16); //allow hexadecimal portnumbes

    // if cn > cs then strtol read at least one digit
    if ((cn > cs) && (port_number <= 15)){
        // if the char after the first integer is a . then we should expect a pin index next
        if (*cn == '.'){
            // move pointer to first digit (hopefully) of pin index
            cs = ++cn;
            
            // grab pin index.
            pin = strtol(cs, &cn, 10);
           
            map<int, PinName> pins = {{(0x00<<8)+0,P0_0},{(0x00<<8)+1,P0_1},{(0x01<<8)+0,P1_0},{(0x01<<8)+1,P1_1},{(0x01<<8)+2,P1_2},{(0x01<<8)+3,P1_3},{(0x01<<8)+4,P1_4},{(0x01<<8)+5,P1_5},{(0x01<<8)+6,P1_6},{(0x01<<8)+7,P1_7},{(0x01<<8)+8,P1_8},{(0x01<<8)+9,P1_9},{(0x01<<8)+10,P1_10},{(0x01<<8)+11,P1_11},{(0x01<<8)+12,P1_12},{(0x01<<8)+13,P1_13},{(0x01<<8)+14,P1_14},{(0x01<<8)+15,P1_15},{(0x01<<8)+16,P1_16},{(0x01<<8)+17,P1_17},{(0x01<<8)+18,P1_18},{(0x01<<8)+20,P1_20},{(0x02<<8)+0,P2_0},{(0x02<<8)+1,P2_1},{(0x02<<8)+2,P2_2},{(0x02<<8)+3,P2_3},{(0x02<<8)+4,P2_4},{(0x02<<8)+5,P2_5},{(0x02<<8)+6,P2_6},{(0x02<<8)+7,P2_7},{(0x02<<8)+8,P2_8},{(0x02<<8)+9,P2_9},{(0x02<<8)+10,P2_10},{(0x02<<8)+11,P2_11},{(0x02<<8)+12,P2_12},{(0x02<<8)+13,P2_13},{(0x03<<8)+1,P3_1},{(0x03<<8)+2,P3_2},{(0x03<<8)+4,P3_4},{(0x03<<8)+5,P3_5},{(0x03<<8)+6,P3_6},{(0x03<<8)+7,P3_7},{(0x03<<8)+8,P3_8},{(0x04<<8)+0,P4_0},{(0x04<<8)+1,P4_1},{(0x04<<8)+2,P4_2},{(0x04<<8)+3,P4_3},{(0x04<<8)+4,P4_4},{(0x04<<8)+5,P4_5},{(0x04<<8)+6,P4_6},{(0x04<<8)+8,P4_8},{(0x04<<8)+9,P4_9},{(0x04<<8)+10,P4_10},{(0x05<<8)+0,P5_0},{(0x05<<8)+1,P5_1},{(0x05<<8)+2,P5_2},{(0x05<<8)+3,P5_3},{(0x05<<8)+4,P5_4},{(0x05<<8)+5,P5_5},{(0x05<<8)+6,P5_6},{(0x05<<8)+7,P5_7},{(0x06<<8)+1,P6_1},{(0x06<<8)+2,P6_2},{(0x06<<8)+3,P6_3},{(0x06<<8)+4,P6_4},{(0x06<<8)+5,P6_5},{(0x06<<8)+6,P6_6},{(0x06<<8)+7,P6_7},{(0x06<<8)+8,P6_8},{(0x06<<8)+9,P6_9},{(0x06<<8)+10,P6_10},{(0x06<<8)+11,P6_11},{(0x06<<8)+12,P6_12},{(0x07<<8)+0,P7_0},{(0x07<<8)+1,P7_1},{(0x07<<8)+2,P7_2},{(0x07<<8)+3,P7_3},{(0x07<<8)+4,P7_4},{(0x07<<8)+5,P7_5},{(0x07<<8)+6,P7_6},{(0x07<<8)+7,P7_7},{(0x08<<8)+8,P8_0},{(0x09<<8)+0,P8_1},{(0x09<<8)+1,P8_2},{(0x09<<8)+2,P8_3},{(0x08<<8)+4,P8_4},{(0x08<<8)+5,P8_5},{(0x08<<8)+6,P8_6},{(0x08<<8)+7,P8_7},{(0x09<<8)+0,P9_0},{(0x09<<8)+1,P9_1},{(0x09<<8)+2,P9_2},{(0x09<<8)+3,P9_3},{(0x09<<8)+4,P9_4},{(0x09<<8)+5,P9_5},{(0x09<<8)+6,P9_6},{(0x0A<<8)+1,PA_1},{(0x0A<<8)+2,PA_2},{(0x0A<<8)+3,PA_3},{(0x0A<<8)+4,PA_4},{(0x0B<<8)+0,PB_0},{(0x0B<<8)+1,PB_1},{(0x0B<<8)+2,PB_2},{(0x0B<<8)+3,PB_3},{(0x0B<<8)+4,PB_4},{(0x0B<<8)+5,PB_5},{(0x0B<<8)+6,PB_6},{(0x0C<<8)+1,PC_1},{(0x0C<<8)+2,PC_2},{(0x0C<<8)+3,PC_3},{(0x0C<<8)+4,PC_4},{(0x0C<<8)+5,PC_5},{(0x0C<<8)+6,PC_6},{(0x0C<<8)+7,PC_7},{(0x0C<<8)+8,PC_8},{(0x0C<<8)+9,PC_9},{(0x0C<<8)+10,PC_10},{(0x0C<<8)+11,PC_11},{(0x0C<<8)+12,PC_12},{(0x0C<<8)+13,PC_13},{(0x0C<<8)+14,PC_14},{(0x0D<<8)+0,PD_0},{(0x0D<<8)+1,PD_1},{(0x0D<<8)+2,PD_2},{(0x0D<<8)+3,PD_3},{(0x0D<<8)+4,PD_4},{(0x0D<<8)+5,PD_5},{(0x0D<<8)+6,PD_6},{(0x0D<<8)+7,PD_7},{(0x0D<<8)+8,PD_8},{(0x0D<<8)+9,PD_9},{(0x0D<<8)+10,PD_10},{(0x0D<<8)+11,PD_11},{(0x0D<<8)+12,PD_12},{(0x0D<<8)+13,PD_13},{(0x0D<<8)+14,PD_14},{(0x0D<<8)+15,PD_15},{(0x0D<<8)+16,PD_16},{(0x0E<<8)+0,PE_0},{(0x0E<<8)+1,PE_1},{(0x0E<<8)+2,PE_2},{(0x0E<<8)+3,PE_3},{(0x0E<<8)+4,PE_4},{(0x0E<<8)+5,PE_5},{(0x0E<<8)+6,PE_6},{(0x0E<<8)+7,PE_7},{(0x0E<<8)+8,PE_8},{(0x0E<<8)+9,PE_9},{(0x0E<<8)+10,PE_10},{(0x0E<<8)+11,PE_11},{(0x0E<<8)+12,PE_12},{(0x0E<<8)+13,PE_13},{(0x0E<<8)+14,PE_14},{(0x0E<<8)+15,PE_15},{(0x0F<<8)+1,PF_1},{(0x0F<<8)+2,PF_2},{(0x0F<<8)+3,PF_3},{(0x0F<<8)+5,PF_5},{(0x0F<<8)+6,PF_6},{(0x0F<<8)+7,PF_7},{(0x0F<<8)+8,PF_8},{(0x0F<<8)+9,PF_9},{(0x0F<<8)+10,PF_10},{(0x0F<<8)+11,PF_11}}; 
           
            // Find the pin
            if( pins.count((port_number<<8)+pin) > 0 ){
                this->mbed_pin = new DigitalInOut( pins[(port_number<<8)+pin] );
                this->pinName = pins[(port_number<<8)+pin];
            }

            // if strtol read some numbers, cn will point to the first non-digit
            if ((cn > cs) && (pin < 32)){

                // now check for modifiers:-
                // ! = invert pin
                // o = set pin to open drain
                // ^ = set pin to pull up
                // v = set pin to pull down
                // - = set pin to no pull up or down
                // @ = set pin to repeater mode
                for (;*cn;cn++) {
                    switch(*cn) {
                        case '!':
                            this->inverting = true;
                            break;
                        case 'o':
                            as_open_drain();
                            break;
                        case '^':
                            pull_up();
                            break;
                        case 'v':
                            pull_down();
                            break;
                        case '-':
                            pull_none();
                            break;
                        case '@':
                            as_repeater();
                            break;
                        default:
                            // skip any whitespace following the pin index
                            if (!is_whitespace(*cn))
                                return this;
                    }
                }
   
                return this;
            }
        }
    }

    // from_string failed. TODO: some sort of error
    this->valid= false;
    this->inverting = false;
    return this;
}

// Configure this pin as OD
Pin* Pin::as_open_drain(){
    if (!this->valid) return this;
    pull_none(); // no pull up by default
    this->mbed_pin->mode(OpenDrain);
    return this;
}


// Configure this pin as a repeater
Pin* Pin::as_repeater(){
    if (!this->valid) return this;
    return this;
}

// Configure this pin as no pullup or pulldown
Pin* Pin::pull_none(){
	if (!this->valid) return this;
        this->mbed_pin->mode(PullNone);
	return this;
}

// Configure this pin as a pullup
Pin* Pin::pull_up(){
    if (!this->valid) return this;
    this->mbed_pin->mode(PullUp);
    return this;
}

// Configure this pin as a pulldown
Pin* Pin::pull_down(){
    if (!this->valid) return this;
    this->mbed_pin->mode(PullDown);
    return this;
}

// If available on this pin, return mbed hardware pwm class for this pin
mbed::PwmOut* Pin::hardware_pwm()
{
    return nullptr;
}

mbed::InterruptIn* Pin::interrupt_pin()
{
    if(!this->valid) return nullptr;
/*
    // set as input
    as_input();

    if (port_number == 0 || port_number == 2) {
        PinName pinname = port_pin((PortName)port_number, pin);
        return new mbed::InterruptIn(pinname);

    }else{
        this->valid= false;
        return nullptr;
    }
*/
    return nullptr;
}
