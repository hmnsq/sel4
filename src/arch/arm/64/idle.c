/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/machine.h>
#شامل <api/debug.h>

/** DONT_TRANSLATE */
خالی NORETURN NO_INLINE VISIBLE halt(خالی)
{
    /* halt is actually, idle thread without the interrupts */
    MSR("daif", (DAIF_DEBUG | DAIF_SERROR | DAIF_IRQ | DAIF_FIRQ));

#اگر_تعریف_شدہ CONFIG_PRINTING
    لکھو("halting...");
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    debug_print_Kernel_اندراج_Reason();
#اگر_ختم
#اگر_ختم
    idle_thread();
    UNREACHABLE();
}
