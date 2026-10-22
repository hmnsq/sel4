/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/registerset.h>

ଅଚଳ register_t ସନ୍ଦେଶ_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    ଆକାର(ସନ୍ଦେଶ_Registers) / ଆକାର(ସନ୍ଦେଶ_Registers[0]) == n_ସନ୍ଦେଶ_Registers
);

ଅଚଳ register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    ଆକାର(frameRegisters) / ଆକାର(frameRegisters[0]) == n_frameRegisters
);

ଅଚଳ register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    ଆକାର(gpRegisters) / ଆକାର(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ getRegister(ଗଣ୍ଠି_STATE(ksCurThread), nbsendRecvDest);
}
#endif
