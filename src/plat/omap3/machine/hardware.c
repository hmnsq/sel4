/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <kernel/vspace.h>
#অন্তর্ভাৱ <arch/machine.h>
#অন্তর্ভাৱ <arch/kernel/vspace.h>
#অন্তর্ভাৱ <plat/machine.h>
#অন্তর্ভাৱ <linker.h>
#অন্তর্ভাৱ <plat/machine/devices_gen.h>
#অন্তর্ভাৱ <plat/machine/hardware.h>

#define INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#define INTCPS_SYSSTATUS_RESETDONE BIT(0)

BOOT_CODE শূন্য initIRQController(শূন্য)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    যেতিয়ালৈ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE শূন্য cpu_initLocalIRQController(শূন্য) {}
