/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initIRQController(ಶೂನ್ಯ)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    ಆಗ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ cpu_initLocalIRQController(ಶೂನ್ಯ) {}
