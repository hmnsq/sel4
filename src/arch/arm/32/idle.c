/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <mode/machine.h>
#অন্তর্ভাব <api/debug.h>

/** DONT_TRANSLATE */
শূন্য NORETURN NO_INLINE VISIBLE halt(শূন্য)
{
    /* halt is actually, idle thread without the interrupts */
    asm অস্থির("cpsid iaf");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
