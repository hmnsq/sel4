/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <config.h>
#अन्तर्भाव <sel4/arch/constants.h>
#अन्तर्भाव <arch/machine/registerset.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/object/structures.h>
#अन्तर्भाव <machine/debug.h>

ध्रुव पञ्जिकाकोशः_t सन्देशः_Registers[] = {
    EDI,
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    EBP
#यदि_अन्तः
};
compile_assert(
    consistent_सन्देशः_registers,
    परिमाण(सन्देशः_Registers) / परिमाण(सन्देशः_Registers[0]) == n_सन्देशः_Registers
);

ध्रुव पञ्जिकाकोशः_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    परिमाण(frameRegisters) / परिमाण(frameRegisters[0]) == n_frameRegisters
);

ध्रुव पञ्जिकाकोशः_t gpRegisters[] = {
    FS_आधारः, GS_आधारः
};
compile_assert(
    consistent_gp_registers,
    परिमाण(gpRegisters) / परिमाण(gpRegisters[0]) == n_gpRegisters
);

शून्य बहुलकम्_init_Context(user_context_t *context)
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

word_t बहुलकम्_sanitise_पञ्जिकाकोशः(पञ्जिकाकोशः_t reg, word_t v)
{
    प्रत्यागम v;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(शून्य)
{
    seL4_IPCBuffer *बफर = (seL4_IPCBuffer *) lookupIPCBuffer(false, ग्रन्थिः_STATE(ksCurThread));
    यदि (बफर != रिक्त) {
        प्रत्यागम बफर->user_दत्त;
    } अन्यथा {
        प्रत्यागम 0;
    }
}
#यदि_अन्तः
