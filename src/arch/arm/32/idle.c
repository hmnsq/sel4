/*
 * Copyright 2014, General Dynamics C4 Systems
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
    asm ಅಸ್ಥಿರ("cpsid iaf");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    ಬರೆ("halting...");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    debug_print_Kernel_ನಮೂದು_Reason();
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
    idle_thread();
    UNREACHABLE();
}
