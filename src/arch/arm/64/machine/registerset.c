/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>

ਅਚਲ ਰਜਿਸਟਰ_t ਸੁਨੇਹਾ_Registers[] = {
    X2, X3, X4, X5
};
compile_assert(
    consistent_ਸੁਨੇਹਾ_registers,
    ਆਕਾਰ(ਸੁਨੇਹਾ_Registers) / ਆਕਾਰ(ਸੁਨੇਹਾ_Registers[0]) == n_ਸੁਨੇਹਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t ਚੌਖਟਾ_Registers[] = {
    FaultIP, SP_EL0, SPSR_EL1,
    X0, X1, X2, X3, X4, X5, X6, X7, X8, X16, X17, X18, X29, X30
};
compile_assert(
    consistent_ਚੌਖਟਾ_registers,
    ਆਕਾਰ(ਚੌਖਟਾ_Registers) / ਆਕਾਰ(ਚੌਖਟਾ_Registers[0]) == n_ਚੌਖਟਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t gpRegisters[] = {
    X9, X10, X11, X12, X13, X14, X15,
    X19, X20, X21, X22, X23, X24, X25, X26, X27, X28,
    TPIDR_EL0, TPIDRRO_EL0,
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
