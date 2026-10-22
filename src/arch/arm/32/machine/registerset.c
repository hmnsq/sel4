/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <arch/machine/registerset.h>

અચલ register_t સંદેશ_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    કદ(સંદેશ_Registers) / કદ(સંદેશ_Registers[0]) == n_સંદેશ_Registers
);

અચલ register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    કદ(frameRegisters) / કદ(frameRegisters[0]) == n_frameRegisters
);

અચલ register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    કદ(gpRegisters) / કદ(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(શૂન્ય)
{
    પાછા getRegister(ગાંઠ_STATE(ksCurThread), nbsendRecvDest);
}
#endif
