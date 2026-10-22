/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت ثبات_t پیام_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_پیام_registers,
    حجم(پیام_Registers) / حجم(پیام_Registers[0]) == n_پیام_Registers
);

ثابت ثبات_t قاب_Registers[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_قاب_registers,
    حجم(قاب_Registers) / حجم(قاب_Registers[0]) == n_قاب_Registers
);

ثابت ثبات_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
کلمه_t getNBSendRecvDest(فراغ)
{
    أرجع get_ثبات(گره_STATE(ksCurThread), nbsendRecvDest);
}
#پایان_اگر
