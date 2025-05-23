#include <stdint.h>
#ifndef CLOCK_H
#define CLOCK_H

//Base address for MC_CGM 
#define MC_CGM_BASE		0x402D8000

//Offsets for MC_GCM registers
#define MC_CGM_MUX_0_CSC	(*(volatile uint32_t *)(MC_CGM_BASE + 0x300))
#define MC_CGM_MUX_0_DC_0	(*(volatile uint32_t *)(MC_CGM_BASE + 0x308))

//Base address for HSE_B configuration_GPR 
#define HSE_B_BASE 		0x4039C000

//Offsets fot HSE_B configuration_GPR registers
#define HSE_B_CONFIG_REG_GPR	(*(volatile uint32_t *)(HSE_B_BASE + 0x64))

//Configure CORE_CLK and prescaler
void clock_config(void);
void set_prescaler(uint8_t div);

#endif /* CLOCK_H */
