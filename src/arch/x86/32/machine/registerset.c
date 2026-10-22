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

ಧ್ರುವ register_t ಸಂದೇಶ_Registers[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    ಪರಿಮಾಣ(ಸಂದೇಶ_Registers) / ಪರಿಮಾಣ(ಸಂದೇಶ_Registers[0]) == n_ಸಂದೇಶ_Registers
);

ಧ್ರುವ register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    ಪರಿಮಾಣ(frameRegisters) / ಪರಿಮಾಣ(frameRegisters[0]) == n_frameRegisters
);

ಧ್ರುವ register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    ಪರಿಮಾಣ(gpRegisters) / ಪರಿಮಾಣ(gpRegisters[0]) == n_gpRegisters
);

ಶೂನ್ಯ Mode_initContext(user_context_t *context)
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

word_t Mode_sanitiseRegister(register_t reg, word_t v)
{
    ಪ್ರತ್ಯಾಗಮ v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(ಶೂನ್ಯ)
{
    seL4_IPCBuffer *buffer = (seL4_IPCBuffer *) lookupIPCBuffer(false, ಗಂಟು_STATE(ksCurThread));
    ಯದಿ (buffer != ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ buffer->userData;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}
#endif
