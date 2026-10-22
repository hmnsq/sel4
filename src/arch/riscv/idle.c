/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <arch/sbi.h>

/** DONT_TRANSLATE */
تهی VISIBLE NO_INLINE halt(تهی)
{
#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
