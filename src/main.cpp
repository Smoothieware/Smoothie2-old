/* Test which brings default HelloWorld project from mbed online compiler
   to be built under GCC.
 */
#include "mbed.h"
#include "Kernel.h"
#include "SwitchPool.h"
#include "TemperatureControlPool.h"
#include "Endstops.h"
#include "Laser.h"
#include "Config.h"
#include "StreamOutputPool.h"

extern "C" void debugHardfault(uint32_t *sp);

DigitalOut leds[4] = {
#ifdef TARGET_BAMBINO210E
        DigitalOut(P6_11),
        DigitalOut(P2_5),
        DigitalOut(P6_1),
        DigitalOut(P6_2)

#elif defined(TARGET_BAMBINO200E)
        DigitalOut(P6_11),
        DigitalOut(P2_5),
        DigitalOut(P6_11), // only has 2 leds
        DigitalOut(P2_5)

#elif defined(TARGET_SMOOTHIE2_PROTO1)
        // smoothie 2 proto1
        DigitalOut(P1_1),
        DigitalOut(P1_2),
        DigitalOut(P2_8),
        DigitalOut(P2_9)
#endif
};

int main() {

	int cnt = 0;

	// Kernel creates modules, and receives and dispatches events between them
	Kernel* kernel = new Kernel();

	// Say hello ( TODO: Add back version and all )
	kernel->streams->printf("Smoothie2 dev\n");

	// Create and add main modules
	kernel->add_module( new Endstops() );
	kernel->add_module( new Laser() );

	// Create all Switch modules
	SwitchPool *sp= new SwitchPool();
	sp->load_tools();
	delete sp;

	// Create all TemperatureControl modules. Note order is important here must be after extruder so Tn as a parameter will get executed first
	TemperatureControlPool *tp= new TemperatureControlPool();
	tp->load_tools();
	delete tp;

	// Clear the configuration cache as it is no longer needed
	kernel->config->config_cache_clear();

	// Main loop
	while(1){
		if(THEKERNEL->is_using_leds()) {
			// flash led 2 to show we are alive
			leds[0]= (cnt++ & 0x1000) ? 1 : 0;
		}
		THEKERNEL->call_event(ON_MAIN_LOOP);
		THEKERNEL->call_event(ON_IDLE);
	}

}

extern "C" void debugHardfault(uint32_t *sp)
{
    uint32_t cfsr  = SCB->CFSR;
    uint32_t hfsr  = SCB->HFSR;
    uint32_t mmfar = SCB->MMFAR;
    uint32_t bfar  = SCB->BFAR;

    uint32_t r0  = sp[0];
    uint32_t r1  = sp[1];
    uint32_t r2  = sp[2];
    uint32_t r3  = sp[3];
    uint32_t r12 = sp[4];
    uint32_t lr  = sp[5];
    uint32_t pc  = sp[6];
    uint32_t psr = sp[7];

    printf("HardFault:\n");
    printf("SCB->CFSR   0x%08lx\n", cfsr);
    printf("SCB->HFSR   0x%08lx\n", hfsr);
    printf("SCB->MMFAR  0x%08lx\n", mmfar);
    printf("SCB->BFAR   0x%08lx\n", bfar);
    printf("\n");

    printf("SP          0x%08lx\n", (uint32_t)sp);
    printf("R0          0x%08lx\n", r0);
    printf("R1          0x%08lx\n", r1);
    printf("R2          0x%08lx\n", r2);
    printf("R3          0x%08lx\n", r3);
    printf("R12         0x%08lx\n", r12);
    printf("LR          0x%08lx\n", lr);
    printf("PC          0x%08lx\n", pc);
    printf("PSR         0x%08lx\n", psr);

    while(1);

}

extern "C" void HardFault_Handler(void) {

    __asm volatile
        (
            "tst lr, #4                                    \n"
            "ite eq                                        \n"
            "mrseq r0, msp                                 \n"
            "mrsne r0, psp                                 \n"
            "ldr r1, debugHardfault_address                \n"
            "bx r1                                         \n"
            "debugHardfault_address: .word debugHardfault  \n"
        );
}

