/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/kernel/vspace.h>
#അന്തര്ഭാവ <plat/machine.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <plat/machine/devices_gen.h>
#അന്തര്ഭാവ <plat/machine/hardware.h>

#define INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#define INTCPS_SYSSTATUS_RESETDONE BIT(0)

BOOT_CODE ശൂന്യം initIRQController(ശൂന്യം)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    ആകുമ്പോൾ (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE ശൂന്യം cpu_initLocalIRQController(ശൂന്യം) {}
