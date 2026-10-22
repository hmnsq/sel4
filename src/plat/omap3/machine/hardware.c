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

#تعریف_کن INTCPS_SYSCONFIG_SOFTRESET بیت(1)
#تعریف_کن INTCPS_SYSSTATUS_RESETDONE بیت(0)

BOOT_کد فراغ initIRQController(فراغ)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    طالما (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_کد فراغ cpu_initLocalIRQController(فراغ) {}
