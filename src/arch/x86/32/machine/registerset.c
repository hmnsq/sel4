/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <sel4/arch/constants.h>
#অন্তর্ভাৱ <arch/machine/registerset.h>
#অন্তর্ভাৱ <machine/fpu.h>
#অন্তর্ভাৱ <arch/object/structures.h>
#অন্তর্ভাৱ <machine/debug.h>

ধ্রুৱ register_t বাৰ্তা_Registers[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    পরিমাণ(বাৰ্তা_Registers) / পরিমাণ(বাৰ্তা_Registers[0]) == n_বাৰ্তা_Registers
);

ধ্রুৱ register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    পরিমাণ(frameRegisters) / পরিমাণ(frameRegisters[0]) == n_frameRegisters
);

ধ্রুৱ register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    পরিমাণ(gpRegisters) / পরিমাণ(gpRegisters[0]) == n_gpRegisters
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
    ঘূৰাই v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(শূন্য)
{
    seL4_IPCBuffer *buffer = (seL4_IPCBuffer *) lookupIPCBuffer(false, গ্ৰন্থি_STATE(ksCurThread));
    যদি (buffer != রিক্ত) {
        ঘূৰাই buffer->userData;
    } অন্যথা {
        ঘূৰাই 0;
    }
}
#endif
