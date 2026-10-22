/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <machine/io.h>
#समावेश <kernel/vspace.h>
#समावेश <arch/machine.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <plat/machine.h>
#समावेश <linker.h>
#समावेश <plat/machine/devices_gen.h>
#समावेश <plat/machine/hardware.h>

#घोषणा INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#घोषणा INTCPS_SYSSTATUS_RESETDONE BIT(0)

BOOT_CODE व्योम initIRQController(व्योम)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    जबतक (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE व्योम cpu_initLocalIRQController(व्योम) {}
