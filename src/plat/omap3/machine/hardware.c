/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <plat/machine.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/devices_gen.h>
#అన్తర్భావ <plat/machine/hardware.h>

#నిర్వచించు INTCPS_SYSCONFIG_SOFTRESET బిట్టు(1)
#నిర్వచించు INTCPS_SYSSTATUS_RESETDONE బిట్టు(0)

BOOT_సంకేతం శూన్య initIRQController(శూన్య)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    అయితే (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_సంకేతం శూన్య cpu_initLocalIRQController(శూన్య) {}
