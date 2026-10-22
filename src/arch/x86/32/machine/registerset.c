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

ثابت رجسٽر_t نياپو_Registers[] = {
    EDI,
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    EBP
#جيڪڏهن_پورو
};
compile_assert(
    consistent_پيغام_registers,
    حجم(نياپو_Registers) / حجم(نياپو_Registers[0]) == n_نياپو_Registers
);

ثابت رجسٽر_t چوکٺو_Registers[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_چوکٺو_registers,
    حجم(چوکٺو_Registers) / حجم(چوکٺو_Registers[0]) == n_چوکٺو_Registers
);

ثابت رجسٽر_t gpRegisters[] = {
    FS_بنياد, GS_بنياد
};
compile_assert(
    consistent_gp_registers,
    حجم(gpRegisters) / حجم(gpRegisters[0]) == n_gpRegisters
);

فراغ گهڻائي_init_Context(user_context_t *context)
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

لفظ_t گهڻائي_sanitise_رجسٽر(رجسٽر_t reg, لفظ_t v)
{
    أرجع v;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
لفظ_t getNBSendRecvDest(فراغ)
{
    seL4_IPCBuffer *بفر = (seL4_IPCBuffer *) lookupIPCBuffer(false, ڳنڍ_STATE(ksCurThread));
    إذا (بفر != عدم) {
        أرجع بفر->user_معلومات;
    } وإلا {
        أرجع 0;
    }
}
#جيڪڏهن_پورو
