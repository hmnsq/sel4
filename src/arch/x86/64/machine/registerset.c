/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <arch/machine/registerset.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/object/structures.h>

ध्रुव register_t सन्देशः_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_message_registers,
    परिमाण(सन्देशः_Registers) / परिमाण(सन्देशः_Registers[0]) == n_सन्देशः_Registers
);

ध्रुव register_t frameRegisters[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
};
compile_assert(
    consistent_frame_registers,
    परिमाण(frameRegisters) / परिमाण(frameRegisters[0]) == n_frameRegisters
);

ध्रुव register_t gpRegisters[] = {
    FS_BASE, GS_BASE
};
compile_assert(
    consistent_gp_registers,
    परिमाण(gpRegisters) / परिमाण(gpRegisters[0]) == n_gpRegisters
);

शून्य Mode_initContext(user_context_t *context)
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
    जइ (reg == FaultIP || reg == NextIP || reg == FS_BASE || reg == GS_BASE) {
        /* ensure instruction address is canonical */
        जइ (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    प्रत्यागम v;
}

#ifdef CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(शून्य)
{
    प्रत्यागम getRegister(ग्रन्थिः_STATE(ksCurThread), nbsendRecvDest);
}
#endif
