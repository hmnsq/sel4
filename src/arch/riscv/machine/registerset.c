/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/registerset.h>

ଅଚଳ register_t ସନ୍ଦେଶ_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_message_registers,
    ଆକାର(ସନ୍ଦେଶ_Registers) / ଆକାର(ସନ୍ଦେଶ_Registers[0]) == n_ସନ୍ଦେଶ_Registers
);

ଅଚଳ register_t frameRegisters[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_frame_registers,
    ଆକାର(frameRegisters) / ଆକାର(frameRegisters[0]) == n_frameRegisters
);

ଅଚଳ register_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
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
