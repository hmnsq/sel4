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

#جے_تعریف_شدہ CONFIG_PRINTING
    لکھو("halting...");
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    debug_print_Kernel_اندراج_Reason();
#جے_انت
#جے_انت
    idle_thread();
    UNREACHABLE();
}
