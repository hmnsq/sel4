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
#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
    لکو("halting...");
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
    debug_print_Kernel_داخلا_Reason();
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
#جيڪڏهن_پورو /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
