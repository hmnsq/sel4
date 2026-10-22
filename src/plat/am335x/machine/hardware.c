/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <plat/machine/hardware.h>

BOOT_CODE व्योम initIRQController(व्योम)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    जबतक (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE व्योम cpu_initLocalIRQController(व्योम) {}
