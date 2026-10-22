/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <machine/io.h>
#شامل <kernel/vspace.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/vspace.h>
#شامل <plat/machine.h>
#شامل <linker.h>
#شامل <plat/machine/devices_gen.h>
#شامل <plat/machine/hardware.h>

#تعریف_کرو INTCPS_SYSCONFIG_SOFTRESET ثنائی_ہندسہ(1)
#تعریف_کرو INTCPS_SYSSTATUS_RESETDONE ثنائی_ہندسہ(0)

BOOT_رمز خالی initIRQController(خالی)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    جب (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_رمز خالی cpu_initLocalIRQController(خالی) {}
