/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>

ثابت register_t رسالة_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    حجم(رسالة_Registers) / حجم(رسالة_Registers[0]) == n_رسالة_Registers
);

ثابت register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    حجم(frameRegisters) / حجم(frameRegisters[0]) == n_frameRegisters
);

ثابت register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(فراغ)
{
    إرجاع getRegister(عقدة_STATE(ksCurThread), nbsendRecvDest);
}
#endif
