#include "lpuart.h"


void my_printf(const char *str) {
    while (*str) {
        while (!(LPUART_STAT & LPUART_STAT_TDRE)) {
            // Wait until the transmit buffer is empty
        }
        LPUART_DATA = *str++; // Send the character
    }
}

void lpuart_init(void){
	
	LPUART_BAUD = 0x1A0;                 // Example: Set baud rate to a reasonable default
    	LPUART_CTRL |= (LPUART_CTRL_TE | LPUART_CTRL_RE); // Enable transmitter and receiver
    	//The |= operator ensures that these bits (TE and RE) are set while preserving other control bits in the LPUART_CTRL register.
	//This step is critical for initializing the LPUART peripheral, as it ensures the module is ready to handle transmit and receive operations.
}

