/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices_gen.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTCPS_SYSCONFIG_SOFTRESET ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTCPS_SYSSTATUS_RESETDONE ਦੋਆਧਾਰੀ_ਅੰਕ(0)

BOOT_ਕੋਡ ਖਾਲੀ initIRQController(ਖਾਲੀ)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    ਜਦੋਂ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_ਕੋਡ ਖਾਲੀ cpu_initLocalIRQController(ਖਾਲੀ) {}
