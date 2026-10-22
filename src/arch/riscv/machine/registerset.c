/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>

ਅਚਲ ਰਜਿਸਟਰ_t ਸੁਨੇਹਾ_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_ਸੁਨੇਹਾ_registers,
    ਆਕਾਰ(ਸੁਨੇਹਾ_Registers) / ਆਕਾਰ(ਸੁਨੇਹਾ_Registers[0]) == n_ਸੁਨੇਹਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t ਚੌਖਟਾ_Registers[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_ਚੌਖਟਾ_registers,
    ਆਕਾਰ(ਚੌਖਟਾ_Registers) / ਆਕਾਰ(ਚੌਖਟਾ_Registers[0]) == n_ਚੌਖਟਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
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
