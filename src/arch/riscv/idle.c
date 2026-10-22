/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <arch/sbi.h>

/** DONT_TRANSLATE */
ଶୂନ୍ୟ VISIBLE NO_INLINE halt(ଶୂନ୍ୟ)
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
