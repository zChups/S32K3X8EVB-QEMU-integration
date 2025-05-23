#include <stdint.h>
#ifndef PIT_H
#define PIT_H

// Base address for the PIT peripheral
#define PIT_BASE_ADDR       0x400B0000  // Updated base address for S32K358

// Offsets for PIT registers
#define PIT_MCR             (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x00))  /* PIT Control Register */
#define PIT_LDVAL0          (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x100)) /* Timer Load Value Register (Channel 0) */
#define PIT_CVAL0           (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x104)) /* Current Timer Value Register (Channel 0) */
#define PIT_TCTRL0          (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x108)) /* Timer Control Register for Channel 0 */
#define PIT_TFLG0           (*(volatile uint32_t *)(PIT_BASE_ADDR + 0x10C)) /* Timer Flag Register for Channel 0 */

// Timer Control Flags
#define PIT_TCTRL_TEN       (1 << 0)    /* Timer Enable */
#define PIT_TCTRL_TIE       (1 << 1)    /* Timer Interrupt Enable */

#define PIT_IRQ 96

//base address for MC_ME module, to enable PIT
#define MC_ME_BASE 	    0x402DC000 

//offsets fot MC_ME register (according to the reference manual: section 46.7.1)
#define MC_ME_PRTN0_COFB1_CLKEN		(*(volatile uint32_t *)(MC_ME_BASE + 0x134))



// Base address for the NVIC
#define NVIC_BASE	    0xE000E100

// Offsets for NVIC registers (there is another registers, checks we should ad it)
#define NVIC_ISER3	    (*(volatile uint32_t *)(NVIC_BASE + 0x0C))
#define NVIC_ICER3          (*(volatile uint32_t *)(NVIC_BASE + 0x8C))
#define NVIC_ISPR3          (*(volatile uint32_t *)(NVIC_BASE + 0x20C))
#define NVIC_ICPR3          (*(volatile uint32_t *)(NVIC_BASE + 0x28C))
#define NVIC_IABR3          (*(volatile uint32_t *)(NVIC_BASE + 0x30C))
#define NVIC_IPR24          (*(volatile uint32_t *)(NVIC_BASE + 0x460))

/*NVIC methods*/
void NVIC_EnableIRQ(void);

void NVIC_DisableIRQ(void);

void NVIC_SetPriority(uint8_t priority);

/*PIT methods*/
void pit_init();

void pit_disable(void);

void pit_handler(void);

#endif /* PIT_H */
