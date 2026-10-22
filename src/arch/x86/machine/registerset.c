/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <arch/machine/registerset.h>

خالی Arch_initContext(user_context_t *context)
{
    منوال_init_Context(context);
    context->registers[FS_بنیاد] = 0;
    context->registers[GS_بنیاد] = 0;
    context->registers[Error] = 0;
    context->registers[FaultIP] = 0;
    context->registers[اگلا_IP] = 0;            /* overwritten by setNextPC() later on */
    context->registers[CS] = SEL_CS_3;
    context->registers[FLAGS] = FLAGS_USER_DEFAULT;
    context->registers[SS] = SEL_DS_3;

    Arch_initFpuContext(context);
#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    Arch_initBreakpointContext(&context->breakpointState);
#اگر_ختم
}

لفظ_t sanitise_رجسٹر(رجسٹر_t reg, لفظ_t v, bool_t archInfo)
{
    /* First perform any mode specific sanitization */
    v = منوال_sanitise_رجسٹر(reg, v);
    اگر (reg == FLAGS) {
        /* Set architecturally defined high and low bits */
        v |=  FLAGS_بلند;
        v &= ~FLAGS_کم;
        /* require user to have interrupts and no traps */
        v |=  FLAGS_IF;
        v &= ~FLAGS_TF;
        /* remove any other bits that shouldn't be set */
        v &=  FLAGS_نقاب;
    }
    واپس v;
}
