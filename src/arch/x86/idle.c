/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <api/debug.h>

/** DONT_TRANSLATE */
ಶೂನ್ಯ VISIBLE halt(ಶೂನ್ಯ)
{
    /* halt is actually, idle thread without the interrupts */
    asm ಚಞ್ಚಲ("cli");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
