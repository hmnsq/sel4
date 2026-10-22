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

#که_تعریف_شوی CONFIG_PRINTING
    چاپ_کړه("halting...");
#که_تعریف_شوی CONFIG_DEBUG_BUILD
    debug_print_Kernel_ثبت_Reason();
#د_که_پای
#د_که_پای
    idle_thread();
    UNREACHABLE();
}
