/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/cpu_registers.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/fpu.h>

/*
 * Setup the FPU register state for a new thread.
 */
ਖਾਲੀ Arch_initFpuContext(user_context_t *context)
{
    context->fpuState = x86KSnullFpuState;
}

/*
 * Initialise the FPU for this machine.
 */
BOOT_ਕੋਡ bool_t Arch_initFpu(ਖਾਲੀ)
{
    /* Enable FPU / SSE / SSE2 / SSE3 / SSSE3 / SSE4 Extensions. */
    write_cr4(read_cr4() | CR4_OSFXSR);

    /* Enable the FPU in general. */
    write_cr0((read_cr0() & ~CR0_EMULATION) | CR0_MONITOR_COPROC | CR0_NUMERIC_ERROR);
    enableFpu();

    /* Initialize the fpu state */
    finit();

    ਜੇ (config_ਸੈੱਟ(CONFIG_XSAVE)) {
        uint64_t xsave_features;
        uint32_t xsave_instruction;
        uint64_t desired_features = config_ternary(CONFIG_XSAVE, CONFIG_XSAVE_FEATURE_ਸੈੱਟ, 1);

        /* check for XSAVE support */
        ਜੇ (!(x86_cpuid_ecx(1, 0) & ਦੋਆਧਾਰੀ_ਅੰਕ(26))) {
            ਲਿਖੋ("XSAVE not supported\n");
            ਵਾਪਸ false;
        }
        /* enable XSAVE support */
        write_cr4(read_cr4() | CR4_OSXSAVE);
        /* check feature mask */
        xsave_features = ((uint64_t)x86_cpuid_edx(0x0d, 0x0) << 32) | x86_cpuid_eax(0x0d, 0x0);
        ਜੇ ((xsave_features & desired_features) != desired_features) {
            ਲਿਖੋ("Requested feature mask is 0x%llx, but only 0x%llx supported\n", desired_features, (ਲੰਮਾ ਲੰਮਾ)xsave_features);
            ਵਾਪਸ false;
        }
        /* enable feature mask */
        write_xcr0(desired_features);
        /* validate the xsave buffer size and instruction */
        ਜੇ (x86_cpuid_ebx(0x0d, 0x0) > CONFIG_XSAVE_ਮਾਪ) {
            ਲਿਖੋ("XSAVE buffer set set to %d, but needs to be at least %d\n", CONFIG_XSAVE_ਮਾਪ, x86_cpuid_ebx(0x0d, 0x0));
            ਵਾਪਸ false;
        }
        ਜੇ (x86_cpuid_ebx(0x0d, 0x0) < CONFIG_XSAVE_ਮਾਪ) {
            ਲਿਖੋ("XSAVE buffer set set to %d, but only needs to be %d.\n"
                   "Warning: Memory may be wasted with larger than needed TCBs.\n",
                   CONFIG_XSAVE_ਮਾਪ, x86_cpuid_ebx(0x0d, 0x0));
        }
        /* check if a specialized XSAVE instruction was requested */
        xsave_instruction = x86_cpuid_eax(0x0d, 0x1);
        ਜੇ (config_ਸੈੱਟ(CONFIG_XSAVE_XSAVEOPT)) {
            ਜੇ (!(xsave_instruction & ਦੋਆਧਾਰੀ_ਅੰਕ(0))) {
                ਲਿਖੋ("XSAVEOPT requested, but not supported\n");
                ਵਾਪਸ false;
            }
        } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_XSAVE_XSAVEC)) {
            ਜੇ (!(xsave_instruction & ਦੋਆਧਾਰੀ_ਅੰਕ(1))) {
                ਲਿਖੋ("XSAVEC requested, but not supported\n");
                ਵਾਪਸ false;
            }
        } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_XSAVE_XSAVES)) {
            ਜੇ (!(xsave_instruction & ਦੋਆਧਾਰੀ_ਅੰਕ(3))) {
                ਲਿਖੋ("XSAVES requested, but not supported\n");
                ਵਾਪਸ false;
            }
        }
        /* Init MXCSR */
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ mxcsr = MXCSR_INIT_ਮੁੱਲ;
        asm ਅਸਥਿਰ("ldmxcsr %0" :: "m"(mxcsr));
    }
    ਵਾਪਸ true;
}
