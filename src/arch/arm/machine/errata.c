/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

/* Prototyped here as this is referenced from assembly */
ਖਾਲੀ arm_errata(ਖਾਲੀ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_ERRATA_773022
/*
 * There is an errata for Cortex-A15 up to r0p4 where the loop buffer
 * may deliver incorrect instructions. The work around is to disable
 * the loop buffer. Errata is number 773022.
 */
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ errata_armA15_773022(ਖਾਲੀ)
{
    /* Fetch the processor primary part number. */
    uint32_t proc_id = getProcessorID();
    uint32_t variant = (proc_id >> 20) & ਨਕਾਬ(4);
    uint32_t revision = proc_id & ਨਕਾਬ(4);
    uint32_t part = (proc_id >> 4) & ਨਕਾਬ(12);

    /* Check that we are running A15 and a revision upto r0p4. */
    ਜੇ (part == 0xc0f && variant == 0 && revision <= 4) {
        /* Disable loop buffer in the auxiliary control register */
        write_Auxiliary_Control_ਰਜਿਸਟਰ(
            read_Auxiliary_Control_ਰਜਿਸਟਰ() | ਦੋਆਧਾਰੀ_ਅੰਕ(1));
    }
}
#ਜੇ_ਅੰਤ

BOOT_ਕੋਡ ਖਾਲੀ VISIBLE arm_errata(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_ERRATA_773022
    errata_armA15_773022();
#ਜੇ_ਅੰਤ
}

