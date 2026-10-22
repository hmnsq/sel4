/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <arch/machine/registerset.h>
#અન્તર્ભાવ <machine/fpu.h>
#અન્તર્ભાવ <arch/object/structures.h>

અચલ register_t સંદેશ_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_message_registers,
    કદ(સંદેશ_Registers) / કદ(સંદેશ_Registers[0]) == n_સંદેશ_Registers
);

અચલ register_t frameRegisters[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
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
    context->registers[RAX] = 0;
    context->registers[RBX] = 0;
    context->registers[RCX] = 0;
    context->registers[RDX] = 0;
    context->registers[RSI] = 0;
    context->registers[RDI] = 0;
    context->registers[RBP] = 0;
    context->registers[R8]  = 0;
    context->registers[R9]  = 0;
    context->registers[R10] = 0;
    context->registers[R11] = 0;
    context->registers[R12] = 0;
    context->registers[R13] = 0;
    context->registers[R14] = 0;
    context->registers[R15] = 0;
    context->registers[RSP] = 0;
}

word_t Mode_sanitiseRegister(register_t reg, word_t v)
{
    જો (reg == FaultIP || reg == NextIP || reg == FS_BASE || reg == GS_BASE) {
        /* ensure instruction address is canonical */
        જો (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    પાછા v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(શૂન્ય)
{
    પાછા getRegister(ગાંઠ_STATE(ksCurThread), nbsendRecvDest);
}
#endif
