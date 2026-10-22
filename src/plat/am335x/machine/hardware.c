/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <plat/machine/hardware.h>

BOOT_సంకేతం శూన్య initIRQController(శూన్య)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    అయితే (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_సంకేతం శూన్య cpu_initLocalIRQController(శూన్య) {}
