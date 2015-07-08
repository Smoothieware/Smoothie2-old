# Smoothie2

(Attempt at) A Smoothie firmware port to the Smoothieboard v2 boards.

The current plan, is only to code as much as is needed to test the various peripherals on the v2-pro board.

However, a minimal implementation of the Kernel/Module system is ported from Smoothie1

Once the testing features are added, I plan to try to port minimal Smoothie motion control features

# Compiling

First, go into gcc4mbed

    cd gcc4mbed/

Then, install the toolchain

    ./linux_install

Finally, setup the build environment

    ./BuildShell

Then go to the Smoothie2 source code

    cd ../src/

And finally compile the code

    make


