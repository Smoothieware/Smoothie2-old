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

* In GcodeDispatch.cpp : Removed dependency to Pauser.h
* In Switch.cpp : Commented out all of the set_low_on_debug stuff
* In SlowTicker.h and Hook.h : Using doubles instead of ints for intervals/frequencies and counting down. Done to accomodate mBed, but dirty costly hack
* In SlowTicker.h : Assuming it's fine to use mBed for this, we can remove a lot of code and just rely fully on mBed
* In RingBuffer.h : Removed the irq stuff
* In Laser.cpp : PWM pin is broken, needs to be fixed

Things changed from Smoothie1 : 

* Pin now uses mBed instead of registers
* SlowTicker now uses mBed instead of registers
* StepTicker now uses mBed instead of register
* Various "waits" now use mBed's Timer

Major current TODO : 
 * Porting of existing functionality that hasn't been ported yet ( anything in https://github.com/Smoothieware/Smoothieware/tree/edge/src/modules that isn't ported yet )
 * Some modules have been ported, but the underlying low-level/HAL stuff to talk to the peripherals isn't ( like SPI, or ADCs etc ), it's just dummy objects right now. So that needs porting too.
 * USB and Ethernet need to be implemented.
 * Doing the step generation on the M0 co-processor instead of on the M4 main core.


Planned refactors : 

* Remove the checksum system for config options
* Try to read config directly from SD card without cache ( as smoothie used to do ) now that we have SDIO
* Queue refactor ( see wiki )
* Adding MTP and removing MSD for USB
* Full rewrite based on a RTOS
