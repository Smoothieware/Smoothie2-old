/**
 * HardFaultHandler_C:
 *
 * This is called from the HardFault_HandlerAsm with a pointer the Fault stack
 * as the parameter. We can then read the values from the stack and place them
 * into local variables for ease of reading.
 *
 * We then read the various Fault Status and Address Registers to help decode
 * cause of the fault.
 * The function ends with a BKPT instruction to force control back into the debugger
 *
 * Refer to http://support.code-red-tech.com/CodeRedWiki/DebugHardFault
 *
 * And to http://www.freertos.org/Debugging-Hard-Faults-On-Cortex-M-Microcontrollers.html
 *
 * Also refer to:
 * Cortex M4 exception handling
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0439b/ch03s09s01.html
 * Cortex M0 exception handing
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/BABBFABJ.html
 *
 * The first register of interest is the program counter.
 *
 * In the code, the variable stacked_pc contains the program counter value.
 * When the fault is a precise fault, the stacked_pc holds the address of the instruction that was executing when the hard fault
 * (or other fault) occurred. When the fault is an imprecise fault, then additional steps are required to find the address of the
 * instruction that caused the fault.
 *
 * To find the instruction at the address held in the stacked_pc variable, either...
 * Open an assembly code window in the debugger, and manually enter the address to view the assembly instructions at that address, or
 * Open the break point window in the debugger, and manually define an execution or access break point at that address.
 * With the break point set, restart the application to see which line of code the instruction relates to.
 *
 * Knowing the instruction that was being executed when the fault occurred allows you to determine which other register values are
 * also of interest.
 *
 * For example, if the instruction was using the value of R7 as an address, then the value of R7 needs to be know.
 * Further, examining the assembly code, and the C code that generated the assembly code, will show what R7 actually holds
 * (it might be the value of a variable, for example).
 *
 * Handling Imprecise Faults:
 *
 * ARM Cortex-M faults can be precise or imprecise.
 *
 * If the IMPRECISERR bit (bit 2) is set in the Bus Fault Status Register (or BFSR, which is at address 0xE000ED29), then the fault
 * is imprecise.
 *
 * It is harder to determine the cause of an imprecise fault because the fault will not necessarily occur concurrently with the
 * instruction that caused the fault.
 *
 * For example, if writes to memory are cached then there might be a delay between an assembly instruction initiating a write to
 * memory and the write to memory actually occurring.
 *
 * If such a delayed write operation is invalid (for example, a write is being attempted to an invalid memory location) then an
 * imprecise fault will occur, and the program counter value obtained using the code above will not be the address of the assembly
 * instruction that initiated the write operation.
 *
 * Turn off write buffering by setting the DISDEFWBUF bit (bit 1) in the Auxiliary Control Register (or ACTLR) compile and
 * re-run the test and then the imprecise fault becoming a precise fault, which makes the fault easier to debug.
 *
 *  Note: this will come at the cost of slower program execution.
 */

// This overrides the WEAK definition of HardFault_Handler
void HardFault_HandlerC(unsigned long *hardfault_args){
  volatile unsigned long stacked_r0 ;
  volatile unsigned long stacked_r1 ;
  volatile unsigned long stacked_r2 ;
  volatile unsigned long stacked_r3 ;
  volatile unsigned long stacked_r12 ;
  volatile unsigned long stacked_lr ;  // Link register
  volatile unsigned long stacked_pc ;  // Program counter
  volatile unsigned long stacked_psr ; // Program status register
  volatile unsigned long _CFSR ;
  volatile unsigned long _HFSR ;
  volatile unsigned long _DFSR ;
  volatile unsigned long _AFSR ;
  volatile unsigned long _BFAR ;
  volatile unsigned long _MMAR ;
  volatile unsigned long _BFSR ;

  stacked_r0 = ((unsigned long)hardfault_args[0]) ;
  stacked_r1 = ((unsigned long)hardfault_args[1]) ;
  stacked_r2 = ((unsigned long)hardfault_args[2]) ;
  stacked_r3 = ((unsigned long)hardfault_args[3]) ;
  stacked_r12 = ((unsigned long)hardfault_args[4]) ;
  stacked_lr = ((unsigned long)hardfault_args[5]) ;
  stacked_pc = ((unsigned long)hardfault_args[6]) ;
  stacked_psr = ((unsigned long)hardfault_args[7]) ;

  // Configurable Fault Status Register
  // Consists of MMSR, BFSR and UFSR
  _CFSR = (*((volatile unsigned long *)(0xE000ED28))) ;

  // Hard Fault Status Register
  _HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;

  // Debug Fault Status Register
  _DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;

  // Auxiliary Fault Status Register
  _AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;

  // Read the Fault Address Registers. These may not contain valid values.
  // Check BFARVALID/MMARVALID to see if they are valid values
  // MemManage Fault Address Register
  _MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;

  // Bus Fault Address Register
  _BFAR = (*((volatile unsigned long *)(0xE000ED38))) ;

  // Bus Fault Status Register
  _BFSR = (*((volatile unsigned long *)(0xE000ED29))) ;

  __asm("BKPT #0\n") ; // Break into the debugger
}

/* The prototype shows it is a naked function - in effect this is just an
assembly function. */
static void HardFault_Handler( void ) __attribute__( ( naked ) );

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
static void HardFault_Handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word HardFault_HandlerCk    \n"
    );
}
