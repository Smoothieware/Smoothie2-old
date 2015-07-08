/* Test which brings default HelloWorld project from mbed online compiler
   to be built under GCC.
*/
#include "mbed.h"
#include "Kernel.h"

int main() {

    // Kernel creates modules, and receives and dispatches events between them
    Kernel* kernel = new Kernel(); 

}

