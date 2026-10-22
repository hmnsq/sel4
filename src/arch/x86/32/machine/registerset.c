/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <sel4/arch/constants.h>
#അന്തര്ഭാവ <arch/machine/registerset.h>
#അന്തര്ഭാവ <machine/fpu.h>
#അന്തര്ഭാവ <arch/object/structures.h>
#അന്തര്ഭാവ <machine/debug.h>

സ്ഥിരം register_t സന്ദേശം_Registers[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    വലുപ്പം(സന്ദേശം_Registers) / വലുപ്പം(സന്ദേശം_Registers[0]) == n_സന്ദേശം_Registers
);

സ്ഥിരം register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    വലുപ്പം(frameRegisters) / വലുപ്പം(frameRegisters[0]) == n_frameRegisters
);

സ്ഥിരം register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    വലുപ്പം(gpRegisters) / വലുപ്പം(gpRegisters[0]) == n_gpRegisters
);

ശൂന്യം Mode_initContext(user_context_t *context)
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
    തിരികെ v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(ശൂന്യം)
{
    seL4_IPCBuffer *buffer = (seL4_IPCBuffer *) lookupIPCBuffer(false, കണ്ണി_STATE(ksCurThread));
    എങ്കിൽ (buffer != ശൂന്യ) {
        തിരികെ buffer->userData;
    } അല്ലെങ്കിൽ {
        തിരികെ 0;
    }
}
#endif
