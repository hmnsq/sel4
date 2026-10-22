/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <arch/machine/registerset.h>

ধ্রুব register_t বার্তা_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_message_registers,
    পরিমাণ(বার্তা_Registers) / পরিমাণ(বার্তা_Registers[0]) == n_বার্তা_Registers
);

ধ্রুব register_t frameRegisters[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_frame_registers,
    পরিমাণ(frameRegisters) / পরিমাণ(frameRegisters[0]) == n_frameRegisters
);

ধ্রুব register_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
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
