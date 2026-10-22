/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <arch/machine/registerset.h>

ধ্রুৱ register_t বাৰ্তা_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    পরিমাণ(বাৰ্তা_Registers) / পরিমাণ(বাৰ্তা_Registers[0]) == n_বাৰ্তা_Registers
);

ধ্রুৱ register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    পরিমাণ(frameRegisters) / পরিমাণ(frameRegisters[0]) == n_frameRegisters
);

ধ্রুৱ register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
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
