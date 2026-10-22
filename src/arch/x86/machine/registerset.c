/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>

ಶೂನ್ಯ Arch_initContext(user_context_t *context)
{
    ಬಹುಲಕ_init_Context(context);
    context->registers[FS_ತಳ] = 0;
    context->registers[GS_ತಳ] = 0;
    context->registers[Error] = 0;
    context->registers[FaultIP] = 0;
    context->registers[ಮುಂದಿನ_IP] = 0;            /* overwritten by setNextPC() later on */
    context->registers[CS] = SEL_CS_3;
    context->registers[FLAGS] = FLAGS_USER_DEFAULT;
    context->registers[SS] = SEL_DS_3;

    Arch_initFpuContext(context);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    Arch_initBreakpointContext(&context->breakpointState);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶಬ್ದ_t sanitise_ರಿಜಿಸ್ಟರ್(ರಿಜಿಸ್ಟರ್_t reg, ಶಬ್ದ_t v, bool_t archInfo)
{
    /* First perform any mode specific sanitization */
    v = ಬಹುಲಕ_sanitise_ರಿಜಿಸ್ಟರ್(reg, v);
    ಆದರೆ (reg == FLAGS) {
        /* Set architecturally defined high and low bits */
        v |=  FLAGS_ಮೇಲಿನ;
        v &= ~FLAGS_ಕೆಳಗಿನ;
        /* require user to have interrupts and no traps */
        v |=  FLAGS_IF;
        v &= ~FLAGS_TF;
        /* remove any other bits that shouldn't be set */
        v &=  FLAGS_ಮುಖವಾಡ;
    }
    ಹಿಂತಿರುಗಿ v;
}
