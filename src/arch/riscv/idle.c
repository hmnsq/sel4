/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <arch/sbi.h>

/** DONT_TRANSLATE */
శూన్య VISIBLE NO_INLINE halt(శూన్య)
{
#నిర్వచితమైతే CONFIG_PRINTING
    రాయి("halting...");
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    debug_print_Kernel_నమోదు_Reason();
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
#అయితే_ముగింపు /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
