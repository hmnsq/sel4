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
#اگر_تعریف_شده CONFIG_PRINTING
    چاپ_کن("halting...");
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
    debug_print_Kernel_مدخل_Reason();
#پایان_اگر /* CONFIG_DEBUG_BUILD */
#پایان_اگر /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
