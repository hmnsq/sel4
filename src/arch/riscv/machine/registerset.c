/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <arch/machine/registerset.h>

மாறிலி register_t செய்தி_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_message_registers,
    அளவு(செய்தி_Registers) / அளவு(செய்தி_Registers[0]) == n_செய்தி_Registers
);

மாறிலி register_t frameRegisters[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_frame_registers,
    அளவு(frameRegisters) / அளவு(frameRegisters[0]) == n_frameRegisters
);

மாறிலி register_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
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
