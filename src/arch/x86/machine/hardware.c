/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/machine/hardware.h>
#अन्तर्भाव <arch/machine/registerset.h>
#अन्तर्भाव <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_CODE शून्य init_sysenter_msrs(शून्य)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(word_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(word_t)&handle_syscall);
    जर (config_set(CONFIG_ARCH_IA32) && !config_set(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(word_t)((अक्षर *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.शब्द[0]
                                                            + 4));
    }
}

word_t PURE getRestartPC(tcb_t *thread)
{
    परत getRegister(thread, FaultIP);
}

शून्य setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NextIP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_CODE uint32_t CONST get_Cache_ओळ_Size_Bits(शून्य)
{
    uint32_t ओळ_size;
    uint32_t n;

    ओळ_size = get_Cache_ओळ_Size();
    जर (ओळ_size == 0) {
        म_लिखो("Cacheline size must be >0\n");
        परत 0;
    }

    /* determine size_bits */
    n = 0;
    पर्यंत (!(ओळ_size & 1)) {
        ओळ_size >>= 1;
        n++;
    }

    जर (ओळ_size != 1) {
        म_लिखो("Cacheline size must be a power of two\n");
        परत 0;
    }

    परत n;
}

/* Flushes a specific memory range from the CPU cache */

शून्य flushCacheRange(शून्य *vaddr, uint32_t size_bits)
{
    word_t v;

    प्रतिज्ञा(size_bits < seL4_WordBits);
    प्रतिज्ञा(IS_ALIGNED((word_t)vaddr, size_bits));

    x86_mfence();

    साठी (v = ROUND_DOWN((word_t)vaddr, x86_KScache_ओळ_Size_Bits);
         v < (word_t)vaddr + BIT(size_bits);
         v += BIT(x86_KScache_ओळ_Size_Bits)) {
        flush_Cache_ओळ((शून्य *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_CODE bool_t disablePrefetchers(शून्य)
{
    x86_cpu_identity_t *model_info;
    uint32_t खालचा, वरचा;
    word_t i;

    uint32_t valid_models[] = { BROADWELL_1_MODEL_ID, BROADWELL_2_MODEL_ID,
                                BROADWELL_3_MODEL_ID, BROADWELL_4_MODEL_ID,
                                BROADWELL_5_MODEL_ID,
                                HASWELL_1_MODEL_ID,  HASWELL_2_MODEL_ID,
                                HASWELL_3_MODEL_ID, HASWELL_4_MODEL_ID,
                                IVY_BRIDGE_1_MODEL_ID,  IVY_BRIDGE_2_MODEL_ID,
                                IVY_BRIDGE_3_MODEL_ID,
                                SANDY_BRIDGE_1_MODEL_ID, SANDY_BRIDGE_2_MODEL_ID, WESTMERE_1_MODEL_ID, WESTMERE_2_MODEL_ID,
                                WESTMERE_3_MODEL_ID, NEHALEM_1_MODEL_ID, NEHALEM_2_MODEL_ID, NEHALEM_3_MODEL_ID,
                                SKYLAKE_1_MODEL_ID, SKYLAKE_2_MODEL_ID
                              };

    model_info = x86_cpuid_get_model_info();

    साठी (i = 0; i < ARRAY_SIZE(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        जर (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            खालचा = x86_rdmsr_खालचा(IA32_PREFETCHER_MSR);
            वरचा = x86_rdmsr_वरचा(IA32_PREFETCHER_MSR);

            खालचा |= IA32_PREFETCHER_MSR_L2;
            खालचा |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            खालचा |= IA32_PREFETCHER_MSR_DCU;
            खालचा |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)वरचा) << 32 | खालचा);

            परत true;
        }
    }

    म_लिखो("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    परत false;
}

BOOT_CODE शून्य enablePMCUser(शून्य)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_CODE bool_t init_ibrs(शून्य)
{
    cpuid_007h_edx_t edx;
    edx.शब्द[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    जर ((config_set(CONFIG_KERNEL_X86_IBRS_BASIC) || config_set(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        म_लिखो("IBRS not supported by CPU\n");
        परत false;
    } अन्यथा जर (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    जर (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        म_लिखो("IBPB not supported by CPU\n");
        परत false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    जर (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.शब्द[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        जर (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    जर (config_set(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        म_लिखो("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    जर (config_set(CONFIG_KERNEL_X86_IBRS_ALL)) {
        जर (!enhanced_ibrs) {
            म_लिखो("Enhanced IBRS not supported by CPU\n");
            परत false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    परत true;
}
