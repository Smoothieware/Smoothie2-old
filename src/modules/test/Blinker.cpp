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

#define stoi(x) atoi(x.c_str())

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
        string possible_direction = shift_parameter(possible_command); 
        Pin* pin = (new Pin())->from_string(pin_name);
        if(possible_direction[0] == 'i') {
            pin->as_input();
        }else{
            pin->as_output();
        }

        if (!this->pins.empty())
        {
            for (unsigned int i=0;i<this->pins.size();i++)
            {
                if(pin->equals(*this->pins[i]))
                {
                    // erase the pin, replace and pop is faster since we don't care about order
                    this->pins[i] = this->pins.back();
                    this->pins.pop_back();
                }
            }
        }

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

    // Command to read and write spi
    if( cmd == "spi" ){
        // Extract the pin name parameter
        string port = shift_parameter(possible_command);
        string data = shift_parameter(possible_command);
        string possible_frequency = shift_parameter(possible_command);
        SPI* spi = NULL;
        Pin* cs = new Pin();
        int r = 0;

        if(port == "0") {
            spi = new SPI(P3_7, P3_6, P3_3);
            cs->from_string("3.8");
        }else if(port == "1") {
            spi = new SPI(P1_4, P1_3, PF_4);
            cs->from_string("1.5");
        }

        if(spi != NULL){
            int freq = 100000;
            if(possible_frequency.size()>0){
                freq = stoi(possible_frequency);
            }
            cs->set(1);
            spi->format(8,3);
            spi->frequency(freq);
            cs->set(0);
            r = spi->write(stoi(data));
            cs->set(1);
            new_message.stream->printf("Value: 0x%x (%d)\n\r", r, r);
        }else{
            new_message.stream->printf("Port %s not found", port.c_str());
        }

        delete spi;
    }


    new_message.stream->printf("OK\n\r");
}

void Blinker::tick(){
    this->flag = !this->flag;
    for( Pin* pin : this->pins ){
        pin->set(this->flag);
    }

}
