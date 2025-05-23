#include "qemu/osdep.h"
#include "hw/char/s32k358_lpuart.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"

/* Debug Printing */
#ifndef LPUART_ERR_DEBUG
#define LPUART_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (LPUART_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ## args)

static int s32k358_lpuart_can_receive(void *opaque) {
    S32K358LPUARTState *s = opaque;
    return !(s->stat & LPUART_STAT_RDRF);
}

static void s32k358_lpuart_receive(void *opaque, const uint8_t *buf, int size) {
    S32K358LPUARTState *s = opaque;

    if (!(s->ctrl & LPUART_CTRL_RE)) {
        DB_PRINT("Receiver not enabled; dropping data.\n");
        return;
    }

    s->data = *buf;
    s->stat |= LPUART_STAT_RDRF; /* Set the Receive Data Register Full flag */
    qemu_set_irq(s->irq, 1);     /* Trigger interrupt */
}

static uint64_t s32k358_lpuart_read(void *opaque, hwaddr addr, unsigned size) {
    S32K358LPUARTState *s = opaque;

    switch (addr) {
    case LPUART_BAUD:
        return s->baud;
    case LPUART_STAT:
        return s->stat;
    case LPUART_CTRL:
        return s->ctrl;
    case LPUART_DATA:
        s->stat &= ~LPUART_STAT_RDRF; /* Clear RXNE flag */
        return s->data;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "[lpuart] - Invalid read offset: 0x%" HWADDR_PRIx "\n", addr);
        return 0;
    }
}

static void s32k358_lpuart_write(void *opaque, hwaddr addr, uint64_t val, unsigned size) {
    S32K358LPUARTState *s = opaque;

    switch (addr) {
    case LPUART_BAUD:
        s->baud = val;
        break;
    case LPUART_CTRL:
        s->ctrl = val;
        if (s->ctrl & LPUART_CTRL_TE) {
            s->stat |= LPUART_STAT_TDRE;
        }
        break;
    case LPUART_DATA:
        if (!(s->ctrl & LPUART_CTRL_TE)) {
            DB_PRINT("Transmitter not enabled; dropping data.\n");
            return;
        }
        qemu_chr_fe_write_all(&s->chr, (uint8_t *)&val, 1);
        s->stat |= LPUART_STAT_TDRE; /* Set TDRE after sending data */
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "Invalid write offset: 0x%" HWADDR_PRIx "\n", addr);
    }
}

static const MemoryRegionOps s32k358_lpuart_ops = {
    .read = s32k358_lpuart_read,
    .write = s32k358_lpuart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property s32k358_lpuart_properties[] = {
    DEFINE_PROP_CHR("chardev", S32K358LPUARTState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void s32k358_lpuart_init(Object *obj)
{
    S32K358LPUARTState *s = S32K358_LPUART(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &s32k358_lpuart_ops, s, "s32k358-lpuart", 0x20);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void s32k358_lpuart_realize(DeviceState *dev, Error **errp) {
    S32K358LPUARTState *s = S32K358_LPUART(dev);

    // Initialize LPUART registers
    s->ctrl = 0;
    s->stat = 0;
    s->baud = 0x1A0; // Example: Default baud rate configuration
    s->data = 0;

    qemu_chr_fe_set_handlers(&s->chr,
                             s32k358_lpuart_can_receive,
                             s32k358_lpuart_receive,
                             NULL, NULL,
                             s, NULL, true);
}

static void s32k358_lpuart_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = s32k358_lpuart_realize;
    device_class_set_props(dc, s32k358_lpuart_properties);
}


static const TypeInfo s32k358_lpuart_info = {
    .name = TYPE_S32K358_LPUART,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(S32K358LPUARTState),
    .instance_init = s32k358_lpuart_init,
    .class_init = s32k358_lpuart_class_init,
};


static void s32k358_lpuart_register_types(void) {
    type_register_static(&s32k358_lpuart_info);
}

type_init(s32k358_lpuart_register_types)


