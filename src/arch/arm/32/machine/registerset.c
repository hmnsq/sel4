/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <arch/machine/registerset.h>

ধ্রুব register_t বার্তা_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    পরিমাণ(বার্তা_Registers) / পরিমাণ(বার্তা_Registers[0]) == n_বার্তা_Registers
);

ধ্রুব register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    পরিমাণ(frameRegisters) / পরিমাণ(frameRegisters[0]) == n_frameRegisters
);

ধ্রুব register_t gpRegisters[] = {
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
    প্রত্যাগম getRegister(গ্রন্থি_STATE(ksCurThread), nbsendRecvDest);
}
#endif
