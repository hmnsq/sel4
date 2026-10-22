/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <arch/machine/registerset.h>

மாறிலி register_t செய்தி_Registers[] = {
    X2, X3, X4, X5
};
compile_assert(
    consistent_message_registers,
    அளவு(செய்தி_Registers) / அளவு(செய்தி_Registers[0]) == n_செய்தி_Registers
);

மாறிலி register_t frameRegisters[] = {
    FaultIP, SP_EL0, SPSR_EL1,
    X0, X1, X2, X3, X4, X5, X6, X7, X8, X16, X17, X18, X29, X30
};
compile_assert(
    consistent_frame_registers,
    அளவு(frameRegisters) / அளவு(frameRegisters[0]) == n_frameRegisters
);

மாறிலி register_t gpRegisters[] = {
    X9, X10, X11, X12, X13, X14, X15,
    X19, X20, X21, X22, X23, X24, X25, X26, X27, X28,
    TPIDR_EL0, TPIDRRO_EL0,
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
