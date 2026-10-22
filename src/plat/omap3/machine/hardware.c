/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <machine/io.h>
#شامل <kernel/vspace.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/vspace.h>
#شامل <plat/machine.h>
#شامل <linker.h>
#شامل <plat/machine/devices_gen.h>
#شامل <plat/machine/hardware.h>

#define INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#define INTCPS_SYSSTATUS_RESETDONE BIT(0)

BOOT_CODE تهی initIRQController(تهی)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    وقتی (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE تهی cpu_initLocalIRQController(تهی) {}
