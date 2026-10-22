/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <arch/machine/registerset.h>
#ضمن <machine/fpu.h>
#ضمن <arch/object/structures.h>

ثابت رجسٹر_t پیغام_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_سنیہا_registers,
    حجم(پیغام_Registers) / حجم(پیغام_Registers[0]) == n_پیغام_Registers
);

ثابت رجسٹر_t چوکھٹا_Registers[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
};
compile_assert(
    consistent_چوکھٹا_registers,
    حجم(چوکھٹا_Registers) / حجم(چوکھٹا_Registers[0]) == n_چوکھٹا_Registers
);

ثابت رجسٹر_t gpRegisters[] = {
    FS_ادھار, GS_ادھار
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

فراغ منوال_init_Context(user_context_t *context)
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

لفظ_t منوال_sanitise_رجسٹر(رجسٹر_t reg, لفظ_t v)
{
    إذا (reg == FaultIP || reg == اگلا_IP || reg == FS_ادھار || reg == GS_ادھار) {
        /* ensure instruction address is canonical */
        إذا (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    أرجع v;
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
لفظ_t getNBSendRecvDest(فراغ)
{
    أرجع get_رجسٹر(گرہ_STATE(ksCurThread), nbsendRecvDest);
}
#جے_انت
