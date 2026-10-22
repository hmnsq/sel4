/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/sbi.h>

/** DONT_TRANSLATE */
ਖਾਲੀ VISIBLE NO_INLINE halt(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    ਲਿਖੋ("halting...");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    debug_print_Kernel_ਇੰਦਰਾਜ_Reason();
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
