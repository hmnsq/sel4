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

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
    لکو("halting...");
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
    debug_print_Kernel_داخلا_Reason();
#جيڪڏهن_پورو
#جيڪڏهن_پورو
    idle_thread();
    UNREACHABLE();
}
