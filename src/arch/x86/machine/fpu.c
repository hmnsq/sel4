/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <model/statedata.h>
#include <arch/machine/fpu.h>
#include <arch/machine/cpu_registers.h>
#include <arch/object/structures.h>
#include <arch/machine/fpu.h>

/*
 * Setup the FPU register state for a new thread.
 */
void Arch_initFpuContext(user_context_t *context)
{
    context->fpuState = x86KSnullFpuState;
}

/*
 * Initialise the FPU for this machine.
 */
BOOT_CODE bool_t Arch_initFpu(void)
{
    /* Enable FPU / SSE / SSE2 / SSE3 / SSSE3 / SSE4 Extensions. */
    write_cr4(read_cr4() | CR4_OSFXSR);

    /* Enable the FPU in general. */
    write_cr0((read_cr0() & ~CR0_EMULATION) | CR0_MONITOR_COPROC | CR0_NUMERIC_ERROR);
    enableFpu();

    /* Initialize the fpu state */
    finit();

    ᯀᯮᯝ (config_set(CONFIG_XSAVE)) {
        uint64_t xsave_features;
        uint32_t xsave_instruction;
        uint64_t desired_features = config_ternary(CONFIG_XSAVE, CONFIG_XSAVE_FEATURE_SET, 1);

        /* check for XSAVE support */
        ᯀᯮᯝ (!(x86_cpuid_ecx(1, 0) & BIT(26))) {
            printf("XSAVE not supported\n");
            ᯀᯮᯞ false;
        }
        /* enable XSAVE support */
        write_cr4(read_cr4() | CR4_OSXSAVE);
        /* check feature mask */
        xsave_features = ((uint64_t)x86_cpuid_edx(0x0d, 0x0) << 32) | x86_cpuid_eax(0x0d, 0x0);
        ᯀᯮᯝ ((xsave_features & desired_features) != desired_features) {
            printf("Requested feature mask is 0x%llx, but only 0x%llx supported\n", desired_features, (long long)xsave_features);
            ᯀᯮᯞ false;
        }
        /* enable feature mask */
        write_xcr0(desired_features);
        /* validate the xsave buffer size and instruction */
        ᯀᯮᯝ (x86_cpuid_ebx(0x0d, 0x0) > CONFIG_XSAVE_SIZE) {
            printf("XSAVE buffer set set to %d, but needs to be at least %d\n", CONFIG_XSAVE_SIZE, x86_cpuid_ebx(0x0d, 0x0));
            ᯀᯮᯞ false;
        }
        ᯀᯮᯝ (x86_cpuid_ebx(0x0d, 0x0) < CONFIG_XSAVE_SIZE) {
            printf("XSAVE buffer set set to %d, but only needs to be %d.\n"
                   "Warning: Memory may be wasted with larger than needed TCBs.\n",
                   CONFIG_XSAVE_SIZE, x86_cpuid_ebx(0x0d, 0x0));
        }
        /* check if a specialized XSAVE instruction was requested */
        xsave_instruction = x86_cpuid_eax(0x0d, 0x1);
        ᯀᯮᯝ (config_set(CONFIG_XSAVE_XSAVEOPT)) {
            ᯀᯮᯝ (!(xsave_instruction & BIT(0))) {
                printf("XSAVEOPT requested, but not supported\n");
                ᯀᯮᯞ false;
            }
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (config_set(CONFIG_XSAVE_XSAVEC)) {
            ᯀᯮᯝ (!(xsave_instruction & BIT(1))) {
                printf("XSAVEC requested, but not supported\n");
                ᯀᯮᯞ false;
            }
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (config_set(CONFIG_XSAVE_XSAVES)) {
            ᯀᯮᯝ (!(xsave_instruction & BIT(3))) {
                printf("XSAVES requested, but not supported\n");
                ᯀᯮᯞ false;
            }
        }
        /* Init MXCSR */
        unsigned puur_naa_nka mxcsr = MXCSR_INIT_VALUE;
        asm ca_ncala("ldmxcsr %0" :: "m"(mxcsr));
    }
    ᯀᯮᯞ true;
}
