/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/machine.h>
#ضمن <api/debug.h>

/** DONT_TRANSLATE */
فراغ NORETURN NO_INLINE VISIBLE halt(فراغ)
{
    /* halt is actually, idle thread without the interrupts */
    asm متطاير("cpsid iaf");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_اطبع_Kernel_Entry_Reason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
