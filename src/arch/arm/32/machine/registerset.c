/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>

ਅਚਲ ਰਜਿਸਟਰ_t ਸੁਨੇਹਾ_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_ਸੁਨੇਹਾ_registers,
    ਆਕਾਰ(ਸੁਨੇਹਾ_Registers) / ਆਕਾਰ(ਸੁਨੇਹਾ_Registers[0]) == n_ਸੁਨੇਹਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t ਚੌਖਟਾ_Registers[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_ਚੌਖਟਾ_registers,
    ਆਕਾਰ(ਚੌਖਟਾ_Registers) / ਆਕਾਰ(ਚੌਖਟਾ_Registers[0]) == n_ਚੌਖਟਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    ਆਕਾਰ(gpRegisters) / ਆਕਾਰ(gpRegisters[0]) == n_gpRegisters
);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸ਼ਬਦ_t getNBSendRecvDest(ਖਾਲੀ)
{
    ਵਾਪਸ get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), nbsendRecvDest);
}
#ਜੇ_ਅੰਤ
