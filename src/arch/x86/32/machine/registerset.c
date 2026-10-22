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

ثابت ثبات_t پیام_Registers[] = {
    EDI,
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    EBP
#پایان_اگر
};
compile_assert(
    consistent_پیام_registers,
    حجم(پیام_Registers) / حجم(پیام_Registers[0]) == n_پیام_Registers
);

ثابت ثبات_t قاب_Registers[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
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
    context->registers[EAX] = 0;
    context->registers[EBX] = 0;
    context->registers[ECX] = 0;
    context->registers[EDX] = 0;
    context->registers[ESI] = 0;
    context->registers[EDI] = 0;
    context->registers[EBP] = 0;
    context->registers[ESP] = 0;
}

کلمه_t نما_sanitise_ثبات(ثبات_t reg, کلمه_t v)
{
    أرجع v;
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
کلمه_t getNBSendRecvDest(فراغ)
{
    seL4_IPCBuffer *میانگیر = (seL4_IPCBuffer *) lookupIPCBuffer(false, گره_STATE(ksCurThread));
    إذا (میانگیر != عدم) {
        أرجع میانگیر->user_داده;
    } وإلا {
        أرجع 0;
    }
}
#پایان_اگر
