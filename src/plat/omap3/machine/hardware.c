/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <plat/machine.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>

#ವ್ಯಾಖ್ಯಾನಿಸು INTCPS_SYSCONFIG_SOFTRESET ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು INTCPS_SYSSTATUS_RESETDONE ಬಿಟ್(0)

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initIRQController(ಶೂನ್ಯ)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    ಆಗ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ cpu_initLocalIRQController(ಶೂನ್ಯ) {}
