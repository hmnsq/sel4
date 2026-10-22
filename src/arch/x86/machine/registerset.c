/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/machine/registerset.h>

فراغ Arch_initContext(user_context_t *context)
{
    گهڻائي_init_Context(context);
    context->registers[FS_بنياد] = 0;
    context->registers[GS_بنياد] = 0;
    context->registers[Error] = 0;
    context->registers[FaultIP] = 0;
    context->registers[ايندڙ_IP] = 0;            /* overwritten by setNextPC() later on */
    context->registers[CS] = SEL_CS_3;
    context->registers[FLAGS] = FLAGS_USER_DEFAULT;
    context->registers[SS] = SEL_DS_3;

    Arch_initFpuContext(context);
#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
    Arch_initBreakpointContext(&context->breakpointState);
#جيڪڏهن_پورو
}

لفظ_t sanitise_رجسٽر(رجسٽر_t reg, لفظ_t v, bool_t archInfo)
{
    /* First perform any mode specific sanitization */
    v = گهڻائي_sanitise_رجسٽر(reg, v);
    إذا (reg == FLAGS) {
        /* Set architecturally defined high and low bits */
        v |=  FLAGS_مٿاهون;
        v &= ~FLAGS_هيٺيون;
        /* require user to have interrupts and no traps */
        v |=  FLAGS_IF;
        v &= ~FLAGS_TF;
        /* remove any other bits that shouldn't be set */
        v &=  FLAGS_نقاب;
    }
    أرجع v;
}
