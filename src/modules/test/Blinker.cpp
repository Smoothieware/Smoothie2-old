#include "Blinker.h"
#include "Pin.h"

#include "mbed.h"

#include <string>
#include <stdarg.h>
using std::string;
#include <vector>
#include "SerialMessage.h"
#include "StreamOutput.h"
#include "Kernel.h"
#include "utils.h"

Blinker::Blinker(){
}

void Blinker::on_module_loaded(){
    this->register_for_event(ON_CONSOLE_LINE_RECEIVED);
    this->ticker = new Ticker();
    this->flag = false;
    this->period = 50000;
}

void Blinker::on_console_line_received(void *line){
    SerialMessage new_message = *static_cast<SerialMessage *>(line);

    // Ignore comments and blank lines and if this is a G code then also ignore it
    char first_char = new_message.message[0];
    if(strchr(";( \n\rGMTN", first_char) != NULL) return;

    // Extract the first parameter
    string possible_command = new_message.message;
    string cmd = shift_parameter(possible_command);

    // If this is a blink command
    if( cmd == "blink" ){
        // Extract the pin name parameter
        string pin_name = shift_parameter(possible_command); 
        Pin* pin = (new Pin())->from_string(pin_name)->as_output();
    
        // Add the pin to the vector
        this->pins.push_back(pin); 

        string possible_period = shift_parameter(possible_command);
        if( possible_period.size() > 0 ){
            this->period = atoi( possible_period.c_str() );
        }

        // Set the ticker
        this->ticker->attach_us(this, &Blinker::tick, this->period); 

    }

    // Command to set a pin
    if( cmd == "set" ){
        // Extract the pin name parameter
        string pin_name = shift_parameter(possible_command); 
        Pin* pin = (new Pin())->from_string(pin_name)->as_output();
        pin->set(true); 
        delete pin;
    }

    // Command to set a pin
    if( cmd == "get" ){
        // Extract the pin name parameter
        string pin_name = shift_parameter(possible_command); 
        Pin* pin = (new Pin())->from_string(pin_name)->as_output();
        new_message.stream->printf("Value: %d\n\r", pin->get());
        delete pin;
    }


    new_message.stream->printf("OK\n\r");
}

void Blinker::tick(){
    this->flag = !this->flag;
    for( Pin* pin : this->pins ){
        pin->set(this->flag);
    }

}
