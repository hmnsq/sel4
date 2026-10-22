/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/hardware.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_CODE ଶୂନ୍ୟ init_sysenter_msrs(ଶୂନ୍ୟ)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(word_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(word_t)&handle_syscall);
    ଯଦି (config_set(CONFIG_ARCH_IA32) && !config_set(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(word_t)((ଅକ୍ଷର *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.ଶବ୍ଦଗୁଡ଼ିକ[0]
                                                            + 4));
    }
}

word_t PURE getRestartPC(tcb_t *thread)
{
    ଫେରନ୍ତୁ getRegister(thread, FaultIP);
}

ଶୂନ୍ୟ setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NextIP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_CODE uint32_t CONST get_Cache_ଧାଡ଼ି_Size_Bits(ଶୂନ୍ୟ)
{
    uint32_t ଧାଡ଼ି_size;
    uint32_t n;

    ଧାଡ଼ି_size = get_Cache_ଧାଡ଼ି_Size();
    ଯଦି (ଧାଡ଼ି_size == 0) {
        printf("Cacheline size must be >0\n");
        ଫେରନ୍ତୁ 0;
    }

    /* determine size_bits */
    n = 0;
    ଯେତେବେଳେ (!(ଧାଡ଼ି_size & 1)) {
        ଧାଡ଼ି_size >>= 1;
        n++;
    }

    ଯଦି (ଧାଡ଼ି_size != 1) {
        printf("Cacheline size must be a power of two\n");
        ଫେରନ୍ତୁ 0;
    }

    ଫେରନ୍ତୁ n;
}

/* Flushes a specific memory range from the CPU cache */

ଶୂନ୍ୟ flushCacheRange(ଶୂନ୍ୟ *vaddr, uint32_t size_bits)
{
    word_t v;

    ପ୍ରତିଜ୍ଞା(size_bits < seL4_WordBits);
    ପ୍ରତିଜ୍ଞା(IS_ALIGNED((word_t)vaddr, size_bits));

    x86_mfence();

    ପାଇଁ (v = ROUND_DOWN((word_t)vaddr, x86_KScache_ଧାଡ଼ି_Size_Bits);
         v < (word_t)vaddr + BIT(size_bits);
         v += BIT(x86_KScache_ଧାଡ଼ି_Size_Bits)) {
        flush_Cache_ଧାଡ଼ି((ଶୂନ୍ୟ *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_CODE bool_t disablePrefetchers(ଶୂନ୍ୟ)
{
    x86_cpu_identity_t *model_info;
    uint32_t ତଳ, ଉପର;
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

    ପାଇଁ (i = 0; i < ARRAY_SIZE(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        ଯଦି (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            ତଳ = x86_rdmsr_ତଳ(IA32_PREFETCHER_MSR);
            ଉପର = x86_rdmsr_ଉପର(IA32_PREFETCHER_MSR);

            ତଳ |= IA32_PREFETCHER_MSR_L2;
            ତଳ |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            ତଳ |= IA32_PREFETCHER_MSR_DCU;
            ତଳ |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)ଉପର) << 32 | ତଳ);

            ଫେରନ୍ତୁ true;
        }
    }

    printf("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    ଫେରନ୍ତୁ false;
}

BOOT_CODE ଶୂନ୍ୟ enablePMCUser(ଶୂନ୍ୟ)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_CODE bool_t init_ibrs(ଶୂନ୍ୟ)
{
    cpuid_007h_edx_t edx;
    edx.ଶବ୍ଦଗୁଡ଼ିକ[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    ଯଦି ((config_set(CONFIG_KERNEL_X86_IBRS_BASIC) || config_set(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        printf("IBRS not supported by CPU\n");
        ଫେରନ୍ତୁ false;
    } ଅନ୍ୟଥା ଯଦି (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    ଯଦି (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        printf("IBPB not supported by CPU\n");
        ଫେରନ୍ତୁ false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    ଯଦି (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.ଶବ୍ଦଗୁଡ଼ିକ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        ଯଦି (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    ଯଦି (config_set(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        printf("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    ଯଦି (config_set(CONFIG_KERNEL_X86_IBRS_ALL)) {
        ଯଦି (!enhanced_ibrs) {
            printf("Enhanced IBRS not supported by CPU\n");
            ଫେରନ୍ତୁ false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    ଫେରନ୍ତୁ true;
}
