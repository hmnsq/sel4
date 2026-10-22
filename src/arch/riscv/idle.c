/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <arch/sbi.h>

/** DONT_TRANSLATE */
خالی VISIBLE NO_INLINE halt(خالی)
{
#اگر_تعریف_شدہ CONFIG_PRINTING
    لکھو("halting...");
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    debug_print_Kernel_اندراج_Reason();
#اگر_ختم /* CONFIG_DEBUG_BUILD */
#اگر_ختم /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
