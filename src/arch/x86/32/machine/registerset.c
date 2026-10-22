/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <sel4/arch/constants.h>
#અન્તર્ભાવ <arch/machine/registerset.h>
#અન્તર્ભાવ <machine/fpu.h>
#અન્તર્ભાવ <arch/object/structures.h>
#અન્તર્ભાવ <machine/debug.h>

અચલ register_t સંદેશ_Registers[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    કદ(સંદેશ_Registers) / કદ(સંદેશ_Registers[0]) == n_સંદેશ_Registers
);

અચલ register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    કદ(frameRegisters) / કદ(frameRegisters[0]) == n_frameRegisters
);

અચલ register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    કદ(gpRegisters) / કદ(gpRegisters[0]) == n_gpRegisters
);

શૂન્ય Mode_initContext(user_context_t *context)
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
    પાછા v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(શૂન્ય)
{
    seL4_IPCBuffer *buffer = (seL4_IPCBuffer *) lookupIPCBuffer(false, ગાંઠ_STATE(ksCurThread));
    જો (buffer != ખાલી) {
        પાછા buffer->userData;
    } નહીંતર {
        પાછા 0;
    }
}
#endif
