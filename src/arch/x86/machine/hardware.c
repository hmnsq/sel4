/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <machine/registerset.h>
#समावेश <model/statedata.h>
#समावेश <object/structures.h>
#समावेश <arch/machine.h>
#समावेश <arch/machine/hardware.h>
#समावेश <arch/machine/registerset.h>
#समावेश <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_CODE व्योम init_sysenter_msrs(व्योम)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(word_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(word_t)&handle_syscall);
    अगर (config_set(CONFIG_ARCH_IA32) && !config_set(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(word_t)((अक्षर *)&x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss.tss.शब्दगण[0]
                                                            + 4));
    }
}

word_t PURE getRestartPC(tcb_t *thread)
{
    वापस getRegister(thread, FaultIP);
}

व्योम set_अग्र_PC(tcb_t *thread, word_t v)
{
    setRegister(thread, अग्र_IP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_CODE uint32_t CONST get_Cache_पंक्ति_आकार_Bits(व्योम)
{
    uint32_t पंक्ति_आकार;
    uint32_t n;

    पंक्ति_आकार = get_Cache_पंक्ति_आकार();
    अगर (पंक्ति_आकार == 0) {
        म_लिखो("Cacheline size must be >0\n");
        वापस 0;
    }

    /* determine size_bits */
    n = 0;
    जबतक (!(पंक्ति_आकार & 1)) {
        पंक्ति_आकार >>= 1;
        n++;
    }

    अगर (पंक्ति_आकार != 1) {
        म_लिखो("Cacheline size must be a power of two\n");
        वापस 0;
    }

    वापस n;
}

/* Flushes a specific memory range from the CPU cache */

व्योम flushCacheRange(व्योम *vaddr, uint32_t आकार_bits)
{
    word_t v;

    निश्चित(आकार_bits < seL4_WordBits);
    निश्चित(IS_ALIGNED((word_t)vaddr, आकार_bits));

    x86_mfence();

    क्रम (v = ROUND_DOWN((word_t)vaddr, x86_KScache_पंक्ति_आकार_Bits);
         v < (word_t)vaddr + BIT(आकार_bits);
         v += BIT(x86_KScache_पंक्ति_आकार_Bits)) {
        flush_Cache_पंक्ति((व्योम *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_CODE bool_t disablePrefetchers(व्योम)
{
    x86_cpu_identity_t *model_info;
    uint32_t निचला, ऊपरी;
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

    क्रम (i = 0; i < ARRAY_आकार(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        अगर (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            निचला = x86_rdmsr_निचला(IA32_PREFETCHER_MSR);
            ऊपरी = x86_rdmsr_ऊपरी(IA32_PREFETCHER_MSR);

            निचला |= IA32_PREFETCHER_MSR_L2;
            निचला |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            निचला |= IA32_PREFETCHER_MSR_DCU;
            निचला |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)ऊपरी) << 32 | निचला);

            वापस true;
        }
    }

    म_लिखो("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    वापस false;
}

BOOT_CODE व्योम enablePMCUser(व्योम)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_CODE bool_t init_ibrs(व्योम)
{
    cpuid_007h_edx_t edx;
    edx.शब्दगण[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    अगर ((config_set(CONFIG_KERNEL_X86_IBRS_BASIC) || config_set(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        म_लिखो("IBRS not supported by CPU\n");
        वापस false;
    } अन्यथा अगर (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    अगर (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        म_लिखो("IBPB not supported by CPU\n");
        वापस false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    अगर (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.शब्दगण[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        अगर (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    अगर (config_set(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        म_लिखो("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    अगर (config_set(CONFIG_KERNEL_X86_IBRS_ALL)) {
        अगर (!enhanced_ibrs) {
            म_लिखो("Enhanced IBRS not supported by CPU\n");
            वापस false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    वापस true;
}
