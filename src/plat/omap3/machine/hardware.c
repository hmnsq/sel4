/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <plat/machine.h>
#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <plat/machine/devices_gen.h>
#અન્તર્ભાવ <plat/machine/hardware.h>

#define INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#define INTCPS_SYSSTATUS_RESETDONE BIT(0)

BOOT_CODE શૂન્ય initIRQController(શૂન્ય)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    જ્યારે (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE શૂન્ય cpu_initLocalIRQController(શૂન્ય) {}
