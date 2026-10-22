/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <arch/machine/registerset.h>

ধ্রুৱ register_t বাৰ্তা_Registers[] = {
    X2, X3, X4, X5
};
compile_assert(
    consistent_message_registers,
    পরিমাণ(বাৰ্তা_Registers) / পরিমাণ(বাৰ্তা_Registers[0]) == n_বাৰ্তা_Registers
);

ধ্রুৱ register_t frameRegisters[] = {
    FaultIP, SP_EL0, SPSR_EL1,
    X0, X1, X2, X3, X4, X5, X6, X7, X8, X16, X17, X18, X29, X30
};
compile_assert(
    consistent_frame_registers,
    পরিমাণ(frameRegisters) / পরিমাণ(frameRegisters[0]) == n_frameRegisters
);

ধ্রুৱ register_t gpRegisters[] = {
    X9, X10, X11, X12, X13, X14, X15,
    X19, X20, X21, X22, X23, X24, X25, X26, X27, X28,
    TPIDR_EL0, TPIDRRO_EL0,
};
compile_assert(
    consistent_gp_registers,
    পরিমাণ(gpRegisters) / পরিমাণ(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(শূন্য)
{
    ঘূৰাই getRegister(গ্ৰন্থি_STATE(ksCurThread), nbsendRecvDest);
}
#endif
