/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <api/types.h>
#समावेश <arch/machine.h>
#समावेश <arch/machine/hardware.h>
#समावेश <util.h>

/* Prototyped here as this is referenced from assembly */
व्योम arm_errata(व्योम);

#अगर_घोषित CONFIG_ARM_ERRATA_773022
/*
 * There is an errata for Cortex-A15 up to r0p4 where the loop buffer
 * may deliver incorrect instructions. The work around is to disable
 * the loop buffer. Errata is number 773022.
 */
BOOT_CODE अटल व्योम errata_armA15_773022(व्योम)
{
    /* Fetch the processor primary part number. */
    uint32_t proc_id = getProcessorID();
    uint32_t variant = (proc_id >> 20) & MASK(4);
    uint32_t revision = proc_id & MASK(4);
    uint32_t part = (proc_id >> 4) & MASK(12);

    /* Check that we are running A15 and a revision upto r0p4. */
    अगर (part == 0xc0f && variant == 0 && revision <= 4) {
        /* Disable loop buffer in the auxiliary control register */
        writeAuxiliaryControlRegister(
            readAuxiliaryControlRegister() | BIT(1));
    }
}
#पूर्ण_अगर

BOOT_CODE व्योम VISIBLE arm_errata(व्योम)
{
#अगर_घोषित CONFIG_ARM_ERRATA_773022
    errata_armA15_773022();
#पूर्ण_अगर
}

