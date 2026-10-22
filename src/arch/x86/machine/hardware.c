/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <machine/registerset.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/machine/hardware.h>
#അന്തര്ഭാവ <arch/machine/registerset.h>
#അന്തര്ഭാവ <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_CODE ശൂന്യം init_sysenter_msrs(ശൂന്യം)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(word_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(word_t)&handle_syscall);
    എങ്കിൽ (config_set(CONFIG_ARCH_IA32) && !config_set(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(word_t)((അക്ഷരം *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.വാക്കുകൾ[0]
                                                            + 4));
    }
}

word_t PURE getRestartPC(tcb_t *thread)
{
    തിരികെ getRegister(thread, FaultIP);
}

ശൂന്യം setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NextIP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_CODE uint32_t CONST get_Cache_വരി_Size_Bits(ശൂന്യം)
{
    uint32_t വരി_size;
    uint32_t n;

    വരി_size = get_Cache_വരി_Size();
    എങ്കിൽ (വരി_size == 0) {
        printf("Cacheline size must be >0\n");
        തിരികെ 0;
    }

    /* determine size_bits */
    n = 0;
    ആകുമ്പോൾ (!(വരി_size & 1)) {
        വരി_size >>= 1;
        n++;
    }

    എങ്കിൽ (വരി_size != 1) {
        printf("Cacheline size must be a power of two\n");
        തിരികെ 0;
    }

    തിരികെ n;
}

/* Flushes a specific memory range from the CPU cache */

ശൂന്യം flushCacheRange(ശൂന്യം *vaddr, uint32_t size_bits)
{
    word_t v;

    പ്രതിജ്ഞാ(size_bits < seL4_WordBits);
    പ്രതിജ്ഞാ(IS_ALIGNED((word_t)vaddr, size_bits));

    x86_mfence();

    വേണ്ടി (v = ROUND_DOWN((word_t)vaddr, x86_KScache_വരി_Size_Bits);
         v < (word_t)vaddr + BIT(size_bits);
         v += BIT(x86_KScache_വരി_Size_Bits)) {
        flush_Cache_വരി((ശൂന്യം *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_CODE bool_t disablePrefetchers(ശൂന്യം)
{
    x86_cpu_identity_t *model_info;
    uint32_t താഴെയുള്ള, മുകളിലെ;
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

    വേണ്ടി (i = 0; i < ARRAY_SIZE(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        എങ്കിൽ (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            താഴെയുള്ള = x86_rdmsr_താഴെയുള്ള(IA32_PREFETCHER_MSR);
            മുകളിലെ = x86_rdmsr_മുകളിലെ(IA32_PREFETCHER_MSR);

            താഴെയുള്ള |= IA32_PREFETCHER_MSR_L2;
            താഴെയുള്ള |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            താഴെയുള്ള |= IA32_PREFETCHER_MSR_DCU;
            താഴെയുള്ള |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)മുകളിലെ) << 32 | താഴെയുള്ള);

            തിരികെ true;
        }
    }

    printf("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    തിരികെ false;
}

BOOT_CODE ശൂന്യം enablePMCUser(ശൂന്യം)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_CODE bool_t init_ibrs(ശൂന്യം)
{
    cpuid_007h_edx_t edx;
    edx.വാക്കുകൾ[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    എങ്കിൽ ((config_set(CONFIG_KERNEL_X86_IBRS_BASIC) || config_set(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        printf("IBRS not supported by CPU\n");
        തിരികെ false;
    } അല്ലെങ്കിൽ എങ്കിൽ (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    എങ്കിൽ (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        printf("IBPB not supported by CPU\n");
        തിരികെ false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    എങ്കിൽ (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.വാക്കുകൾ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        എങ്കിൽ (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    എങ്കിൽ (config_set(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        printf("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    എങ്കിൽ (config_set(CONFIG_KERNEL_X86_IBRS_ALL)) {
        എങ്കിൽ (!enhanced_ibrs) {
            printf("Enhanced IBRS not supported by CPU\n");
            തിരികെ false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    തിരികെ true;
}
