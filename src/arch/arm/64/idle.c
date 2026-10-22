/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <mode/machine.h>
#అన్తర్భావ <api/debug.h>

/** DONT_TRANSLATE */
శూన్య NORETURN NO_INLINE VISIBLE halt(శూన్య)
{
    /* halt is actually, idle thread without the interrupts */
    MSR("daif", (DAIF_DEBUG | DAIF_SERROR | DAIF_IRQ | DAIF_FIRQ));

#నిర్వచితమైతే CONFIG_PRINTING
    రాయి("halting...");
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    debug_print_Kernel_నమోదు_Reason();
#అయితే_ముగింపు
#అయితే_ముగింపు
    idle_thread();
    UNREACHABLE();
}
