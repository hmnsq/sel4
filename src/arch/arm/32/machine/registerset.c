/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>
#समावेश <arch/machine/registerset.h>

स्थिर register_t संदेश_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    माप(संदेश_Registers) / माप(संदेश_Registers[0]) == n_संदेश_Registers
);

स्थिर register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    माप(frameRegisters) / माप(frameRegisters[0]) == n_frameRegisters
);

स्थिर register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    माप(gpRegisters) / माप(gpRegisters[0]) == n_gpRegisters
);

#अगर_घोषित CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(व्योम)
{
    वापस getRegister(गाँठ_STATE(ksCurThread), nbsendRecvDest);
}
#पूर्ण_अगर
