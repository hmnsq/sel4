/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_ਕੋਡ ਖਾਲੀ init_sysenter_msrs(ਖਾਲੀ)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(ਸ਼ਬਦ_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(ਸ਼ਬਦ_t)&handle_syscall);
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_IA32) && !config_ਸੈੱਟ(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(ਸ਼ਬਦ_t)((ਅੱਖਰ *)&x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss.tss.ਸ਼ਬਦ[0]
                                                            + 4));
    }
}

ਸ਼ਬਦ_t ਸ਼ੁੱਧ getRestartPC(tcb_t *thread)
{
    ਵਾਪਸ get_ਰਜਿਸਟਰ(thread, FaultIP);
}

ਖਾਲੀ ਸੈੱਟ_ਅਗਲਾ_PC(tcb_t *thread, ਸ਼ਬਦ_t v)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਅਗਲਾ_IP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_ਕੋਡ uint32_t CONST get_Cache_ਸਤਰ_ਮਾਪ_ਬਿੱਟ(ਖਾਲੀ)
{
    uint32_t ਸਤਰ_ਮਾਪ;
    uint32_t n;

    ਸਤਰ_ਮਾਪ = get_Cache_ਸਤਰ_ਮਾਪ();
    ਜੇ (ਸਤਰ_ਮਾਪ == 0) {
        ਲਿਖੋ("Cacheline size must be >0\n");
        ਵਾਪਸ 0;
    }

    /* determine size_bits */
    n = 0;
    ਜਦੋਂ (!(ਸਤਰ_ਮਾਪ & 1)) {
        ਸਤਰ_ਮਾਪ >>= 1;
        n++;
    }

    ਜੇ (ਸਤਰ_ਮਾਪ != 1) {
        ਲਿਖੋ("Cacheline size must be a power of two\n");
        ਵਾਪਸ 0;
    }

    ਵਾਪਸ n;
}

/* Flushes a specific memory range from the CPU cache */

ਖਾਲੀ flushCacheRange(ਖਾਲੀ *vaddr, uint32_t ਮਾਪ_ਬਿੱਟ)
{
    ਸ਼ਬਦ_t v;

    ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ_ਬਿੱਟ < se_L4_ਸ਼ਬਦ_ਬਿੱਟ);
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED((ਸ਼ਬਦ_t)vaddr, ਮਾਪ_ਬਿੱਟ));

    x86_mfence();

    ਲਈ (v = ਗੇੜਾ_ਥੱਲੇ((ਸ਼ਬਦ_t)vaddr, x86_KScache_ਸਤਰ_ਮਾਪ_ਬਿੱਟ);
         v < (ਸ਼ਬਦ_t)vaddr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ_ਬਿੱਟ);
         v += ਦੋਆਧਾਰੀ_ਅੰਕ(x86_KScache_ਸਤਰ_ਮਾਪ_ਬਿੱਟ)) {
        flush_Cache_ਸਤਰ((ਖਾਲੀ *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_ਕੋਡ bool_t disablePrefetchers(ਖਾਲੀ)
{
    x86_cpu_ਪਛਾਣ_ਤੱਤ_t *model_info;
    uint32_t ਨੀਵਾਂ, ਉੱਚਾ;
    ਸ਼ਬਦ_t i;

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

    ਲਈ (i = 0; i < ARRAY_ਮਾਪ(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        ਜੇ (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            ਨੀਵਾਂ = x86_rdmsr_ਨੀਵਾਂ(IA32_PREFETCHER_MSR);
            ਉੱਚਾ = x86_rdmsr_ਉੱਚਾ(IA32_PREFETCHER_MSR);

            ਨੀਵਾਂ |= IA32_PREFETCHER_MSR_L2;
            ਨੀਵਾਂ |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            ਨੀਵਾਂ |= IA32_PREFETCHER_MSR_DCU;
            ਨੀਵਾਂ |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)ਉੱਚਾ) << 32 | ਨੀਵਾਂ);

            ਵਾਪਸ true;
        }
    }

    ਲਿਖੋ("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    ਵਾਪਸ false;
}

BOOT_ਕੋਡ ਖਾਲੀ enablePMCUser(ਖਾਲੀ)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_ਕੋਡ bool_t init_ibrs(ਖਾਲੀ)
{
    cpuid_007h_edx_t edx;
    edx.ਸ਼ਬਦ[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    ਜੇ ((config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_BASIC) || config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        ਲਿਖੋ("IBRS not supported by CPU\n");
        ਵਾਪਸ false;
    } ਨਹੀਂਤਾਂ ਜੇ (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        ਲਿਖੋ("IBPB not supported by CPU\n");
        ਵਾਪਸ false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    ਜੇ (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.ਸ਼ਬਦ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        ਜੇ (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        ਲਿਖੋ("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_ALL)) {
        ਜੇ (!enhanced_ibrs) {
            ਲਿਖੋ("Enhanced IBRS not supported by CPU\n");
            ਵਾਪਸ false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    ਵਾਪਸ true;
}
