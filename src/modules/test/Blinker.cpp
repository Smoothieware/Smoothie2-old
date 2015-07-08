#include "Blinker.h"
#include <string>
#include <stdarg.h>
using std::string;
#include "SerialMessage.h"
#include "StreamOutput.h"
#include "Kernel.h"

Blinker::Blinker(){
}

void Blinker::on_module_loaded(){
    this->register_for_event(ON_CONSOLE_LINE_RECEIVED);
}

void Blinker::on_console_line_received(void *line){
    SerialMessage new_message = *static_cast<SerialMessage *>(line);
    new_message.stream->printf("OK\n");
}
