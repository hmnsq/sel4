/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/structures.h>

ਅਚਲ ਰਜਿਸਟਰ_t ਸੁਨੇਹਾ_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_ਸੁਨੇਹਾ_registers,
    ਆਕਾਰ(ਸੁਨੇਹਾ_Registers) / ਆਕਾਰ(ਸੁਨੇਹਾ_Registers[0]) == n_ਸੁਨੇਹਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t ਚੌਖਟਾ_Registers[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
};
compile_assert(
    consistent_ਚੌਖਟਾ_registers,
    ਆਕਾਰ(ਚੌਖਟਾ_Registers) / ਆਕਾਰ(ਚੌਖਟਾ_Registers[0]) == n_ਚੌਖਟਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t gpRegisters[] = {
    FS_ਅਧਾਰ, GS_ਅਧਾਰ
};
compile_assert(
    consistent_gp_registers,
    ਆਕਾਰ(gpRegisters) / ਆਕਾਰ(gpRegisters[0]) == n_gpRegisters
);

ਖਾਲੀ ਬਹੁਲਕ_init_Context(user_context_t *context)
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

ਸ਼ਬਦ_t ਬਹੁਲਕ_sanitise_ਰਜਿਸਟਰ(ਰਜਿਸਟਰ_t reg, ਸ਼ਬਦ_t v)
{
    ਜੇ (reg == FaultIP || reg == ਅਗਲਾ_IP || reg == FS_ਅਧਾਰ || reg == GS_ਅਧਾਰ) {
        /* ensure instruction address is canonical */
        ਜੇ (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    ਵਾਪਸ v;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸ਼ਬਦ_t getNBSendRecvDest(ਖਾਲੀ)
{
    ਵਾਪਸ get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), nbsendRecvDest);
}
#ਜੇ_ਅੰਤ
