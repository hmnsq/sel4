/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <model/statedata.h>
#شامل <arch/machine/fpu.h>
#شامل <arch/machine/cpu_registers.h>
#شامل <arch/object/structures.h>
#شامل <arch/machine/fpu.h>

/*
 * Setup the FPU register state for a new thread.
 */
خالی Arch_initFpuContext(user_context_t *context)
{
    context->fpuState = x86KSnullFpuState;
}

/*
 * Initialise the FPU for this machine.
 */
BOOT_رمز bool_t Arch_initFpu(خالی)
{
    /* Enable FPU / SSE / SSE2 / SSE3 / SSSE3 / SSE4 Extensions. */
    write_cr4(read_cr4() | CR4_OSFXSR);

    /* Enable the FPU in general. */
    write_cr0((read_cr0() & ~CR0_EMULATION) | CR0_MONITOR_COPROC | CR0_NUMERIC_ERROR);
    enableFpu();

    /* Initialize the fpu state */
    finit();

    اگر (config_سیٹ(CONFIG_XSAVE)) {
        uint64_t xsave_features;
        uint32_t xsave_instruction;
        uint64_t desired_features = config_ternary(CONFIG_XSAVE, CONFIG_XSAVE_FEATURE_سیٹ, 1);

        /* check for XSAVE support */
        اگر (!(x86_cpuid_ecx(1, 0) & ثنائی_ہندسہ(26))) {
            لکھو("XSAVE not supported\n");
            واپس false;
        }
        /* enable XSAVE support */
        write_cr4(read_cr4() | CR4_OSXSAVE);
        /* check feature mask */
        xsave_features = ((uint64_t)x86_cpuid_edx(0x0d, 0x0) << 32) | x86_cpuid_eax(0x0d, 0x0);
        اگر ((xsave_features & desired_features) != desired_features) {
            لکھو("Requested feature mask is 0x%llx, but only 0x%llx supported\n", desired_features, (طویل طویل)xsave_features);
            واپس false;
        }
        /* enable feature mask */
        write_xcr0(desired_features);
        /* validate the xsave buffer size and instruction */
        اگر (x86_cpuid_ebx(0x0d, 0x0) > CONFIG_XSAVE_سائز) {
            لکھو("XSAVE buffer set set to %d, but needs to be at least %d\n", CONFIG_XSAVE_سائز, x86_cpuid_ebx(0x0d, 0x0));
            واپس false;
        }
        اگر (x86_cpuid_ebx(0x0d, 0x0) < CONFIG_XSAVE_سائز) {
            لکھو("XSAVE buffer set set to %d, but only needs to be %d.\n"
                   "Warning: Memory may be wasted with larger than needed TCBs.\n",
                   CONFIG_XSAVE_سائز, x86_cpuid_ebx(0x0d, 0x0));
        }
        /* check if a specialized XSAVE instruction was requested */
        xsave_instruction = x86_cpuid_eax(0x0d, 0x1);
        اگر (config_سیٹ(CONFIG_XSAVE_XSAVEOPT)) {
            اگر (!(xsave_instruction & ثنائی_ہندسہ(0))) {
                لکھو("XSAVEOPT requested, but not supported\n");
                واپس false;
            }
        } ورنہ اگر (config_سیٹ(CONFIG_XSAVE_XSAVEC)) {
            اگر (!(xsave_instruction & ثنائی_ہندسہ(1))) {
                لکھو("XSAVEC requested, but not supported\n");
                واپس false;
            }
        } ورنہ اگر (config_سیٹ(CONFIG_XSAVE_XSAVES)) {
            اگر (!(xsave_instruction & ثنائی_ہندسہ(3))) {
                لکھو("XSAVES requested, but not supported\n");
                واپس false;
            }
        }
        /* Init MXCSR */
        غیرعلامتی صحیح mxcsr = MXCSR_INIT_قدر;
        asm متلون("ldmxcsr %0" :: "m"(mxcsr));
    }
    واپس true;
}
