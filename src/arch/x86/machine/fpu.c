/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <model/statedata.h>
#ضمن <arch/machine/fpu.h>
#ضمن <arch/machine/cpu_registers.h>
#ضمن <arch/object/structures.h>
#ضمن <arch/machine/fpu.h>

/*
 * Setup the FPU register state for a new thread.
 */
فراغ Arch_initFpuContext(user_context_t *context)
{
    context->fpuState = x86KSnullFpuState;
}

/*
 * Initialise the FPU for this machine.
 */
BOOT_ڪوڊ bool_t Arch_initFpu(فراغ)
{
    /* Enable FPU / SSE / SSE2 / SSE3 / SSSE3 / SSE4 Extensions. */
    write_cr4(read_cr4() | CR4_OSFXSR);

    /* Enable the FPU in general. */
    write_cr0((read_cr0() & ~CR0_EMULATION) | CR0_MONITOR_COPROC | CR0_NUMERIC_ERROR);
    enableFpu();

    /* Initialize the fpu state */
    finit();

    إذا (config_سيٽ(CONFIG_XSAVE)) {
        uint64_t xsave_features;
        uint32_t xsave_instruction;
        uint64_t desired_features = config_ternary(CONFIG_XSAVE, CONFIG_XSAVE_FEATURE_سيٽ, 1);

        /* check for XSAVE support */
        إذا (!(x86_cpuid_ecx(1, 0) & ٻٽو_انگ(26))) {
            لکو("XSAVE not supported\n");
            أرجع false;
        }
        /* enable XSAVE support */
        write_cr4(read_cr4() | CR4_OSXSAVE);
        /* check feature mask */
        xsave_features = ((uint64_t)x86_cpuid_edx(0x0d, 0x0) << 32) | x86_cpuid_eax(0x0d, 0x0);
        إذا ((xsave_features & desired_features) != desired_features) {
            لکو("Requested feature mask is 0x%llx, but only 0x%llx supported\n", desired_features, (ڊگهو ڊگهو)xsave_features);
            أرجع false;
        }
        /* enable feature mask */
        write_xcr0(desired_features);
        /* validate the xsave buffer size and instruction */
        إذا (x86_cpuid_ebx(0x0d, 0x0) > CONFIG_XSAVE_سائيز) {
            لکو("XSAVE buffer set set to %d, but needs to be at least %d\n", CONFIG_XSAVE_سائيز, x86_cpuid_ebx(0x0d, 0x0));
            أرجع false;
        }
        إذا (x86_cpuid_ebx(0x0d, 0x0) < CONFIG_XSAVE_سائيز) {
            لکو("XSAVE buffer set set to %d, but only needs to be %d.\n"
                   "Warning: Memory may be wasted with larger than needed TCBs.\n",
                   CONFIG_XSAVE_سائيز, x86_cpuid_ebx(0x0d, 0x0));
        }
        /* check if a specialized XSAVE instruction was requested */
        xsave_instruction = x86_cpuid_eax(0x0d, 0x1);
        إذا (config_سيٽ(CONFIG_XSAVE_XSAVEOPT)) {
            إذا (!(xsave_instruction & ٻٽو_انگ(0))) {
                لکو("XSAVEOPT requested, but not supported\n");
                أرجع false;
            }
        } وإلا إذا (config_سيٽ(CONFIG_XSAVE_XSAVEC)) {
            إذا (!(xsave_instruction & ٻٽو_انگ(1))) {
                لکو("XSAVEC requested, but not supported\n");
                أرجع false;
            }
        } وإلا إذا (config_سيٽ(CONFIG_XSAVE_XSAVES)) {
            إذا (!(xsave_instruction & ٻٽو_انگ(3))) {
                لکو("XSAVES requested, but not supported\n");
                أرجع false;
            }
        }
        /* Init MXCSR */
        بنانشان صحيح mxcsr = MXCSR_INIT_قدر;
        asm متطاير("ldmxcsr %0" :: "m"(mxcsr));
    }
    أرجع true;
}
