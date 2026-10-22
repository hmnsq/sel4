/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <arch/machine/registerset.h>

અચલ register_t સંદેશ_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_message_registers,
    કદ(સંદેશ_Registers) / કદ(સંદેશ_Registers[0]) == n_સંદેશ_Registers
);

અચલ register_t frameRegisters[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_frame_registers,
    કદ(frameRegisters) / કદ(frameRegisters[0]) == n_frameRegisters
);

અચલ register_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
};
compile_assert(
    consistent_gp_registers,
    કદ(gpRegisters) / કદ(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(શૂન્ય)
{
    પાછા getRegister(ગાંઠ_STATE(ksCurThread), nbsendRecvDest);
}
#endif
