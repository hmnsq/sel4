/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <arch/machine/registerset.h>

स्थिरांक register_t संदेश_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    आकार(संदेश_Registers) / आकार(संदेश_Registers[0]) == n_संदेश_Registers
);

स्थिरांक register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    आकार(frameRegisters) / आकार(frameRegisters[0]) == n_frameRegisters
);

स्थिरांक register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    आकार(gpRegisters) / आकार(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(शून्य)
{
    परत getRegister(गाठ_STATE(ksCurThread), nbsendRecvDest);
}
#endif
