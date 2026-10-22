/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>

ಧ್ರುವ register_t ಸಂದೇಶ_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    ಪರಿಮಾಣ(ಸಂದೇಶ_Registers) / ಪರಿಮಾಣ(ಸಂದೇಶ_Registers[0]) == n_ಸಂದೇಶ_Registers
);

ಧ್ರುವ register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    ಪರಿಮಾಣ(frameRegisters) / ಪರಿಮಾಣ(frameRegisters[0]) == n_frameRegisters
);

ಧ್ರುವ register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    ಪರಿಮಾಣ(gpRegisters) / ಪರಿಮಾಣ(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ getRegister(ಗಂಟು_STATE(ksCurThread), nbsendRecvDest);
}
#endif
