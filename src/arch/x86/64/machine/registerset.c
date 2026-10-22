/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/object/structures.h>

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಸಂದೇಶ_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_ಸಂದೇಶ_registers,
    ಗಾತ್ರ(ಸಂದೇಶ_Registers) / ಗಾತ್ರ(ಸಂದೇಶ_Registers[0]) == n_ಸಂದೇಶ_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಚೌಕಟ್ಟು_Registers[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
};
compile_assert(
    consistent_ಚೌಕಟ್ಟು_registers,
    ಗಾತ್ರ(ಚೌಕಟ್ಟು_Registers) / ಗಾತ್ರ(ಚೌಕಟ್ಟು_Registers[0]) == n_ಚೌಕಟ್ಟು_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t gpRegisters[] = {
    FS_ತಳ, GS_ತಳ
};
compile_assert(
    consistent_gp_registers,
    ಗಾತ್ರ(gpRegisters) / ಗಾತ್ರ(gpRegisters[0]) == n_gpRegisters
);

ಶೂನ್ಯ ಬಹುಲಕ_init_Context(user_context_t *context)
{
    context->registers[RAX] = 0;
    context->registers[RBX] = 0;
    context->registers[RCX] = 0;
    context->registers[RDX] = 0;
    context->registers[RSI] = 0;
    context->registers[RDI] = 0;
    context->registers[RBP] = 0;
    context->registers[R8]  = 0;
    context->registers[R9]  = 0;
    context->registers[R10] = 0;
    context->registers[R11] = 0;
    context->registers[R12] = 0;
    context->registers[R13] = 0;
    context->registers[R14] = 0;
    context->registers[R15] = 0;
    context->registers[RSP] = 0;
}

ಶಬ್ದ_t ಬಹುಲಕ_sanitise_ರಿಜಿಸ್ಟರ್(ರಿಜಿಸ್ಟರ್_t reg, ಶಬ್ದ_t v)
{
    ಆದರೆ (reg == FaultIP || reg == ಮುಂದಿನ_IP || reg == FS_ತಳ || reg == GS_ತಳ) {
        /* ensure instruction address is canonical */
        ಆದರೆ (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    ಹಿಂತಿರುಗಿ v;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶಬ್ದ_t getNBSendRecvDest(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), nbsendRecvDest);
}
#ಆದರೆ_ಅಂತ್ಯ
