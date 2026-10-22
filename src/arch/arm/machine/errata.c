/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <api/types.h>
#شامل <arch/machine.h>
#شامل <arch/machine/hardware.h>
#شامل <util.h>

/* Prototyped here as this is referenced from assembly */
خالی arm_errata(خالی);

#اگر_تعریف_شدہ CONFIG_ARM_ERRATA_773022
/*
 * There is an errata for Cortex-A15 up to r0p4 where the loop buffer
 * may deliver incorrect instructions. The work around is to disable
 * the loop buffer. Errata is number 773022.
 */
BOOT_رمز ساکن خالی errata_armA15_773022(خالی)
{
    /* Fetch the processor primary part number. */
    uint32_t proc_id = getProcessorID();
    uint32_t variant = (proc_id >> 20) & نقاب(4);
    uint32_t revision = proc_id & نقاب(4);
    uint32_t part = (proc_id >> 4) & نقاب(12);

    /* Check that we are running A15 and a revision upto r0p4. */
    اگر (part == 0xc0f && variant == 0 && revision <= 4) {
        /* Disable loop buffer in the auxiliary control register */
        write_Auxiliary_Control_رجسٹر(
            read_Auxiliary_Control_رجسٹر() | ثنائی_ہندسہ(1));
    }
}
#اگر_ختم

BOOT_رمز خالی VISIBLE arm_errata(خالی)
{
#اگر_تعریف_شدہ CONFIG_ARM_ERRATA_773022
    errata_armA15_773022();
#اگر_ختم
}

