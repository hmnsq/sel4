/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/hardware.h>

BOOT_CODE ଶୂନ୍ୟ initIRQController(ଶୂନ୍ୟ)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    ଯେତେବେଳେ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE ଶୂନ୍ୟ cpu_initLocalIRQController(ଶୂନ୍ୟ) {}
