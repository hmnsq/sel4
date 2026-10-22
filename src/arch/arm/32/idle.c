/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/machine.h>
#ಅನ್ತರ್ಭಾವ <api/debug.h>

/** DONT_TRANSLATE */
ಶೂನ್ಯ NORETURN NO_INLINE VISIBLE halt(ಶೂನ್ಯ)
{
    /* halt is actually, idle thread without the interrupts */
    asm ಚಞ್ಚಲ("cpsid iaf");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
