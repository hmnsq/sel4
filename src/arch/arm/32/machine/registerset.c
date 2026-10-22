/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <assert.h>
#include <arch/machine/registerset.h>

const register_t मेनसाहे_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_message_registers,
    sizeof(मेनसाहे_Registers) / sizeof(मेनसाहे_Registers[0]) == n_मेनसाहे_Registers
);

const register_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    sizeof(frameRegisters) / sizeof(frameRegisters[0]) == n_frameRegisters
);

const register_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    sizeof(gpRegisters) / sizeof(gpRegisters[0]) == n_gpRegisters
);

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(void)
{
    return getRegister(बुहोल_STATE(ksCurThread), nbsendRecvDest);
}
#endif
