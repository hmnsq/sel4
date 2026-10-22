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

#اگر_تعریف_شده CONFIG_PRINTING
    چاپ_کن("halting...");
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
    debug_print_Kernel_مدخل_Reason();
#پایان_اگر
#پایان_اگر
    idle_thread();
    UNREACHABLE();
}
