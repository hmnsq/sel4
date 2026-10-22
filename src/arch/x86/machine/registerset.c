/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/machine/registerset.h>

शून्य Arch_initContext(user_context_t *context)
{
    बहुलकम्_init_Context(context);
    context->registers[FS_आधारः] = 0;
    context->registers[GS_आधारः] = 0;
    context->registers[Error] = 0;
    context->registers[FaultIP] = 0;
    context->registers[अग्र_IP] = 0;            /* overwritten by setNextPC() later on */
    context->registers[CS] = SEL_CS_3;
    context->registers[FLAGS] = FLAGS_USER_DEFAULT;
    context->registers[SS] = SEL_DS_3;

    Arch_initFpuContext(context);
#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    Arch_initBreakpointContext(&context->breakpointState);
#यदि_अन्तः
}

word_t sanitise_पञ्जिकाकोशः(पञ्जिकाकोशः_t reg, word_t v, bool_t archInfo)
{
    /* First perform any mode specific sanitization */
    v = बहुलकम्_sanitise_पञ्जिकाकोशः(reg, v);
    यदि (reg == FLAGS) {
        /* Set architecturally defined high and low bits */
        v |=  FLAGS_उच्चः;
        v &= ~FLAGS_नीचः;
        /* require user to have interrupts and no traps */
        v |=  FLAGS_IF;
        v &= ~FLAGS_TF;
        /* remove any other bits that shouldn't be set */
        v &=  FLAGS_आवरणम्;
    }
    प्रत्यागम v;
}
