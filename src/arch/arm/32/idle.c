/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <mode/machine.h>
#અન્તર્ભાવ <api/debug.h>

/** DONT_TRANSLATE */
શૂન્ય NORETURN NO_INLINE VISIBLE halt(શૂન્ય)
{
    /* halt is actually, idle thread without the interrupts */
    asm અસ્થિર("cpsid iaf");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
