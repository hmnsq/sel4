/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/machine.h>
#ಅನ್ತರ್ಭಾವ <api/debug.h>

/** DONT_TRANSLATE */
ಶೂನ್ಯ NORETURN NO_INLINE VISIBLE halt(ಶೂನ್ಯ)
{
    /* halt is actually, idle thread without the interrupts */
    MSR("daif", (DAIF_DEBUG | DAIF_SERROR | DAIF_IRQ | DAIF_FIRQ));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    ಬರೆ("halting...");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    debug_print_Kernel_ನಮೂದು_Reason();
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
    idle_thread();
    UNREACHABLE();
}
