/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>
#ضمن <machine/fpu.h>
#ضمن <arch/object/structures.h>

ثابت ثبات_t پیام_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_پیام_registers,
    حجم(پیام_Registers) / حجم(پیام_Registers[0]) == n_پیام_Registers
);

ثابت ثبات_t قاب_Registers[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
};
compile_assert(
    consistent_قاب_registers,
    حجم(قاب_Registers) / حجم(قاب_Registers[0]) == n_قاب_Registers
);

ثابت ثبات_t gpRegisters[] = {
    FS_پایه, GS_پایه
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

فراغ نما_init_Context(user_context_t *context)
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

کلمه_t نما_sanitise_ثبات(ثبات_t reg, کلمه_t v)
{
    إذا (reg == FaultIP || reg == بعدی_IP || reg == FS_پایه || reg == GS_پایه) {
        /* ensure instruction address is canonical */
        إذا (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    أرجع v;
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
کلمه_t getNBSendRecvDest(فراغ)
{
    أرجع get_ثبات(گره_STATE(ksCurThread), nbsendRecvDest);
}
#پایان_اگر
