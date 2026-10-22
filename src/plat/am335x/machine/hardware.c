/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <plat/machine/hardware.h>

BOOT_CODE શૂન્ય initIRQController(શૂન્ય)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    જ્યારે (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE શૂન્ય cpu_initLocalIRQController(શૂન્ય) {}
