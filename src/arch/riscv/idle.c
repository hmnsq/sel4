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
#که_تعریف_شوی CONFIG_PRINTING
    چاپ_کړه("halting...");
#که_تعریف_شوی CONFIG_DEBUG_BUILD
    debug_print_Kernel_ثبت_Reason();
#د_که_پای /* CONFIG_DEBUG_BUILD */
#د_که_پای /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
