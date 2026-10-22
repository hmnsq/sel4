/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <api/types.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/machine/hardware.h>
#అన్తర్భావ <util.h>

/* Prototyped here as this is referenced from assembly */
శూన్య arm_errata(శూన్య);

#నిర్వచితమైతే CONFIG_ARM_ERRATA_773022
/*
 * There is an errata for Cortex-A15 up to r0p4 where the loop buffer
 * may deliver incorrect instructions. The work around is to disable
 * the loop buffer. Errata is number 773022.
 */
BOOT_సంకేతం స్థిర శూన్య errata_armA15_773022(శూన్య)
{
    /* Fetch the processor primary part number. */
    uint32_t proc_id = getProcessorID();
    uint32_t variant = (proc_id >> 20) & ముసుగు(4);
    uint32_t revision = proc_id & ముసుగు(4);
    uint32_t part = (proc_id >> 4) & ముసుగు(12);

    /* Check that we are running A15 and a revision upto r0p4. */
    ఒకవేళ (part == 0xc0f && variant == 0 && revision <= 4) {
        /* Disable loop buffer in the auxiliary control register */
        write_Auxiliary_Control_రిజిస్టరు(
            read_Auxiliary_Control_రిజిస్టరు() | బిట్టు(1));
    }
}
#అయితే_ముగింపు

BOOT_సంకేతం శూన్య VISIBLE arm_errata(శూన్య)
{
#నిర్వచితమైతే CONFIG_ARM_ERRATA_773022
    errata_armA15_773022();
#అయితే_ముగింపు
}

