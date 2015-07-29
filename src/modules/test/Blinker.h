#ifndef BLINKER_MODULE_H
#define BLINKER_MODULE_H
#include "mbed.h"
#include "Module.h"
#include "Pin.h"
#include <vector>

class Blinker : public Module {
    public:
        Blinker();
        virtual ~Blinker() {};
        virtual void on_module_loaded();
        virtual void on_console_line_received(void *line);
        void tick();
        Ticker* ticker;
        std::vector<Pin*> pins;
        bool flag;
};

#endif
