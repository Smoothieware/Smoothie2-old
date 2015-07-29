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

# TODO : 

Things that are broken during the port and should be added back as things progress ( some things might be missing here. I try to use the TOADDBACK label when commenting things so search for that too ) : 

* In Switch.cpp : Dependence to Conveyor.h, and emptying of the queue in on_gcode_received
* In Switch.cpp : Commented out all of the public access stuff
* In GcodeDispatch.cpp : Removed dependency to Pauser.h, Conveyor.h
* In GcodeDispatch.cpp : Commented out all of the public access stuff
* In Switch.cpp : Commented out all of the set_low_on_debug stuff
* In SlowTicker.h and Hook.h : Using doubles instead of ints for intervals/frequencies and counting down. Done to accomodate mBed, but dirty costly hack


Things changed from Smoothie1 : 

* Pin now uses mBed instead of registers
* SlowTicker now uses mBed instead of registers
