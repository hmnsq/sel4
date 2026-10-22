/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <sel4/arch/constants.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/structures.h>
#ଅନ୍ତର୍ଭାଵ <machine/debug.h>

ଅଚଳ register_t ସନ୍ଦେଶ_Registers[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    ଆକାର(ସନ୍ଦେଶ_Registers) / ଆକାର(ସନ୍ଦେଶ_Registers[0]) == n_ସନ୍ଦେଶ_Registers
);

ଅଚଳ register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    ଆକାର(frameRegisters) / ଆକାର(frameRegisters[0]) == n_frameRegisters
);

ଅଚଳ register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    ଆକାର(gpRegisters) / ଆକାର(gpRegisters[0]) == n_gpRegisters
);

ଶୂନ୍ୟ Mode_initContext(user_context_t *context)
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
    ଫେରନ୍ତୁ v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(ଶୂନ୍ୟ)
{
    seL4_IPCBuffer *buffer = (seL4_IPCBuffer *) lookupIPCBuffer(false, ଗଣ୍ଠି_STATE(ksCurThread));
    ଯଦି (buffer != ଖାଲି) {
        ଫେରନ୍ତୁ buffer->userData;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ 0;
    }
}
#endif
