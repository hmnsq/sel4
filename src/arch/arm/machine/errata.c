/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <api/types.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/machine/hardware.h>
#अन्तर्भाव <util.h>

/* Prototyped here as this is referenced from assembly */
शून्य arm_errata(शून्य);

#ifdef CONFIG_ARM_ERRATA_773022
/*
 * There is an errata for Cortex-A15 up to r0p4 where the loop buffer
 * may deliver incorrect instructions. The work around is to disable
 * the loop buffer. Errata is number 773022.
 */
BOOT_CODE स्थिर शून्य errata_armA15_773022(शून्य)
{
    /* Fetch the processor primary part number. */
    uint32_t proc_id = getProcessorID();
    uint32_t variant = (proc_id >> 20) & MASK(4);
    uint32_t revision = proc_id & MASK(4);
    uint32_t part = (proc_id >> 4) & MASK(12);

    /* Check that we are running A15 and a revision upto r0p4. */
    जर (part == 0xc0f && variant == 0 && revision <= 4) {
        /* Disable loop buffer in the auxiliary control register */
        writeAuxiliaryControlRegister(
            readAuxiliaryControlRegister() | BIT(1));
    }
}
#endif

BOOT_CODE शून्य VISIBLE arm_errata(शून्य)
{
#ifdef CONFIG_ARM_ERRATA_773022
    errata_armA15_773022();
#endif
}

