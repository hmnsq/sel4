/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <api/debug.h>

/** DONT_TRANSLATE */
خالی VISIBLE halt(خالی)
{
    /* halt is actually, idle thread without the interrupts */
    asm متلون("cli");

#اگر_تعریف_شدہ CONFIG_PRINTING
    لکھو("halting...");
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    debug_print_Kernel_اندراج_Reason();
#اگر_ختم
#اگر_ختم
    idle_thread();
    UNREACHABLE();
}
