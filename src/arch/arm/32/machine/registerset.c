/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت ثبتوونکی_t پیغام_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_پیغام_registers,
    حجم(پیغام_Registers) / حجم(پیغام_Registers[0]) == n_پیغام_Registers
);

ثابت ثبتوونکی_t چوکاټ_Registers[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_چوکاټ_registers,
    حجم(چوکاټ_Registers) / حجم(چوکاټ_Registers[0]) == n_چوکاټ_Registers
);

ثابت ثبتوونکی_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
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
