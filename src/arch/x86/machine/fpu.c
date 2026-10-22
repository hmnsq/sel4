/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/cpu_registers.h>
#ಅನ್ತರ್ಭಾವ <arch/object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/fpu.h>

/*
 * Setup the FPU register state for a new thread.
 */
ಶೂನ್ಯ Arch_initFpuContext(user_context_t *context)
{
    context->fpuState = x86KSnullFpuState;
}

/*
 * Initialise the FPU for this machine.
 */
BOOT_ಸಂಕೇತ bool_t Arch_initFpu(ಶೂನ್ಯ)
{
    /* Enable FPU / SSE / SSE2 / SSE3 / SSSE3 / SSE4 Extensions. */
    write_cr4(read_cr4() | CR4_OSFXSR);

    /* Enable the FPU in general. */
    write_cr0((read_cr0() & ~CR0_EMULATION) | CR0_MONITOR_COPROC | CR0_NUMERIC_ERROR);
    enableFpu();

    /* Initialize the fpu state */
    finit();

    ಆದರೆ (config_ಗಣ(CONFIG_XSAVE)) {
        uint64_t xsave_features;
        uint32_t xsave_instruction;
        uint64_t desired_features = config_ternary(CONFIG_XSAVE, CONFIG_XSAVE_FEATURE_ಗಣ, 1);

        /* check for XSAVE support */
        ಆದರೆ (!(x86_cpuid_ecx(1, 0) & ಬಿಟ್(26))) {
            ಬರೆ("XSAVE not supported\n");
            ಹಿಂತಿರುಗಿ false;
        }
        /* enable XSAVE support */
        write_cr4(read_cr4() | CR4_OSXSAVE);
        /* check feature mask */
        xsave_features = ((uint64_t)x86_cpuid_edx(0x0d, 0x0) << 32) | x86_cpuid_eax(0x0d, 0x0);
        ಆದರೆ ((xsave_features & desired_features) != desired_features) {
            ಬರೆ("Requested feature mask is 0x%llx, but only 0x%llx supported\n", desired_features, (ದೀರ್ಘ ದೀರ್ಘ)xsave_features);
            ಹಿಂತಿರುಗಿ false;
        }
        /* enable feature mask */
        write_xcr0(desired_features);
        /* validate the xsave buffer size and instruction */
        ಆದರೆ (x86_cpuid_ebx(0x0d, 0x0) > CONFIG_XSAVE_ಗಾತ್ರ) {
            ಬರೆ("XSAVE buffer set set to %d, but needs to be at least %d\n", CONFIG_XSAVE_ಗಾತ್ರ, x86_cpuid_ebx(0x0d, 0x0));
            ಹಿಂತಿರುಗಿ false;
        }
        ಆದರೆ (x86_cpuid_ebx(0x0d, 0x0) < CONFIG_XSAVE_ಗಾತ್ರ) {
            ಬರೆ("XSAVE buffer set set to %d, but only needs to be %d.\n"
                   "Warning: Memory may be wasted with larger than needed TCBs.\n",
                   CONFIG_XSAVE_ಗಾತ್ರ, x86_cpuid_ebx(0x0d, 0x0));
        }
        /* check if a specialized XSAVE instruction was requested */
        xsave_instruction = x86_cpuid_eax(0x0d, 0x1);
        ಆದರೆ (config_ಗಣ(CONFIG_XSAVE_XSAVEOPT)) {
            ಆದರೆ (!(xsave_instruction & ಬಿಟ್(0))) {
                ಬರೆ("XSAVEOPT requested, but not supported\n");
                ಹಿಂತಿರುಗಿ false;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_XSAVE_XSAVEC)) {
            ಆದರೆ (!(xsave_instruction & ಬಿಟ್(1))) {
                ಬರೆ("XSAVEC requested, but not supported\n");
                ಹಿಂತಿರುಗಿ false;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_XSAVE_XSAVES)) {
            ಆದರೆ (!(xsave_instruction & ಬಿಟ್(3))) {
                ಬರೆ("XSAVES requested, but not supported\n");
                ಹಿಂತಿರುಗಿ false;
            }
        }
        /* Init MXCSR */
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ mxcsr = MXCSR_INIT_ಮೌಲ್ಯ;
        asm ಅಸ್ಥಿರ("ldmxcsr %0" :: "m"(mxcsr));
    }
    ಹಿಂತಿರುಗಿ true;
}
