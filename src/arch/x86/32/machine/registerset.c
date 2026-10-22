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

ثابت رجسٹر_t پیغام_Registers[] = {
    EDI,
#جے_بنا_تعریف CONFIG_KERNEL_MCS
    EBP
#جے_انت
};
compile_assert(
    consistent_سنیہا_registers,
    حجم(پیغام_Registers) / حجم(پیغام_Registers[0]) == n_پیغام_Registers
);

ثابت رجسٹر_t چوکھٹا_Registers[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
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
    context->registers[EAX] = 0;
    context->registers[EBX] = 0;
    context->registers[ECX] = 0;
    context->registers[EDX] = 0;
    context->registers[ESI] = 0;
    context->registers[EDI] = 0;
    context->registers[EBP] = 0;
    context->registers[ESP] = 0;
}

لفظ_t منوال_sanitise_رجسٹر(رجسٹر_t reg, لفظ_t v)
{
    أرجع v;
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
لفظ_t getNBSendRecvDest(فراغ)
{
    seL4_IPCBuffer *بفر = (seL4_IPCBuffer *) lookupIPCBuffer(false, گرہ_STATE(ksCurThread));
    إذا (بفر != عدم) {
        أرجع بفر->user_انکڑے;
    } وإلا {
        أرجع 0;
    }
}
#جے_انت
