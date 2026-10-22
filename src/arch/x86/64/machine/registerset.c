/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <arch/machine/registerset.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/object/structures.h>

ध्रुव पञ्जिकाकोशः_t सन्देशः_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_सन्देशः_registers,
    परिमाण(सन्देशः_Registers) / परिमाण(सन्देशः_Registers[0]) == n_सन्देशः_Registers
);

ध्रुव पञ्जिकाकोशः_t frameRegisters[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
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

word_t बहुलकम्_sanitise_पञ्जिकाकोशः(पञ्जिकाकोशः_t reg, word_t v)
{
    यदि (reg == FaultIP || reg == अग्र_IP || reg == FS_आधारः || reg == GS_आधारः) {
        /* ensure instruction address is canonical */
        यदि (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    प्रत्यागम v;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(शून्य)
{
    प्रत्यागम get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), nbsendRecvDest);
}
#यदि_अन्तः
