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

    जर (config_set(CONFIG_XSAVE)) {
        uint64_t xsave_features;
        uint32_t xsave_instruction;
        uint64_t desired_features = config_ternary(CONFIG_XSAVE, CONFIG_XSAVE_FEATURE_SET, 1);

        /* check for XSAVE support */
        जर (!(x86_cpuid_ecx(1, 0) & BIT(26))) {
            म_लिखो("XSAVE not supported\n");
            परत false;
        }
        /* enable XSAVE support */
        write_cr4(read_cr4() | CR4_OSXSAVE);
        /* check feature mask */
        xsave_features = ((uint64_t)x86_cpuid_edx(0x0d, 0x0) << 32) | x86_cpuid_eax(0x0d, 0x0);
        जर ((xsave_features & desired_features) != desired_features) {
            म_लिखो("Requested feature mask is 0x%llx, but only 0x%llx supported\n", desired_features, (दीर्घ दीर्घ)xsave_features);
            परत false;
        }
        /* enable feature mask */
        write_xcr0(desired_features);
        /* validate the xsave buffer size and instruction */
        जर (x86_cpuid_ebx(0x0d, 0x0) > CONFIG_XSAVE_SIZE) {
            म_लिखो("XSAVE buffer set set to %d, but needs to be at least %d\n", CONFIG_XSAVE_SIZE, x86_cpuid_ebx(0x0d, 0x0));
            परत false;
        }
        जर (x86_cpuid_ebx(0x0d, 0x0) < CONFIG_XSAVE_SIZE) {
            म_लिखो("XSAVE buffer set set to %d, but only needs to be %d.\n"
                   "Warning: Memory may be wasted with larger than needed TCBs.\n",
                   CONFIG_XSAVE_SIZE, x86_cpuid_ebx(0x0d, 0x0));
        }
        /* check if a specialized XSAVE instruction was requested */
        xsave_instruction = x86_cpuid_eax(0x0d, 0x1);
        जर (config_set(CONFIG_XSAVE_XSAVEOPT)) {
            जर (!(xsave_instruction & BIT(0))) {
                म_लिखो("XSAVEOPT requested, but not supported\n");
                परत false;
            }
        } अन्यथा जर (config_set(CONFIG_XSAVE_XSAVEC)) {
            जर (!(xsave_instruction & BIT(1))) {
                म_लिखो("XSAVEC requested, but not supported\n");
                परत false;
            }
        } अन्यथा जर (config_set(CONFIG_XSAVE_XSAVES)) {
            जर (!(xsave_instruction & BIT(3))) {
                म_लिखो("XSAVES requested, but not supported\n");
                परत false;
            }
        }
        /* Init MXCSR */
        अचिन्हित पूर्णांक mxcsr = MXCSR_INIT_मूल्य;
        यंत्र अस्थिर("ldmxcsr %0" :: "m"(mxcsr));
    }
    परत true;
}
