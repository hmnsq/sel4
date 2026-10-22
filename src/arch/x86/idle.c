/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <api/debug.h>

/** DONT_TRANSLATE */
فراغ VISIBLE halt(فراغ)
{
    /* halt is actually, idle thread without the interrupts */
    asm متطاير("cli");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_اطبع_Kernel_Entry_Reason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
