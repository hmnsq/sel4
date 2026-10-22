/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت رجسٽر_t نياپو_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_پيغام_registers,
    حجم(نياپو_Registers) / حجم(نياپو_Registers[0]) == n_نياپو_Registers
);

ثابت رجسٽر_t چوکٺو_Registers[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_چوکٺو_registers,
    حجم(چوکٺو_Registers) / حجم(چوکٺو_Registers[0]) == n_چوکٺو_Registers
);

ثابت رجسٽر_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
لفظ_t getNBSendRecvDest(فراغ)
{
    أرجع get_رجسٽر(ڳنڍ_STATE(ksCurThread), nbsendRecvDest);
}
#جيڪڏهن_پورو
