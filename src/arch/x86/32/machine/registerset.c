/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <config.h>
#شامل <sel4/arch/constants.h>
#شامل <arch/machine/registerset.h>
#شامل <machine/fpu.h>
#شامل <arch/object/structures.h>
#شامل <machine/debug.h>

ثابت رجسٹر_t پیغام_Registers[] = {
    EDI,
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    EBP
#اگر_ختم
};
compile_assert(
    consistent_پیغام_registers,
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
    FS_بنیاد, GS_بنیاد
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

خالی منوال_init_Context(user_context_t *context)
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
    واپس v;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
لفظ_t getNBSendRecvDest(خالی)
{
    seL4_IPCBuffer *بفر = (seL4_IPCBuffer *) lookupIPCBuffer(false, گرہ_STATE(ksCurThread));
    اگر (بفر != تہی) {
        واپس بفر->user_ڈیٹا;
    } ورنہ {
        واپس 0;
    }
}
#اگر_ختم
