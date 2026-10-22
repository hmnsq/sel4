/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <config.h>
#অন্তর্ভাব <sel4/arch/constants.h>
#অন্তর্ভাব <arch/machine/registerset.h>
#অন্তর্ভাব <machine/fpu.h>
#অন্তর্ভাব <arch/object/structures.h>
#অন্তর্ভাব <machine/debug.h>

ধ্রুবক register_t বার্তা_Registers[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    আকার(বার্তা_Registers) / আকার(বার্তা_Registers[0]) == n_বার্তা_Registers
);

ধ্রুবক register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    আকার(frameRegisters) / আকার(frameRegisters[0]) == n_frameRegisters
);

ধ্রুবক register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    আকার(gpRegisters) / আকার(gpRegisters[0]) == n_gpRegisters
);

শূন্য Mode_initContext(user_context_t *context)
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
    ফেরত v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(শূন্য)
{
    seL4_IPCBuffer *বাফার = (seL4_IPCBuffer *) lookupIPCBuffer(false, গ্রন্থি_STATE(ksCurThread));
    যদি (বাফার != ফাঁকা) {
        ফেরত বাফার->user_তথ্য;
    } অন্যথা {
        ফেরত 0;
    }
}
#endif
