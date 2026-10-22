/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت ثبتوونکی_t پیغام_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_پیغام_registers,
    حجم(پیغام_Registers) / حجم(پیغام_Registers[0]) == n_پیغام_Registers
);

ثابت ثبتوونکی_t چوکاټ_Registers[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_چوکاټ_registers,
    حجم(چوکاټ_Registers) / حجم(چوکاټ_Registers[0]) == n_چوکاټ_Registers
);

ثابت ثبتوونکی_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

#که_تعریف_شوی CONFIG_KERNEL_MCS
کلمه_t getNBSendRecvDest(فراغ)
{
    أرجع get_ثبتوونکی(غوټه_STATE(ksCurThread), nbsendRecvDest);
}
#د_که_پای
