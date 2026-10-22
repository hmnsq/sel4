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

ثابت ثبتوونکی_t پیغام_Registers[] = {
    EDI,
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    EBP
#د_که_پای
};
compile_assert(
    consistent_پیغام_registers,
    حجم(پیغام_Registers) / حجم(پیغام_Registers[0]) == n_پیغام_Registers
);

ثابت ثبتوونکی_t چوکاټ_Registers[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_چوکاټ_registers,
    حجم(چوکاټ_Registers) / حجم(چوکاټ_Registers[0]) == n_چوکاټ_Registers
);

ثابت ثبتوونکی_t gpRegisters[] = {
    FS_بنسټ, GS_بنسټ
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

فراغ ډېرپېښ_init_Context(user_context_t *context)
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

کلمه_t ډېرپېښ_sanitise_ثبتوونکی(ثبتوونکی_t reg, کلمه_t v)
{
    أرجع v;
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
کلمه_t getNBSendRecvDest(فراغ)
{
    seL4_IPCBuffer *بفر = (seL4_IPCBuffer *) lookupIPCBuffer(false, غوټه_STATE(ksCurThread));
    إذا (بفر != عدم) {
        أرجع بفر->user_معلومات;
    } وإلا {
        أرجع 0;
    }
}
#د_که_پای
