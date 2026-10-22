/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <api/types.h>
#ضمن <arch/machine.h>
#ضمن <arch/machine/hardware.h>
#ضمن <util.h>

/* Prototyped here as this is referenced from assembly */
فراغ arm_errata(فراغ);

#اگر_تعریف_شده CONFIG_ARM_ERRATA_773022
/*
 * There is an errata for Cortex-A15 up to r0p4 where the loop buffer
 * may deliver incorrect instructions. The work around is to disable
 * the loop buffer. Errata is number 773022.
 */
BOOT_کد ساكن فراغ errata_armA15_773022(فراغ)
{
    /* Fetch the processor primary part number. */
    uint32_t proc_id = getProcessorID();
    uint32_t variant = (proc_id >> 20) & ماسک(4);
    uint32_t revision = proc_id & ماسک(4);
    uint32_t part = (proc_id >> 4) & ماسک(12);

    /* Check that we are running A15 and a revision upto r0p4. */
    إذا (part == 0xc0f && variant == 0 && revision <= 4) {
        /* Disable loop buffer in the auxiliary control register */
        write_Auxiliary_Control_ثبات(
            read_Auxiliary_Control_ثبات() | بیت(1));
    }
}
#پایان_اگر

BOOT_کد فراغ VISIBLE arm_errata(فراغ)
{
#اگر_تعریف_شده CONFIG_ARM_ERRATA_773022
    errata_armA15_773022();
#پایان_اگر
}

