/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಸಂದೇಶ_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_ಸಂದೇಶ_registers,
    ಗಾತ್ರ(ಸಂದೇಶ_Registers) / ಗಾತ್ರ(ಸಂದೇಶ_Registers[0]) == n_ಸಂದೇಶ_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಚೌಕಟ್ಟು_Registers[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_ಚೌಕಟ್ಟು_registers,
    ಗಾತ್ರ(ಚೌಕಟ್ಟು_Registers) / ಗಾತ್ರ(ಚೌಕಟ್ಟು_Registers[0]) == n_ಚೌಕಟ್ಟು_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
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
