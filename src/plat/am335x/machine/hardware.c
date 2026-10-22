/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <plat/machine/hardware.h>

BOOT_CODE ശൂന്യം initIRQController(ശൂന്യം)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    ആകുമ്പോൾ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE ശൂന്യം cpu_initLocalIRQController(ശൂന്യം) {}
