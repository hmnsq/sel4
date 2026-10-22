/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/machine/fpu.h>
#अन्तर्भाव <arch/machine/cpu_registers.h>
#अन्तर्भाव <arch/object/structures.h>
#अन्तर्भाव <arch/machine/fpu.h>

/*
 * Setup the FPU register state for a new thread.
 */
शून्य Arch_initFpuContext(user_context_t *context)
{
    context->fpuState = x86KSnullFpuState;
}

/*
 * Initialise the FPU for this machine.
 */
BOOT_CODE bool_t Arch_initFpu(शून्य)
{
    /* Enable FPU / SSE / SSE2 / SSE3 / SSSE3 / SSE4 Extensions. */
    write_cr4(read_cr4() | CR4_OSFXSR);

    /* Enable the FPU in general. */
    write_cr0((read_cr0() & ~CR0_EMULATION) | CR0_MONITOR_COPROC | CR0_NUMERIC_ERROR);
    enableFpu();

    /* Initialize the fpu state */
    finit();

    यदि (config_set(CONFIG_XSAVE)) {
        uint64_t xsave_features;
        uint32_t xsave_instruction;
        uint64_t desired_features = config_ternary(CONFIG_XSAVE, CONFIG_XSAVE_FEATURE_SET, 1);

        /* check for XSAVE support */
        यदि (!(x86_cpuid_ecx(1, 0) & BIT(26))) {
            printf("XSAVE not supported\n");
            प्रत्यागम false;
        }
        /* enable XSAVE support */
        write_cr4(read_cr4() | CR4_OSXSAVE);
        /* check feature mask */
        xsave_features = ((uint64_t)x86_cpuid_edx(0x0d, 0x0) << 32) | x86_cpuid_eax(0x0d, 0x0);
        यदि ((xsave_features & desired_features) != desired_features) {
            printf("Requested feature mask is 0x%llx, but only 0x%llx supported\n", desired_features, (long long)xsave_features);
            प्रत्यागम false;
        }
        /* enable feature mask */
        write_xcr0(desired_features);
        /* validate the xsave buffer size and instruction */
        यदि (x86_cpuid_ebx(0x0d, 0x0) > CONFIG_XSAVE_SIZE) {
            printf("XSAVE buffer set set to %d, but needs to be at least %d\n", CONFIG_XSAVE_SIZE, x86_cpuid_ebx(0x0d, 0x0));
            प्रत्यागम false;
        }
        यदि (x86_cpuid_ebx(0x0d, 0x0) < CONFIG_XSAVE_SIZE) {
            printf("XSAVE buffer set set to %d, but only needs to be %d.\n"
                   "Warning: Memory may be wasted with larger than needed TCBs.\n",
                   CONFIG_XSAVE_SIZE, x86_cpuid_ebx(0x0d, 0x0));
        }
        /* check if a specialized XSAVE instruction was requested */
        xsave_instruction = x86_cpuid_eax(0x0d, 0x1);
        यदि (config_set(CONFIG_XSAVE_XSAVEOPT)) {
            यदि (!(xsave_instruction & BIT(0))) {
                printf("XSAVEOPT requested, but not supported\n");
                प्रत्यागम false;
            }
        } अन्यथा यदि (config_set(CONFIG_XSAVE_XSAVEC)) {
            यदि (!(xsave_instruction & BIT(1))) {
                printf("XSAVEC requested, but not supported\n");
                प्रत्यागम false;
            }
        } अन्यथा यदि (config_set(CONFIG_XSAVE_XSAVES)) {
            यदि (!(xsave_instruction & BIT(3))) {
                printf("XSAVES requested, but not supported\n");
                प्रत्यागम false;
            }
        }
        /* Init MXCSR */
        unsigned पूर्णाङ्क mxcsr = MXCSR_INIT_मूल्य;
        asm चञ्चल("ldmxcsr %0" :: "m"(mxcsr));
    }
    प्रत्यागम true;
}
