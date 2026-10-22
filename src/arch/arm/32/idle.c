/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <mode/machine.h>
#അന്തര്ഭാവ <api/debug.h>

/** DONT_TRANSLATE */
ശൂന്യം NORETURN NO_INLINE VISIBLE halt(ശൂന്യം)
{
    /* halt is actually, idle thread without the interrupts */
    asm അസ്ഥിര("cpsid iaf");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
