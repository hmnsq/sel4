/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/debug.h>

/** DONT_TRANSLATE */
ਖਾਲੀ VISIBLE halt(ਖਾਲੀ)
{
    /* halt is actually, idle thread without the interrupts */
    asm ਅਸਥਿਰ("cli");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    ਲਿਖੋ("halting...");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    debug_print_Kernel_ਇੰਦਰਾਜ_Reason();
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
    idle_thread();
    UNREACHABLE();
}
