/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <arch/machine/registerset.h>

శూన్య Arch_initContext(user_context_t *context)
{
    బహుళకం_init_Context(context);
    context->registers[FS_పునాది] = 0;
    context->registers[GS_పునాది] = 0;
    context->registers[Error] = 0;
    context->registers[FaultIP] = 0;
    context->registers[తదుపరి_IP] = 0;            /* overwritten by setNextPC() later on */
    context->registers[CS] = SEL_CS_3;
    context->registers[FLAGS] = FLAGS_USER_DEFAULT;
    context->registers[SS] = SEL_DS_3;

    Arch_initFpuContext(context);
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    Arch_initBreakpointContext(&context->breakpointState);
#అయితే_ముగింపు
}

మాట_t sanitise_రిజిస్టరు(రిజిస్టరు_t reg, మాట_t v, bool_t archInfo)
{
    /* First perform any mode specific sanitization */
    v = బహుళకం_sanitise_రిజిస్టరు(reg, v);
    ఒకవేళ (reg == FLAGS) {
        /* Set architecturally defined high and low bits */
        v |=  FLAGS_ఎగువ;
        v &= ~FLAGS_దిగువ;
        /* require user to have interrupts and no traps */
        v |=  FLAGS_IF;
        v &= ~FLAGS_TF;
        /* remove any other bits that shouldn't be set */
        v &=  FLAGS_ముసుగు;
    }
    తిరిగి v;
}
