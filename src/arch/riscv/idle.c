/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/sbi.h>

/** DONT_TRANSLATE */
فراغ VISIBLE NO_INLINE halt(فراغ)
{
#جے_تعریف_شدہ CONFIG_PRINTING
    لکھو("halting...");
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    debug_print_Kernel_اندراج_Reason();
#جے_انت /* CONFIG_DEBUG_BUILD */
#جے_انت /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
