/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <plat/machine/hardware.h>

BOOT_CODE تهی initIRQController(تهی)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    وقتی (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE تهی cpu_initLocalIRQController(تهی) {}
