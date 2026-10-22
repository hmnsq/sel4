/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <plat/machine.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/devices_gen.h>
#अन्तर्भाव <plat/machine/hardware.h>

#define INTCPS_SYSCONFIG_SOFTRESET BIT(1)
#define INTCPS_SYSSTATUS_RESETDONE BIT(0)

BOOT_CODE शून्य initIRQController(शून्य)
{
    intc->intcps_sysconfig = INTCPS_SYSCONFIG_SOFTRESET;
    पर्यंत (!(intc->intcps_sysstatus & INTCPS_SYSSTATUS_RESETDONE)) ;
}

BOOT_CODE शून्य cpu_initLocalIRQController(शून्य) {}
