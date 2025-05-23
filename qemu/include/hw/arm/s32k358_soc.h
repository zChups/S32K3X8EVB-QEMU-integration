#ifndef HW_ARM_S32K358_SOC_H
#define HW_ARM_S32K358_SOC_H

#include "hw/timer/s32k358_pit.h"
#include "hw/char/s32k358_lpuart.h"
#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/arm/armv7m.h"
#include "hw/or-irq.h"

#define TYPE_S32K358_SOC "s32k358-soc"
OBJECT_DECLARE_SIMPLE_TYPE(S32K358State, S32K358_SOC)

/* Number of LPUART modules */
#define NUM_LPUART 8

/* Base addresses */
#define LPUART_BASE_ADDR 0x40328000
#define PIT_BASE_ADDR    0x400B0000

/* Memory sizes and base addresses */
#define SRAM_BASE_ADDR  0x20400000
#define SRAM_SIZE       0x00040000 	// 256 KB SRAM


#define FLASH_BASE_ADDR  0x00400000   // Start address for flash memory, should be 0x00400000
#define FLASH_SIZE       0x00200000   // 4 blocks of 2 MB size

/* IRQ Lines */
#define LPUART0_IRQ 141
#define LPUART1_IRQ 142
#define LPUART2_IRQ 143
#define LPUART3_IRQ 144
#define LPUART4_IRQ 145
#define LPUART5_IRQ 146
#define LPUART6_IRQ 147
#define LPUART7_IRQ 148


#define PIT_IRQ     96

struct S32K358State {
    SysBusDevice parent_obj;

    /* ARMv7M CPU */
    ARMv7MState armv7m;

    /* Peripherals */
    S32K358LPUARTState lpuart[NUM_LPUART];
    S32K358PITState pit;

    /* Memory regions */
    MemoryRegion flash;
    MemoryRegion flash_alias;
    MemoryRegion sram;

    Clock *sysclk;
};

#endif /* HW_ARM_S32K358_SOC_H */






