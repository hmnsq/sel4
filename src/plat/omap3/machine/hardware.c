/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <machine/io.h>
#ضمن <kernel/vspace.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <plat/machine.h>
#ضمن <linker.h>
#ضمن <plat/machine/devices_gen.h>
#ضمن <plat/machine/hardware.h>

#define INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#define INTCPS_SYSSTATUS_RESETDONE BIT(0)

BOOT_CODE فراغ initIRQController(فراغ)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    כלעוד (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE فراغ cpu_initLocalIRQController(فراغ) {}
