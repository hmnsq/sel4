/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <mode/machine.h>
#ଅନ୍ତର୍ଭାଵ <api/debug.h>

/** DONT_TRANSLATE */
ଶୂନ୍ୟ NORETURN NO_INLINE VISIBLE halt(ଶୂନ୍ୟ)
{
    /* halt is actually, idle thread without the interrupts */
    asm ଅସ୍ଥିର("cpsid iaf");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
