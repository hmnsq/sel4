/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <arch/machine/registerset.h>
#অন্তর্ভাব <machine/fpu.h>
#অন্তর্ভাব <arch/object/structures.h>

ধ্রুবক register_t বার্তা_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_message_registers,
    আকার(বার্তা_Registers) / আকার(বার্তা_Registers[0]) == n_বার্তা_Registers
);

ধ্রুবক register_t frameRegisters[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
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
    যদি (reg == FaultIP || reg == পরবর্তী_IP || reg == FS_BASE || reg == GS_BASE) {
        /* ensure instruction address is canonical */
        যদি (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    ফেরত v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(শূন্য)
{
    ফেরত getRegister(গ্রন্থি_STATE(ksCurThread), nbsendRecvDest);
}
#endif
