/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت رجسٹر_t پیغام_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_سنیہا_registers,
    حجم(پیغام_Registers) / حجم(پیغام_Registers[0]) == n_پیغام_Registers
);

ثابت رجسٹر_t چوکھٹا_Registers[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_چوکھٹا_registers,
    حجم(چوکھٹا_Registers) / حجم(چوکھٹا_Registers[0]) == n_چوکھٹا_Registers
);

ثابت رجسٹر_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
لفظ_t getNBSendRecvDest(فراغ)
{
    أرجع get_رجسٹر(گرہ_STATE(ksCurThread), nbsendRecvDest);
}
#جے_انت
