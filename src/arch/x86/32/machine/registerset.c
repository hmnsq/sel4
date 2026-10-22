/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <config.h>
#شامل <sel4/arch/constants.h>
#شامل <arch/machine/registerset.h>
#شامل <machine/fpu.h>
#شامل <arch/object/structures.h>
#شامل <machine/debug.h>

ثابت register_t پیام_Registers[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    اندازه(پیام_Registers) / اندازه(پیام_Registers[0]) == n_پیام_Registers
);

ثابت register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    اندازه(frameRegisters) / اندازه(frameRegisters[0]) == n_frameRegisters
);

ثابت register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    اندازه(gpRegisters) / اندازه(gpRegisters[0]) == n_gpRegisters
);

تهی Mode_initContext(user_context_t *context)
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
    بازگشت v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(تهی)
{
    seL4_IPCBuffer *buffer = (seL4_IPCBuffer *) lookupIPCBuffer(false, گره_STATE(ksCurThread));
    اگر (buffer != پوچ) {
        بازگشت buffer->userData;
    } وگرنه {
        بازگشت 0;
    }
}
#endif
