/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت رجسٽر_t نياپو_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_پيغام_registers,
    حجم(نياپو_Registers) / حجم(نياپو_Registers[0]) == n_نياپو_Registers
);

ثابت رجسٽر_t چوکٺو_Registers[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_چوکٺو_registers,
    حجم(چوکٺو_Registers) / حجم(چوکٺو_Registers[0]) == n_چوکٺو_Registers
);

ثابت رجسٽر_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
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
