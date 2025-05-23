#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "qemu/error-report.h"
#include "hw/arm/boot.h"
#include "hw/arm/s32k358_soc.h"

#include "qemu/log.h"

/* Define the main SYSCLK frequency */
//Based on the S32K358 documentation, FIRC (Fast Internal Reference Clock) is the default system clock source, and it operates at 48 MHz.
#define SYSCLK_FRQ 48000000ULL /* 48 MHz */


static void s32k3x8evb_init(MachineState *machine)
{
    DeviceState *soc_dev;
    Clock *sysclk;

    /* Initialize the fixed-frequency clock */
    sysclk = clock_new(OBJECT(machine), "SYSCLK");
    clock_set_hz(sysclk, SYSCLK_FRQ);

    /* Create the SoC device */
    soc_dev = qdev_new(TYPE_S32K358_SOC);
    object_property_add_child(OBJECT(machine), "soc", OBJECT(soc_dev));
    qdev_connect_clock_in(soc_dev, "sysclk", sysclk);
    qemu_log("SYSCLK connected with frequency: %u Hz\n", clock_get_hz(sysclk));

    sysbus_realize_and_unref(SYS_BUS_DEVICE(soc_dev), &error_fatal);
    qemu_log("Realized S32K358 SoC\n");


    /* Load the firmware if specified */
    if (machine->kernel_filename) {
        armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename, 0, FLASH_SIZE); 
    }
}

static void s32k3x8evb_machine_init(MachineClass *mc)
{
    static const char *const valid_cpu_types[] = {
        ARM_CPU_TYPE_NAME("cortex-m7"), /* S32K358 has a Cortex-M7 core */
        NULL
    };

    mc->desc = "S32K3X8EVB-Q289 Machine (Cortex-M7)";
    mc->init = s32k3x8evb_init;
    mc->valid_cpu_types = valid_cpu_types;
}

DEFINE_MACHINE("s32k3x8evb", s32k3x8evb_machine_init);


