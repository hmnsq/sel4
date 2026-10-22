/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <arch/machine/registerset.h>

மாறிலி register_t செய்தி_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    அளவு(செய்தி_Registers) / அளவு(செய்தி_Registers[0]) == n_செய்தி_Registers
);

மாறிலி register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    அளவு(frameRegisters) / அளவு(frameRegisters[0]) == n_frameRegisters
);

மாறிலி register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    அளவு(gpRegisters) / அளவு(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(வெறுமை)
{
    திரும்பு getRegister(முனை_STATE(ksCurThread), nbsendRecvDest);
}
#endif
