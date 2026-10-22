/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت ثبات_t پیام_Registers[] = {
    X2, X3, X4, X5
};
compile_assert(
    consistent_پیام_registers,
    حجم(پیام_Registers) / حجم(پیام_Registers[0]) == n_پیام_Registers
);

ثابت ثبات_t قاب_Registers[] = {
    FaultIP, SP_EL0, SPSR_EL1,
    X0, X1, X2, X3, X4, X5, X6, X7, X8, X16, X17, X18, X29, X30
};
compile_assert(
    consistent_قاب_registers,
    حجم(قاب_Registers) / حجم(قاب_Registers[0]) == n_قاب_Registers
);

ثابت ثبات_t gpRegisters[] = {
    X9, X10, X11, X12, X13, X14, X15,
    X19, X20, X21, X22, X23, X24, X25, X26, X27, X28,
    TPIDR_EL0, TPIDRRO_EL0,
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
