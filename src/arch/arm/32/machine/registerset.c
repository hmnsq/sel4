/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت ثبات_t پیام_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_پیام_registers,
    حجم(پیام_Registers) / حجم(پیام_Registers[0]) == n_پیام_Registers
);

ثابت ثبات_t قاب_Registers[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_قاب_registers,
    حجم(قاب_Registers) / حجم(قاب_Registers[0]) == n_قاب_Registers
);

ثابت ثبات_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
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
