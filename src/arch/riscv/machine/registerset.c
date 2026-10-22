/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <arch/machine/registerset.h>

अचर register_t सन्देश_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_message_registers,
    परिमाण(सन्देश_Registers) / परिमाण(सन्देश_Registers[0]) == n_सन्देश_Registers
);

अचर register_t frameRegisters[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_frame_registers,
    परिमाण(frameRegisters) / परिमाण(frameRegisters[0]) == n_frameRegisters
);

अचर register_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
};
compile_assert(
    consistent_gp_registers,
    परिमाण(gpRegisters) / परिमाण(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(शून्य)
{
    फर्काऊ getRegister(गाँठो_STATE(ksCurThread), nbsendRecvDest);
}
#endif
