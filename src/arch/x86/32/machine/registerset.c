/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <sel4/arch/constants.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/object/structures.h>
#ಅನ್ತರ್ಭಾವ <machine/debug.h>

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಸಂದೇಶ_Registers[] = {
    EDI,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    EBP
#ಆದರೆ_ಅಂತ್ಯ
};
compile_assert(
    consistent_ಸಂದೇಶ_registers,
    ಗಾತ್ರ(ಸಂದೇಶ_Registers) / ಗಾತ್ರ(ಸಂದೇಶ_Registers[0]) == n_ಸಂದೇಶ_Registers
);

ಸ್ಥಿರಾಂಕ ರಿಜಿಸ್ಟರ್_t ಚೌಕಟ್ಟು_Registers[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
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
    context->registers[EAX] = 0;
    context->registers[EBX] = 0;
    context->registers[ECX] = 0;
    context->registers[EDX] = 0;
    context->registers[ESI] = 0;
    context->registers[EDI] = 0;
    context->registers[EBP] = 0;
    context->registers[ESP] = 0;
}

ಶಬ್ದ_t ಬಹುಲಕ_sanitise_ರಿಜಿಸ್ಟರ್(ರಿಜಿಸ್ಟರ್_t reg, ಶಬ್ದ_t v)
{
    ಹಿಂತಿರುಗಿ v;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶಬ್ದ_t getNBSendRecvDest(ಶೂನ್ಯ)
{
    seL4_IPCBuffer *ಬಫರ್ = (seL4_IPCBuffer *) lookupIPCBuffer(false, ಗಂಟು_STATE(ksCurThread));
    ಆದರೆ (ಬಫರ್ != ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ ಬಫರ್->user_ದತ್ತಾಂಶ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}
#ಆದರೆ_ಅಂತ್ಯ
