/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <machine/registerset.h>

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t fault_messages[][MAX_ಸಂದೇಶ_ಗಾತ್ರ] = {
    [ಸಂದೇಶ_ID_Syscall] = SYSCALL_ಸಂದೇಶ,
    [ಸಂದೇಶ_ID_Exception] = EXCEPTION_ಸಂದೇಶ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    [ಸಂದೇಶ_ID_Timeout_ಉತ್ತರ] = TIMEOUT_ಉತ್ತರ_ಸಂದೇಶ,
#ಆದರೆ_ಅಂತ್ಯ
};
