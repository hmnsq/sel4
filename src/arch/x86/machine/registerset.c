/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/machine/registerset.h>

فراغ Arch_initContext(user_context_t *context)
{
    ډېرپېښ_init_Context(context);
    context->registers[FS_بنسټ] = 0;
    context->registers[GS_بنسټ] = 0;
    context->registers[Error] = 0;
    context->registers[FaultIP] = 0;
    context->registers[بل_IP] = 0;            /* overwritten by setNextPC() later on */
    context->registers[CS] = SEL_CS_3;
    context->registers[FLAGS] = FLAGS_USER_DEFAULT;
    context->registers[SS] = SEL_DS_3;

    Arch_initFpuContext(context);
#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
    Arch_initBreakpointContext(&context->breakpointState);
#د_که_پای
}

کلمه_t sanitise_ثبتوونکی(ثبتوونکی_t reg, کلمه_t v, bool_t archInfo)
{
    /* First perform any mode specific sanitization */
    v = ډېرپېښ_sanitise_ثبتوونکی(reg, v);
    إذا (reg == FLAGS) {
        /* Set architecturally defined high and low bits */
        v |=  FLAGS_لوړ;
        v &= ~FLAGS_ټیټ;
        /* require user to have interrupts and no traps */
        v |=  FLAGS_IF;
        v &= ~FLAGS_TF;
        /* remove any other bits that shouldn't be set */
        v &=  FLAGS_ماسک;
    }
    أرجع v;
}
