/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>

BOOT_ਕੋਡ ਖਾਲੀ initIRQController(ਖਾਲੀ)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    ਜਦੋਂ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_ਕੋਡ ਖਾਲੀ cpu_initLocalIRQController(ਖਾਲੀ) {}
