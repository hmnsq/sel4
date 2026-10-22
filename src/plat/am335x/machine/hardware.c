/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <plat/machine/hardware.h>

BOOT_کوڈ فراغ initIRQController(فراغ)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    طالما (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_کوڈ فراغ cpu_initLocalIRQController(فراغ) {}
