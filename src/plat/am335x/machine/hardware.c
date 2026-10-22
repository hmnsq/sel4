/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <plat/machine/hardware.h>

BOOT_CODE শূন্য initIRQController(শূন্য)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    যেতিয়ালৈ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE শূন্য cpu_initLocalIRQController(শূন্য) {}
