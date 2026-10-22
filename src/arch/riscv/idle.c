/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <arch/sbi.h>

/** DONT_TRANSLATE */
ಶೂನ್ಯ VISIBLE NO_INLINE halt(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    ಬರೆ("halting...");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    debug_print_Kernel_ನಮೂದು_Reason();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
