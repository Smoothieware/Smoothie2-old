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
 
            char pins[164][4] = {{0x00, 0, 0, 0},{0x00, 1, 0, 1},{0x01, 0, 0, 4},{0x01, 1, 0, 8},{0x01, 2, 0, 9},{0x01, 3, 0, 10},{0x01, 4, 0, 11},{0x01, 5, 1, 8},{0x01, 6, 1, 9},{0x01, 7, 1, 0},{0x01, 8, 1, 1},{0x01, 9, 1, 2},{0x01, 10, 1, 3},{0x01, 11, 1, 4},{0x01, 12, 1, 5},{0x01, 13, 1, 6},{0x01, 14, 1, 7},{0x01, 15, 0, 2},{0x01, 16, 0, 3},{0x01, 17, 0, 12},{0x01, 18, 0, 13},{0x01, 20, 0, 15},{0x02, 0, 5, 0},{0x02, 1, 5, 1},{0x02, 2, 5, 2},{0x02, 3, 5, 3},{0x02, 4, 5, 4},{0x02, 5, 5, 5},{0x02, 6, 5, 6},{0x02, 7, 0, 7},{0x02, 8, 5, 7},{0x02, 9, 1, 10},{0x02, 10, 0, 14},{0x02, 11, 1, 11},{0x02, 12, 1, 12},{0x02, 13, 1, 13},{0x03, 1, 5, 8},{0x03, 2, 5, 9},{0x03, 4, 1, 14},{0x03, 5, 1, 15},{0x03, 6, 0, 6},{0x03, 7, 5, 10},{0x03, 8, 5, 11},{0x04, 0, 2, 0},{0x04, 1, 2, 1},{0x04, 2, 2, 2},{0x04, 3, 2, 3},{0x04, 4, 2, 4},{0x04, 5, 2, 5},{0x04, 6, 2, 6},{0x04, 8, 5, 12},{0x04, 9, 5, 13},{0x04, 10, 5, 14},{0x05, 0, 2, 9},{0x05, 1, 2, 10},{0x05, 2, 2, 11},{0x05, 3, 2, 12},{0x05, 4, 2, 13},{0x05, 5, 2, 14},{0x05, 6, 2, 15},{0x05, 7, 2, 7},{0x06, 1, 3, 0},{0x06, 2, 3, 1},{0x06, 3, 3, 2},{0x06, 4, 3, 3},{0x06, 5, 3, 4},{0x06, 6, 0, 5},{0x06, 7, 5, 15},{0x06, 8, 5, 16},{0x06, 9, 3, 5},{0x06, 10, 3, 6},{0x06, 11, 3, 7},{0x06, 12, 2, 8},{0x07, 0, 3, 8},{0x07, 1, 3, 9},{0x07, 2, 3, 10},{0x07, 3, 3, 11},{0x07, 4, 3, 12},{0x07, 5, 3, 13},{0x07, 6, 3, 14},{0x07, 7, 3, 15},{0x08, 8, 4, 0},{0x09, 0, 4, 1},{0x09, 1, 4, 2},{0x09, 2, 4, 3},{0x08, 4, 4, 4},{0x08, 5, 4, 5},{0x08, 6, 4, 6},{0x08, 7, 4, 7},{0x09, 0, 4, 12},{0x09, 1, 4, 13},{0x09, 2, 4, 14},{0x09, 3, 4, 15},{0x09, 4, 5, 17},{0x09, 5, 5, 18},{0x09, 6, 4, 11},{0x0A, 1, 4, 8},{0x0A, 2, 4, 9},{0x0A, 3, 4, 10},{0x0A, 4, 5, 19},{0x0B, 0, 5, 20},{0x0B, 1, 5, 21},{0x0B, 2, 5, 22},{0x0B, 3, 5, 23},{0x0B, 4, 5, 24},{0x0B, 5, 5, 25},{0x0B, 6, 5, 26},{0x0C, 1, 6, 0},{0x0C, 2, 6, 1},{0x0C, 3, 6, 2},{0x0C, 4, 6, 3},{0x0C, 5, 6, 4},{0x0C, 6, 6, 5},{0x0C, 7, 6, 6},{0x0C, 8, 6, 7},{0x0C, 9, 6, 8},{0x0C, 10, 6, 9},{0x0C, 11, 6, 10},{0x0C, 12, 6, 11},{0x0C, 13, 6, 12},{0x0C, 14, 6, 13},{0x0D, 0, 6, 14},{0x0D, 1, 6, 15},{0x0D, 2, 6, 16},{0x0D, 3, 6, 17},{0x0D, 4, 6, 18},{0x0D, 5, 6, 19},{0x0D, 6, 6, 20},{0x0D, 7, 6, 21},{0x0D, 8, 6, 22},{0x0D, 9, 6, 23},{0x0D, 10, 6, 24},{0x0D, 11, 6, 25},{0x0D, 12, 6, 26},{0x0D, 13, 6, 27},{0x0D, 14, 6, 28},{0x0D, 15, 6, 29},{0x0D, 16, 6, 30},{0x0E, 0, 7, 0},{0x0E, 1, 7, 1},{0x0E, 2, 7, 2},{0x0E, 3, 7, 3},{0x0E, 4, 7, 4},{0x0E, 5, 7, 5},{0x0E, 6, 7, 6},{0x0E, 7, 7, 7},{0x0E, 8, 7, 8},{0x0E, 9, 7, 9},{0x0E, 10, 7, 10},{0x0E, 11, 7, 11},{0x0E, 12, 7, 12},{0x0E, 13, 7, 13},{0x0E, 14, 7, 14},{0x0E, 15, 7, 15},{0x0F, 1, 7, 16},{0x0F, 2, 7, 17},{0x0F, 3, 7, 18},{0x0F, 5, 7, 19},{0x0F, 6, 7, 20},{0x0F, 7, 7, 21},{0x0F, 8, 7, 22},{0x0F, 9, 7, 23},{0x0F, 10, 7, 24},{0x0F, 11, 7, 25}};

            // Find the pin
            for( int i=0; i<164; ++i ){
                if( pins[i][0] == port_number && pins[i][1] == pin ){
                    this->mbed_pin = new DigitalInOut( MBED_PIN( pins[i][0], pins[i][1], pins[i][2], pins[i][3] ) );
                }
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
