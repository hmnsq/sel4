/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಸಂದೇಶ_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_ಸಂದೇಶ_registers,
    ಗಾತ್ರ(ಸಂದೇಶ_Registers) / ಗಾತ್ರ(ಸಂದೇಶ_Registers[0]) == n_ಸಂದೇಶ_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಚೌಕಟ್ಟು_Registers[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_ಚೌಕಟ್ಟು_registers,
    ಗಾತ್ರ(ಚೌಕಟ್ಟು_Registers) / ಗಾತ್ರ(ಚೌಕಟ್ಟು_Registers[0]) == n_ಚೌಕಟ್ಟು_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
};
compile_assert(
    consistent_gp_registers,
    ಗಾತ್ರ(gpRegisters) / ಗಾತ್ರ(gpRegisters[0]) == n_gpRegisters
);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶಬ್ದ_t getNBSendRecvDest(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), nbsendRecvDest);
}
#ಆದರೆ_ಅಂತ್ಯ
