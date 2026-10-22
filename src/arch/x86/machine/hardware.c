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
BOOT_संहिता शून्य init_sysenter_msrs(शून्य)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(word_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(word_t)&handle_syscall);
    यदि (config_समुच्चयः(CONFIG_ARCH_IA32) && !config_समुच्चयः(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(word_t)((वर्ण *)&x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss.tss.शब्दाः[0]
                                                            + 4));
    }
}

word_t विशुद्धम् getRestartPC(tcb_t *thread)
{
    प्रत्यागम get_पञ्जिकाकोशः(thread, FaultIP);
}

शून्य समुच्चयः_अग्र_PC(tcb_t *thread, word_t v)
{
    समुच्चयः_पञ्जिकाकोशः(thread, अग्र_IP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_संहिता uint32_t CONST get_Cache_पङ्क्तिः_आकार_द्वयङ्काः(शून्य)
{
    uint32_t पङ्क्तिः_आकार;
    uint32_t n;

    पङ्क्तिः_आकार = get_Cache_पङ्क्तिः_आकार();
    यदि (पङ्क्तिः_आकार == 0) {
        प्रारूप_लिख("Cacheline size must be >0\n");
        प्रत्यागम 0;
    }

    /* determine size_bits */
    n = 0;
    यावत् (!(पङ्क्तिः_आकार & 1)) {
        पङ्क्तिः_आकार >>= 1;
        n++;
    }

    यदि (पङ्क्तिः_आकार != 1) {
        प्रारूप_लिख("Cacheline size must be a power of two\n");
        प्रत्यागम 0;
    }

    प्रत्यागम n;
}

/* Flushes a specific memory range from the CPU cache */

शून्य flushCacheRange(शून्य *vaddr, uint32_t आकार_द्वयङ्काः)
{
    word_t v;

    प्रतिज्ञा(आकार_द्वयङ्काः < se_L4_Word_द्वयङ्काः);
    प्रतिज्ञा(IS_ALIGNED((word_t)vaddr, आकार_द्वयङ्काः));

    x86_mfence();

    प्रति (v = आवर्तनम्_अधस्तात्((word_t)vaddr, x86_KScache_पङ्क्तिः_आकार_द्वयङ्काः);
         v < (word_t)vaddr + BIT(आकार_द्वयङ्काः);
         v += BIT(x86_KScache_पङ्क्तिः_आकार_द्वयङ्काः)) {
        flush_Cache_पङ्क्तिः((शून्य *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_संहिता bool_t disablePrefetchers(शून्य)
{
    x86_cpu_तटस्थः_t *model_info;
    uint32_t नीचः, उच्चः;
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

    प्रति (i = 0; i < ARRAY_आकार(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        यदि (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            नीचः = x86_rdmsr_नीचः(IA32_PREFETCHER_MSR);
            उच्चः = x86_rdmsr_उच्चः(IA32_PREFETCHER_MSR);

            नीचः |= IA32_PREFETCHER_MSR_L2;
            नीचः |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            नीचः |= IA32_PREFETCHER_MSR_DCU;
            नीचः |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)उच्चः) << 32 | नीचः);

            प्रत्यागम true;
        }
    }

    प्रारूप_लिख("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    प्रत्यागम false;
}

BOOT_संहिता शून्य enablePMCUser(शून्य)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_संहिता bool_t init_ibrs(शून्य)
{
    cpuid_007h_edx_t edx;
    edx.शब्दाः[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    यदि ((config_समुच्चयः(CONFIG_KERNEL_X86_IBRS_BASIC) || config_समुच्चयः(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        प्रारूप_लिख("IBRS not supported by CPU\n");
        प्रत्यागम false;
    } अन्यथा यदि (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        प्रारूप_लिख("IBPB not supported by CPU\n");
        प्रत्यागम false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    यदि (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.शब्दाः[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        यदि (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        प्रारूप_लिख("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_IBRS_ALL)) {
        यदि (!enhanced_ibrs) {
            प्रारूप_लिख("Enhanced IBRS not supported by CPU\n");
            प्रत्यागम false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    प्रत्यागम true;
}
