/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/machine.h>
#شامل <api/debug.h>

/** DONT_TRANSLATE */
تهی NORETURN NO_INLINE VISIBLE halt(تهی)
{
    /* halt is actually, idle thread without the interrupts */
    asm فرار("cpsid iaf");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
