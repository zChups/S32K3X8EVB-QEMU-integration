#include <stdint.h>
#include "clock.h"

void clock_config(void){
	MC_CGM_MUX_0_CSC |= ~(0xF << 24U); //set SELCTL field to 0: FIRC clock selected
	HSE_B_CONFIG_REG_GPR |= (0x3 << 0U); //set FIRC_DIV_SEL BIT: undivided, it is the same as the FIRC clock frequency
}

void set_prescaler(uint8_t div){
	//since the register is made by 3 bits, we truncate the uint8_t value 
	uint32_t value_limited = (div & ((1U << 3) -1));
       	//set the DIV field to a value passed to the function 	
	MC_CGM_MUX_0_DC_0 |= (value_limited << 16);
}	
