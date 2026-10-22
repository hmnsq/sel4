/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <plat/machine.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/devices_gen.h>
#অন্তর্ভাব <plat/machine/hardware.h>

#define INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#define INTCPS_SYSSTATUS_RESETDONE BIT(0)

BOOT_CODE শূন্য initIRQController(শূন্য)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    যাবত্ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE শূন্য cpu_initLocalIRQController(শূন্য) {}
