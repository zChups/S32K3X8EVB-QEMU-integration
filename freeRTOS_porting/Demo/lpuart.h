#ifndef LPUART_H
#define LPUART_H

#include <stdint.h>

// Base address for LPUART0
#define LPUART0_BASE_ADDR   0x40328000

// Offsets for LPUART registers
#define LPUART_BAUD         (*(volatile uint32_t *)(LPUART0_BASE_ADDR + 0x10)) /* Baud Rate Register */
#define LPUART_STAT         (*(volatile uint32_t *)(LPUART0_BASE_ADDR + 0x14)) /* Status Register */
#define LPUART_CTRL         (*(volatile uint32_t *)(LPUART0_BASE_ADDR + 0x18)) /* Control Register */
#define LPUART_DATA         (*(volatile uint32_t *)(LPUART0_BASE_ADDR + 0x1C)) /* Data Register */


// Bit definitions for LPUART_CTRL
#define LPUART_STAT_TDRE    (1 << 23) /* Transmit Data Register Empty */
#define LPUART_STAT_RDRF    (1 << 21) /* Receive Data Register Full */
#define LPUART_CTRL_TE      (1 << 19) /* Transmitter Enable */
#define LPUART_CTRL_RE      (1 << 18) /* Receiver Enable */

void my_printf(const char *str);

void lpuart_init(void);


#endif 

/*
LPUART_CTRL_TE is a bit in the LPUART_CTRL (Control Register) that enables the UART's ability to send data. 
If this bit is not set, writing to the data register (LPUART_DATA) won't actually transmit any data.

LPUART_CTRL_RE is a bit in the LPUART_CTRL register that enables the UART's ability to receive data. 
If this bit is not set, incoming data won't be captured in the LPUART_DATA register.

1 << 19: This shifts a 1 19 positions to the left, resulting in the bitmask 0x00080000.
1 << 18: This shifts a 1 18 positions to the left, resulting in the bitmask 0x00040000.
*/



