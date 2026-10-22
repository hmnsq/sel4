/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <machine/registerset.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/machine/hardware.h>
#અન્તર્ભાવ <arch/machine/registerset.h>
#અન્તર્ભાવ <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_CODE શૂન્ય init_sysenter_msrs(શૂન્ય)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(word_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(word_t)&handle_syscall);
    જો (config_set(CONFIG_ARCH_IA32) && !config_set(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(word_t)((અક્ષર *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.શબ્દો[0]
                                                            + 4));
    }
}

word_t PURE getRestartPC(tcb_t *thread)
{
    પાછા getRegister(thread, FaultIP);
}

શૂન્ય setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NextIP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_CODE uint32_t CONST get_Cache_લીટી_Size_Bits(શૂન્ય)
{
    uint32_t લીટી_size;
    uint32_t n;

    લીટી_size = get_Cache_લીટી_Size();
    જો (લીટી_size == 0) {
        printf("Cacheline size must be >0\n");
        પાછા 0;
    }

    /* determine size_bits */
    n = 0;
    જ્યારે (!(લીટી_size & 1)) {
        લીટી_size >>= 1;
        n++;
    }

    જો (લીટી_size != 1) {
        printf("Cacheline size must be a power of two\n");
        પાછા 0;
    }

    પાછા n;
}

/* Flushes a specific memory range from the CPU cache */

શૂન્ય flushCacheRange(શૂન્ય *vaddr, uint32_t size_bits)
{
    word_t v;

    પ્રતિજ્ઞા(size_bits < seL4_WordBits);
    પ્રતિજ્ઞા(IS_ALIGNED((word_t)vaddr, size_bits));

    x86_mfence();

    માટે (v = ROUND_DOWN((word_t)vaddr, x86_KScache_લીટી_Size_Bits);
         v < (word_t)vaddr + BIT(size_bits);
         v += BIT(x86_KScache_લીટી_Size_Bits)) {
        flush_Cache_લીટી((શૂન્ય *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_CODE bool_t disablePrefetchers(શૂન્ય)
{
    x86_cpu_identity_t *model_info;
    uint32_t નીચલું, ઉપલું;
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

    માટે (i = 0; i < ARRAY_SIZE(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        જો (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            નીચલું = x86_rdmsr_નીચલું(IA32_PREFETCHER_MSR);
            ઉપલું = x86_rdmsr_ઉપલું(IA32_PREFETCHER_MSR);

            નીચલું |= IA32_PREFETCHER_MSR_L2;
            નીચલું |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            નીચલું |= IA32_PREFETCHER_MSR_DCU;
            નીચલું |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)ઉપલું) << 32 | નીચલું);

            પાછા true;
        }
    }

    printf("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    પાછા false;
}

BOOT_CODE શૂન્ય enablePMCUser(શૂન્ય)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_CODE bool_t init_ibrs(શૂન્ય)
{
    cpuid_007h_edx_t edx;
    edx.શબ્દો[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    જો ((config_set(CONFIG_KERNEL_X86_IBRS_BASIC) || config_set(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        printf("IBRS not supported by CPU\n");
        પાછા false;
    } નહીંતર જો (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    જો (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        printf("IBPB not supported by CPU\n");
        પાછા false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    જો (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.શબ્દો[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        જો (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    જો (config_set(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        printf("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    જો (config_set(CONFIG_KERNEL_X86_IBRS_ALL)) {
        જો (!enhanced_ibrs) {
            printf("Enhanced IBRS not supported by CPU\n");
            પાછા false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    પાછા true;
}
