/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <config.h>
#அந்தர்பாவ <sel4/arch/constants.h>
#அந்தர்பாவ <arch/machine/registerset.h>
#அந்தர்பாவ <machine/fpu.h>
#அந்தர்பாவ <arch/object/structures.h>
#அந்தர்பாவ <machine/debug.h>

மாறிலி register_t செய்தி_Registers[] = {
    EDI,
#ifndef CONFIG_KERNEL_MCS
    EBP
#endif
};
compile_assert(
    consistent_message_registers,
    அளவு(செய்தி_Registers) / அளவு(செய்தி_Registers[0]) == n_செய்தி_Registers
);

மாறிலி register_t frameRegisters[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_frame_registers,
    அளவு(frameRegisters) / அளவு(frameRegisters[0]) == n_frameRegisters
);

மாறிலி register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    அளவு(gpRegisters) / அளவு(gpRegisters[0]) == n_gpRegisters
);

வெறுமை Mode_initContext(user_context_t *context)
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
    திரும்பு v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(வெறுமை)
{
    seL4_IPCBuffer *buffer = (seL4_IPCBuffer *) lookupIPCBuffer(false, முனை_STATE(ksCurThread));
    எனில் (buffer != வெற்று) {
        திரும்பு buffer->userData;
    } இல்லையெனில் {
        திரும்பு 0;
    }
}
#endif
