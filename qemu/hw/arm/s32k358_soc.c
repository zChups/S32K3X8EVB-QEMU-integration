#include "qemu/osdep.h"
#include "exec/address-spaces.h"
#include "hw/arm/s32k358_soc.h"
#include "hw/qdev-clock.h"
#include "chardev/char.h"
#include "qemu/error-report.h"
#include "hw/arm/armv7m.h"
#include "qapi/error.h"
#include "sysemu/sysemu.h"
#include "hw/misc/unimp.h"
#include "qemu/log.h"

static void s32k358_soc_initfn(Object *obj) {
    qemu_log("Initializing S32K358 SoC\n");
    
    S32K358State *s = S32K358_SOC(obj);

    /* Initialize ARM Cortex-M7 core */
    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);

    /* Initialize PIT (Periodic Interrupt Timer) */
    object_initialize_child(obj, "pit", &s->pit, TYPE_S32K358_PIT);
    qemu_log("Initialized PIT child device\n");


    /* Initialize LPUART instances (8 modules) */
    for (int i = 0; i < NUM_LPUART; i++) {
        object_initialize_child(obj, "lpuart[*]", &s->lpuart[i], TYPE_S32K358_LPUART);
    }

    /* Initialize system clock */
    s->sysclk = qdev_init_clock_in(DEVICE(s), "sysclk", NULL, NULL, 0);
}

static void s32k358_soc_realize(DeviceState *dev_soc, Error **errp) {
	
    qemu_log("Realizing S32K358 SoC\n");

    S32K358State *s = S32K358_SOC(dev_soc);
    MemoryRegion *system_memory = get_system_memory();
    DeviceState *dev;
    SysBusDevice *busdev;
    Error *err = NULL;

    /* Verify system clock */
    if (!clock_has_source(s->sysclk)) {
        error_setg(errp, "sysclk clock must be wired up by the board code");
        return;
    }

    /* Flash memory setup */
    memory_region_init_rom(&s->flash, OBJECT(dev_soc), "S32K358.flash", FLASH_SIZE, errp);
    if (err) {
        error_propagate(errp, err);
        return;
    }
    
    memory_region_init_alias(&s->flash_alias, OBJECT(dev_soc),
                             "S32K358.flash.alias", &s->flash, 0,
                             FLASH_SIZE);

    memory_region_add_subregion(system_memory, FLASH_BASE_ADDR, &s->flash);
    memory_region_add_subregion(system_memory, 0, &s->flash_alias);

    /* SRAM memory setup */
    memory_region_init_ram(&s->sram, NULL, "S32K358.sram", SRAM_SIZE, errp);
    if (err) {
        error_propagate(errp, err);
        return;
    }
    memory_region_add_subregion(system_memory, SRAM_BASE_ADDR, &s->sram);

    /* Realize ARMv7M CPU */
    dev = DEVICE(&s->armv7m);
    qdev_prop_set_uint32(dev, "num-irq", 240); /* 240 IRQ lines */
    qdev_prop_set_uint8(dev, "num-prio-bits", 4);
    qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m7"));
    qdev_prop_set_bit(dev, "enable-bitband", true);
    qdev_connect_clock_in(dev, "cpuclk", s->sysclk);
    object_property_set_link(OBJECT(&s->armv7m), "memory", OBJECT(system_memory), &error_abort);

    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp)) {
        return;
    }
    qemu_log("Realized ARMv7M CPU\n"); 
    
     /* Realize all LPUART instances (16 modules) */
    const int lpuart_irq[NUM_LPUART] = {
        LPUART0_IRQ, LPUART1_IRQ, LPUART2_IRQ, LPUART3_IRQ,
        LPUART4_IRQ, LPUART5_IRQ, LPUART6_IRQ, LPUART7_IRQ
    };

    for (int i = 0; i < NUM_LPUART; i++) {
        dev = DEVICE(&s->lpuart[i]);

        /* Set up the chardev connection */
        qdev_prop_set_chr(dev, "chardev", serial_hd(i));

        if (!sysbus_realize(SYS_BUS_DEVICE(&s->lpuart[i]), errp)) {
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, LPUART_BASE_ADDR + (i * 0x4000));
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(DEVICE(&s->armv7m), lpuart_irq[i]));
    }
    
    qemu_log("Realized UART\n");
    
    /* Realize PIT (Periodic Interrupt Timer) */
    dev = DEVICE(&s->pit);
    qdev_connect_clock_in(dev, "pclk", s->sysclk); // Connect the clock to the PIT
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->pit), errp)) {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    
    qemu_log("PIT: pclk connected with frequency: %u Hz\n", clock_get_hz(s->sysclk));
    sysbus_mmio_map(busdev, 0, PIT_BASE_ADDR);
    sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(DEVICE(&s->armv7m), PIT_IRQ));
    
    qemu_log("Realized PIT\n"); 
    
   
}

static void s32k358_soc_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->realize = s32k358_soc_realize;
}

static const TypeInfo s32k358_soc_info = {
    .name = TYPE_S32K358_SOC,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(S32K358State),
    .instance_init = s32k358_soc_initfn,
    .class_init = s32k358_soc_class_init,
};

static void s32k358_soc_register_types(void) {
    type_register_static(&s32k358_soc_info);
}

type_init(s32k358_soc_register_types);


