#ifndef HW_S32K358_LPUART_H
#define HW_S32K358_LPUART_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"

/* LPUART Register Offsets */
#define LPUART_BAUD    0x10  /* Baud Rate Register */
#define LPUART_STAT    0x14  /* Status Register */
#define LPUART_CTRL    0x18  /* Control Register */
#define LPUART_DATA    0x1C  /* Data Register */

/* Bit Definitions */
#define LPUART_STAT_TDRE    (1 << 23) /* Transmit Data Register Empty */
#define LPUART_STAT_RDRF    (1 << 21) /* Receive Data Register Full */
#define LPUART_CTRL_TE      (1 << 19) /* Transmitter Enable */
#define LPUART_CTRL_RE      (1 << 18) /* Receiver Enable */

#define TYPE_S32K358_LPUART "s32k358-lpuart"
OBJECT_DECLARE_SIMPLE_TYPE(S32K358LPUARTState, S32K358_LPUART)

struct S32K358LPUARTState {
    SysBusDevice parent_obj;

    MemoryRegion mmio;   /* MMIO region */
    qemu_irq irq;        /* IRQ line */

    uint32_t baud;       /* Baud Rate Register */
    uint32_t stat;       /* Status Register */
    uint32_t ctrl;       /* Control Register */
    uint32_t data;       /* Data Register */

    CharBackend chr;     /* Character backend for UART */
};

#endif /* HW_S32K358_LPUART_H */

