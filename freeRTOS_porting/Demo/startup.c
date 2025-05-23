/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include "lpuart.h"
#include "pit.h"

/* FreeRTOS interrupt handlers. */
extern void vPortSVCHandler( void );
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );

/* Custom PIT interrupt handler. */
extern void PIT_Handler(void); 

/* Exception handlers. */
static void HardFault_Handler( void ) __attribute__( ( naked ) );
static void Default_Handler( void ) __attribute__( ( naked ) );
void Reset_Handler( void ) __attribute__( ( naked ) );

extern int main( void );
extern uint32_t _estack;

/* Vector table. */
const uint32_t* isr_vector[] __attribute__((section(".isr_vector"), used)) =
{
    ( uint32_t * ) &_estack,           		// Initial Stack Pointer (SP)    [0]
    ( uint32_t * ) &Reset_Handler,     		// Initial Program Counter (PC)  [1]
    ( uint32_t * ) &Default_Handler,       	// Non-maskable Interrupt (NMI)  [2]
    ( uint32_t * ) &HardFault_Handler, 		// Hard Fault                    [3]
    ( uint32_t * ) &Default_Handler, 		// MemManage Fault               [4]
    ( uint32_t * ) &Default_Handler,  		// Bus Fault                     [5]
    ( uint32_t * ) &Default_Handler,		// Usage Fault                   [6]
    0,                                 		// RESERVED                      [7]
    0,                                 		// RESERVED                      [8]
    0,                                 		// RESERVED                      [9]
    0,                                 		// RESERVED                      [10]
    ( uint32_t * ) &vPortSVCHandler,       	// Supervisor Call (SVCall)      [11]
    ( uint32_t * ) &Default_Handler,  		// Debug Monitor                 [12]
    0,                                 		// RESERVED                      [13]
    ( uint32_t * ) &xPortPendSVHandler,    	// Pendable request for service  [14]
    ( uint32_t * ) &xPortSysTickHandler,   	// System tick timer (SysTick)   [15]

    /* IRQ entries */
    [16 ... 111] = (uint32_t *) &Default_Handler, /* Default handlers */
    [112] = (uint32_t *) &PIT_Handler,           /* IRQ 112 (96+16): PIT interrupt */
    [113 ... 240] = (uint32_t *) &Default_Handler /* Remaining entries */
};


void Reset_Handler( void )
{
    main();
}

/* Variables used to store the value of registers at the time a hardfault
 * occurs.  These are volatile to try and prevent the compiler/linker optimizing
 * them away as the variables never actually get used. */
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

/* Called from the hardfault handler to provide information on the processor
 * state at the time of the fault.
 */
__attribute__( ( used ) ) void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    my_printf( "Calling prvGetRegistersFromStack() from fault handler" );
    //fflush( stdout );

    /* When the following line is hit, the variables contain the register values. */
    for( ;; );
}


void Default_Handler( void )
{
    __asm volatile
    (
        ".align 8                                \n"
        " ldr r3, =0xe000ed04                    \n" /* Load the address of the interrupt control register into r3. */
        " ldr r2, [r3, #0]                       \n" /* Load the value of the interrupt control register into r2. */
        " uxtb r2, r2                            \n" /* The interrupt number is in the least significant byte - clear all other bits. */
        "Infinite_Loop:                          \n" /* Sit in an infinite loop - the number of the executing interrupt is held in r2. */
        " b  Infinite_Loop                       \n"
        " .ltorg                                 \n"
    );
}

void HardFault_Handler( void )
{
    __asm volatile
    (
        ".align 8                                                   \n"
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, =prvGetRegistersFromStack                         \n"
        " bx r2                                                     \n"
        " .ltorg                                                    \n"
    );
}
