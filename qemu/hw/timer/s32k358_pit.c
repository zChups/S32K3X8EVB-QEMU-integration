#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "hw/registerfields.h"
#include "hw/qdev-clock.h"
#include "hw/timer/s32k358_pit.h"
#include "qapi/error.h"
#include "trace.h"

#include "qemu/log.h"
#include "qemu/module.h"


/* Debug Printing */
#ifndef PIT_ERR_DEBUG
#define PIT_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (PIT_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)


/* PIT Timer Tick Callback */
static void s32k358_pit_tick(void *opaque) {
    S32K358PITState *s = S32K358_PIT(opaque);
    //qemu_log("PIT: timer reached 0, i'm in the callback function\n");
    /* Check if interrupt is enabled */
    if (s->tctrl & PIT_TCTRL_TIE) {
        s->tflg |= 1; /* Set interrupt flag */
        //qemu_log("PIT: sending interrupt\n");
        qemu_irq_pulse(s->irq); /* Trigger interrupt */
        //qemu_set_irq(s->irq, 0);
    }
}

/* Clock Update Callback */
static void s32k358_pit_clk_update(void *opaque, ClockEvent event) {
    S32K358PITState *s = S32K358_PIT(opaque);

    ptimer_transaction_begin(s->timer);
    ptimer_set_period_from_clock(s->timer, s->pclk, 1);
    ptimer_transaction_commit(s->timer);
}

/* Register Read */
static uint64_t s32k358_pit_read(void *opaque, hwaddr offset, unsigned size) {
    S32K358PITState *s = S32K358_PIT(opaque);

    switch (offset) {
    case PIT_MCR:
        return s->mcr;
    case PIT_LDVAL0:
        return s->ldval;
    case PIT_CVAL0:
        return ptimer_get_count(s->timer);
    case PIT_TCTRL0:
        return s->tctrl;
    case PIT_TFLG0:
        return s->tflg;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "Invalid read offset 0x%" HWADDR_PRIx "\n", offset);
        return 0;
    }
}

/* Register Write */
static void s32k358_pit_write(void *opaque, hwaddr offset, uint64_t value, unsigned size) {
    S32K358PITState *s = S32K358_PIT(opaque);

    switch (offset) {
    case PIT_MCR:
        s->mcr = value & 0xFFFFFFFD; /* Only handle MDIS and FRZ bits */
        if (s->mcr & 0x2) {
            ptimer_stop(s->timer);
        }
        break;
    case PIT_LDVAL0:
        s->ldval = value;
        ptimer_transaction_begin(s->timer);
        ptimer_set_limit(s->timer, value + 1, 1);
        ptimer_transaction_commit(s->timer);
        break;
    case PIT_TCTRL0:
        s->tctrl = value & 0x3; /* Mask to valid bits */
        ptimer_transaction_begin(s->timer);
        if (s->tctrl & PIT_TCTRL_TEN) {
            //qemu_log("PIT: calling ptimer_run\n");
            ptimer_run(s->timer, ptimer_get_limit(s->timer) == 0);
        } else {
            ptimer_stop(s->timer);
        }
        ptimer_transaction_commit(s->timer);
        break;
    case PIT_TFLG0:
        if (value & 1) {
            s->tflg = 0; /* Clear interrupt flag */
        }
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "Invalid write offset 0x%" HWADDR_PRIx "\n", offset);
        break;
    }
}

/* Memory Region Operations */
static const MemoryRegionOps s32k358_pit_ops = {
    .read = s32k358_pit_read,
    .write = s32k358_pit_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/* Device Reset */
static void s32k358_pit_reset(DeviceState *dev) {
    S32K358PITState *s = S32K358_PIT(dev);

    s->mcr = 0x6; /* Default: PIT disabled */
    s->ldval = 0xFFFFFFFF; /* Max load value */
    s->tctrl = 0; /* Disable timer */
    s->tflg = 0; /* Clear interrupt flag */

    ptimer_transaction_begin(s->timer);
    ptimer_stop(s->timer);
    ptimer_transaction_commit(s->timer);
}

/* Device Initialization */
static void s32k358_pit_init(Object *obj) {
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    S32K358PITState *s = S32K358_PIT(obj);
    
    qemu_log("Initializing PIT device\n");
    
    memory_region_init_io(&s->iomem, obj, &s32k358_pit_ops, s, "s32k358-pit", 0x200);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);

    s->pclk = qdev_init_clock_in(DEVICE(s), "pclk",
                                 s32k358_pit_clk_update, s, ClockUpdate);
                                 
    qemu_log("PIT initialized successfully\n");
   
}

/* Device Realization */
static void s32k358_pit_realize(DeviceState *dev, Error **errp) {
    S32K358PITState *s = S32K358_PIT(dev);
    
    qemu_log("Realizing PIT device\n");

    if (!clock_has_source(s->pclk)) {
        error_setg(errp, "S32K358 PIT: clock input must be connected");
        return;
    }
	
    s->timer = ptimer_init(s32k358_pit_tick, s,
                       PTIMER_POLICY_WRAP_AFTER_ONE_PERIOD |
                       PTIMER_POLICY_TRIGGER_ONLY_ON_DECREMENT |
                       PTIMER_POLICY_NO_IMMEDIATE_RELOAD |
                       PTIMER_POLICY_NO_COUNTER_ROUND_DOWN);


    
    ptimer_transaction_begin(s->timer);
    ptimer_set_period_from_clock(s->timer, s->pclk, 1);
    ptimer_transaction_commit(s->timer);
    
    qemu_log("PIT realized successfully\n");
}

/* Device Class Initialization */
static void s32k358_pit_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = s32k358_pit_realize;
    device_class_set_legacy_reset(dc, s32k358_pit_reset);
}

/* Device Type Registration */
static const TypeInfo s32k358_pit_info = {
    .name = TYPE_S32K358_PIT,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(S32K358PITState),
    .instance_init = s32k358_pit_init,
    .class_init = s32k358_pit_class_init,
};

static void s32k358_pit_register_types(void) {
    type_register_static(&s32k358_pit_info);
}
type_init(s32k358_pit_register_types);


