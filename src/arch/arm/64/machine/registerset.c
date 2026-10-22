/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಸಂದೇಶ_Registers[] = {
    X2, X3, X4, X5
};
compile_assert(
    consistent_ಸಂದೇಶ_registers,
    ಗಾತ್ರ(ಸಂದೇಶ_Registers) / ಗಾತ್ರ(ಸಂದೇಶ_Registers[0]) == n_ಸಂದೇಶ_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಚೌಕಟ್ಟು_Registers[] = {
    FaultIP, SP_EL0, SPSR_EL1,
    X0, X1, X2, X3, X4, X5, X6, X7, X8, X16, X17, X18, X29, X30
};
compile_assert(
    consistent_ಚೌಕಟ್ಟು_registers,
    ಗಾತ್ರ(ಚೌಕಟ್ಟು_Registers) / ಗಾತ್ರ(ಚೌಕಟ್ಟು_Registers[0]) == n_ಚೌಕಟ್ಟು_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t gpRegisters[] = {
    X9, X10, X11, X12, X13, X14, X15,
    X19, X20, X21, X22, X23, X24, X25, X26, X27, X28,
    TPIDR_EL0, TPIDRRO_EL0,
};
compile_assert(
    consistent_gp_registers,
    ಗಾತ್ರ(gpRegisters) / ಗಾತ್ರ(gpRegisters[0]) == n_gpRegisters
);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶಬ್ದ_t getNBSendRecvDest(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), nbsendRecvDest);
}
#ಆದರೆ_ಅಂತ್ಯ
