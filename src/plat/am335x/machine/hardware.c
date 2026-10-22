/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <plat/machine/hardware.h>

BOOT_CODE शून्य initIRQController(शून्य)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    जाव (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE शून्य cpu_initLocalIRQController(शून्य) {}
