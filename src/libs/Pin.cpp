#include "Pin.h"
#include "utils.h"

// mbed libraries for hardware pwm
#include "PwmOut.h"
#include "InterruptIn.h"
#include "PinNames.h"
#include "port_api.h"
#include "mbed.h"

Pin::Pin(){
    this->inverting= false;
    this->valid= false;
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

    // grab first integer as port. pointer to first non-digit goes in cn
    port_number = strtol(cs, &cn, 10);
    // if cn > cs then strtol read at least one digit
    if ((cn > cs) && (port_number <= 7)){
        // if the char after the first integer is a . then we should expect a pin index next
        if (*cn == '.'){
            // move pointer to first digit (hopefully) of pin index
            cs = ++cn;
            
            // grab pin index.
            pin = strtol(cs, &cn, 10);
 
            // Get a mBed port
            PortName port;
            switch(port_number){
                case 0 : port = Port0;
                case 1 : port = Port1;
                case 2 : port = Port2;
                case 3 : port = Port3;
                case 4 : port = Port4;
                case 5 : port = Port5;
                case 6 : port = Port6;
                case 7 : port = Port7;
            }
            
            // Get a mBed pin from the port and pin number 
            PinName pin_name = port_pin( port, pin ); 

            // Set our mbed DigitalInOut pin
            this->mbed_pin = new DigitalInOut(pin_name);

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
