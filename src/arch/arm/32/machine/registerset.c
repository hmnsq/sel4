/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <arch/machine/registerset.h>

സ്ഥിരം register_t സന്ദേശം_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    വലുപ്പം(സന്ദേശം_Registers) / വലുപ്പം(സന്ദേശം_Registers[0]) == n_സന്ദേശം_Registers
);

സ്ഥിരം register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    വലുപ്പം(frameRegisters) / വലുപ്പം(frameRegisters[0]) == n_frameRegisters
);

സ്ഥിരം register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    വലുപ്പം(gpRegisters) / വലുപ്പം(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(ശൂന്യം)
{
    തിരികെ getRegister(കണ്ണി_STATE(ksCurThread), nbsendRecvDest);
}
#endif
