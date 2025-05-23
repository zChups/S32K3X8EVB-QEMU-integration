#ifndef HW_S32K358_PIT_H
#define HW_S32K358_PIT_H

#include "hw/sysbus.h"
#include "hw/ptimer.h"
#include "hw/qdev-clock.h"
#include "qom/object.h"
#include "hw/clock.h"


/* PIT Register Offsets */
#define PIT_MCR      0x00   /* Module Control Register */
#define PIT_LDVAL0   0x100  /* Timer Load Value Register (Channel 0) */
#define PIT_CVAL0    0x104  /* Current Timer Value Register (Channel 0) */
#define PIT_TCTRL0   0x108  /* Timer Control Register (Channel 0) */
#define PIT_TFLG0    0x10C  /* Timer Flag Register (Channel 0) */

/* Timer Control Flags */
#define PIT_TCTRL_TEN  (1 << 0) /* Timer Enable */
#define PIT_TCTRL_TIE  (1 << 1) /* Timer Interrupt Enable */

#define TYPE_S32K358_PIT "s32k358-pit"
OBJECT_DECLARE_SIMPLE_TYPE(S32K358PITState, S32K358_PIT)

struct S32K358PITState {
    /* QEMU object hierarchy */
    SysBusDevice parent_obj;

    /* Memory-mapped registers and IRQ */
    MemoryRegion iomem;
    qemu_irq irq;

    /* Clock and timer management */
    struct ptimer_state *timer;
    Clock *pclk;

    /* PIT registers */
    uint32_t mcr;     /* Module Control Register */
    uint32_t ldval;   /* Load Value Register */
    uint32_t tctrl;   /* Timer Control Register */
    uint32_t tflg;    /* Timer Flag Register */
};

#endif /* HW_S32K358_PIT_H */

