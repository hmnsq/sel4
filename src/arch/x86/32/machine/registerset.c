/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>
#समावेश <config.h>
#समावेश <sel4/arch/constants.h>
#समावेश <arch/machine/registerset.h>
#समावेश <machine/fpu.h>
#समावेश <arch/object/structures.h>
#समावेश <machine/debug.h>

स्थिर register_t संदेश_Registers[] = {
    EDI,
#अगर_अघोषित CONFIG_KERNEL_MCS
    EBP
#पूर्ण_अगर
};
compile_assert(
    consistent_message_registers,
    माप(संदेश_Registers) / माप(संदेश_Registers[0]) == n_संदेश_Registers
);

स्थिर register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    माप(frameRegisters) / माप(frameRegisters[0]) == n_frameRegisters
);

स्थिर register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    माप(gpRegisters) / माप(gpRegisters[0]) == n_gpRegisters
);

व्योम Mode_initContext(user_context_t *context)
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
    वापस v;
}

#अगर_घोषित CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(व्योम)
{
    seL4_IPCBuffer *बफर = (seL4_IPCBuffer *) lookupIPCBuffer(false, गाँठ_STATE(ksCurThread));
    अगर (बफर != शून्य) {
        वापस बफर->user_तथ्य;
    } अन्यथा {
        वापस 0;
    }
}
#पूर्ण_अगर
