/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/machine/fpu.h>
#అన్తర్భావ <arch/machine/cpu_registers.h>
#అన్తర్భావ <arch/object/structures.h>
#అన్తర్భావ <arch/machine/fpu.h>

/*
 * Setup the FPU register state for a new thread.
 */
శూన్య Arch_initFpuContext(user_context_t *context)
{
    context->fpuState = x86KSnullFpuState;
}

/*
 * Initialise the FPU for this machine.
 */
BOOT_సంకేతం bool_t Arch_initFpu(శూన్య)
{
    /* Enable FPU / SSE / SSE2 / SSE3 / SSSE3 / SSE4 Extensions. */
    write_cr4(read_cr4() | CR4_OSFXSR);

    /* Enable the FPU in general. */
    write_cr0((read_cr0() & ~CR0_EMULATION) | CR0_MONITOR_COPROC | CR0_NUMERIC_ERROR);
    enableFpu();

    /* Initialize the fpu state */
    finit();

    ఒకవేళ (config_సమితి(CONFIG_XSAVE)) {
        uint64_t xsave_features;
        uint32_t xsave_instruction;
        uint64_t desired_features = config_ternary(CONFIG_XSAVE, CONFIG_XSAVE_FEATURE_సమితి, 1);

        /* check for XSAVE support */
        ఒకవేళ (!(x86_cpuid_ecx(1, 0) & బిట్టు(26))) {
            రాయి("XSAVE not supported\n");
            తిరిగి false;
        }
        /* enable XSAVE support */
        write_cr4(read_cr4() | CR4_OSXSAVE);
        /* check feature mask */
        xsave_features = ((uint64_t)x86_cpuid_edx(0x0d, 0x0) << 32) | x86_cpuid_eax(0x0d, 0x0);
        ఒకవేళ ((xsave_features & desired_features) != desired_features) {
            రాయి("Requested feature mask is 0x%llx, but only 0x%llx supported\n", desired_features, (దీర్ఘ దీర్ఘ)xsave_features);
            తిరిగి false;
        }
        /* enable feature mask */
        write_xcr0(desired_features);
        /* validate the xsave buffer size and instruction */
        ఒకవేళ (x86_cpuid_ebx(0x0d, 0x0) > CONFIG_XSAVE_కొలత) {
            రాయి("XSAVE buffer set set to %d, but needs to be at least %d\n", CONFIG_XSAVE_కొలత, x86_cpuid_ebx(0x0d, 0x0));
            తిరిగి false;
        }
        ఒకవేళ (x86_cpuid_ebx(0x0d, 0x0) < CONFIG_XSAVE_కొలత) {
            రాయి("XSAVE buffer set set to %d, but only needs to be %d.\n"
                   "Warning: Memory may be wasted with larger than needed TCBs.\n",
                   CONFIG_XSAVE_కొలత, x86_cpuid_ebx(0x0d, 0x0));
        }
        /* check if a specialized XSAVE instruction was requested */
        xsave_instruction = x86_cpuid_eax(0x0d, 0x1);
        ఒకవేళ (config_సమితి(CONFIG_XSAVE_XSAVEOPT)) {
            ఒకవేళ (!(xsave_instruction & బిట్టు(0))) {
                రాయి("XSAVEOPT requested, but not supported\n");
                తిరిగి false;
            }
        } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_XSAVE_XSAVEC)) {
            ఒకవేళ (!(xsave_instruction & బిట్టు(1))) {
                రాయి("XSAVEC requested, but not supported\n");
                తిరిగి false;
            }
        } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_XSAVE_XSAVES)) {
            ఒకవేళ (!(xsave_instruction & బిట్టు(3))) {
                రాయి("XSAVES requested, but not supported\n");
                తిరిగి false;
            }
        }
        /* Init MXCSR */
        గుర్తులేని పూర్ణాంకం mxcsr = MXCSR_INIT_విలువ;
        asm అస్థిర("ldmxcsr %0" :: "m"(mxcsr));
    }
    తిరిగి true;
}
