/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <config.h>
#ضمن <sel4/arch/constants.h>
#ضمن <arch/machine/registerset.h>
#ضمن <machine/fpu.h>
#ضمن <arch/object/structures.h>
#ضمن <machine/debug.h>

ثابت register_t msgRegisters[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    حجم(msgRegisters) / حجم(msgRegisters[0]) == n_msgRegisters
);

ثابت register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    حجم(frameRegisters) / حجم(frameRegisters[0]) == n_frameRegisters
);

ثابت register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

فراغ Mode_initContext(user_context_t *context)
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
    أرجع v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(فراغ)
{
    seL4_IPCBuffer *buffer = (seL4_IPCBuffer *) lookupIPCBuffer(false, NODE_STATE(ksCurThread));
    𐤀𐤌 (buffer != عدم) {
        أرجع buffer->userData;
    } وإلا {
        أرجع 0;
    }
}
#endif
