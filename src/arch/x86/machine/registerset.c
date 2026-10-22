/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>

ਖਾਲੀ Arch_initContext(user_context_t *context)
{
    ਬਹੁਲਕ_init_Context(context);
    context->registers[FS_ਅਧਾਰ] = 0;
    context->registers[GS_ਅਧਾਰ] = 0;
    context->registers[Error] = 0;
    context->registers[FaultIP] = 0;
    context->registers[ਅਗਲਾ_IP] = 0;            /* overwritten by setNextPC() later on */
    context->registers[CS] = SEL_CS_3;
    context->registers[FLAGS] = FLAGS_USER_DEFAULT;
    context->registers[SS] = SEL_DS_3;

    Arch_initFpuContext(context);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    Arch_initBreakpointContext(&context->breakpointState);
#ਜੇ_ਅੰਤ
}

ਸ਼ਬਦ_t sanitise_ਰਜਿਸਟਰ(ਰਜਿਸਟਰ_t reg, ਸ਼ਬਦ_t v, bool_t archInfo)
{
    /* First perform any mode specific sanitization */
    v = ਬਹੁਲਕ_sanitise_ਰਜਿਸਟਰ(reg, v);
    ਜੇ (reg == FLAGS) {
        /* Set architecturally defined high and low bits */
        v |=  FLAGS_ਉੱਚਾ;
        v &= ~FLAGS_ਨੀਵਾਂ;
        /* require user to have interrupts and no traps */
        v |=  FLAGS_IF;
        v &= ~FLAGS_TF;
        /* remove any other bits that shouldn't be set */
        v &=  FLAGS_ਨਕਾਬ;
    }
    ਵਾਪਸ v;
}
