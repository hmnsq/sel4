/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>
#समावेश <arch/machine/registerset.h>

स्थिर register_t संदेश_Registers[] = {
    X2, X3, X4, X5
};
compile_assert(
    consistent_message_registers,
    माप(संदेश_Registers) / माप(संदेश_Registers[0]) == n_संदेश_Registers
);

स्थिर register_t frameRegisters[] = {
    FaultIP, SP_EL0, SPSR_EL1,
    X0, X1, X2, X3, X4, X5, X6, X7, X8, X16, X17, X18, X29, X30
};
compile_assert(
    consistent_frame_registers,
    माप(frameRegisters) / माप(frameRegisters[0]) == n_frameRegisters
);

स्थिर register_t gpRegisters[] = {
    X9, X10, X11, X12, X13, X14, X15,
    X19, X20, X21, X22, X23, X24, X25, X26, X27, X28,
    TPIDR_EL0, TPIDRRO_EL0,
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
