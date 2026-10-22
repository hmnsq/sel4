/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_sysenter_msrs(ಶೂನ್ಯ)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(ಶಬ್ದ_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(ಶಬ್ದ_t)&handle_syscall);
    ಆದರೆ (config_ಗಣ(CONFIG_ARCH_IA32) && !config_ಗಣ(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(ಶಬ್ದ_t)((ಅಕ್ಷರ *)&x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss.tss.ಪದಗಳು[0]
                                                            + 4));
    }
}

ಶಬ್ದ_t ಶುದ್ಧ getRestartPC(tcb_t *thread)
{
    ಹಿಂತಿರುಗಿ get_ರಿಜಿಸ್ಟರ್(thread, FaultIP);
}

ಶೂನ್ಯ ಗಣ_ಮುಂದಿನ_PC(tcb_t *thread, ಶಬ್ದ_t v)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಮುಂದಿನ_IP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_ಸಂಕೇತ uint32_t CONST get_Cache_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(ಶೂನ್ಯ)
{
    uint32_t ಸಾಲು_ಗಾತ್ರ;
    uint32_t n;

    ಸಾಲು_ಗಾತ್ರ = get_Cache_ಸಾಲು_ಗಾತ್ರ();
    ಆದರೆ (ಸಾಲು_ಗಾತ್ರ == 0) {
        ಬರೆ("Cacheline size must be >0\n");
        ಹಿಂತಿರುಗಿ 0;
    }

    /* determine size_bits */
    n = 0;
    ಆಗ (!(ಸಾಲು_ಗಾತ್ರ & 1)) {
        ಸಾಲು_ಗಾತ್ರ >>= 1;
        n++;
    }

    ಆದರೆ (ಸಾಲು_ಗಾತ್ರ != 1) {
        ಬರೆ("Cacheline size must be a power of two\n");
        ಹಿಂತಿರುಗಿ 0;
    }

    ಹಿಂತಿರುಗಿ n;
}

/* Flushes a specific memory range from the CPU cache */

ಶೂನ್ಯ flushCacheRange(ಶೂನ್ಯ *vaddr, uint32_t ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    ಶಬ್ದ_t v;

    ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು < se_L4_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು);
    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED((ಶಬ್ದ_t)vaddr, ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು));

    x86_mfence();

    ಗಾಗಿ (v = ಸುತ್ತು_ಕೆಳಕ್ಕೆ((ಶಬ್ದ_t)vaddr, x86_KScache_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
         v < (ಶಬ್ದ_t)vaddr + ಬಿಟ್(ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
         v += ಬಿಟ್(x86_KScache_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        flush_Cache_ಸಾಲು((ಶೂನ್ಯ *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_ಸಂಕೇತ bool_t disablePrefetchers(ಶೂನ್ಯ)
{
    x86_cpu_ತಟಸ್ಥ_t *model_info;
    uint32_t ಕೆಳಗಿನ, ಮೇಲಿನ;
    ಶಬ್ದ_t i;

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

    ಗಾಗಿ (i = 0; i < ARRAY_ಗಾತ್ರ(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        ಆದರೆ (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            ಕೆಳಗಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_PREFETCHER_MSR);
            ಮೇಲಿನ = x86_rdmsr_ಮೇಲಿನ(IA32_PREFETCHER_MSR);

            ಕೆಳಗಿನ |= IA32_PREFETCHER_MSR_L2;
            ಕೆಳಗಿನ |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            ಕೆಳಗಿನ |= IA32_PREFETCHER_MSR_DCU;
            ಕೆಳಗಿನ |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)ಮೇಲಿನ) << 32 | ಕೆಳಗಿನ);

            ಹಿಂತಿರುಗಿ true;
        }
    }

    ಬರೆ("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    ಹಿಂತಿರುಗಿ false;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ enablePMCUser(ಶೂನ್ಯ)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_ಸಂಕೇತ bool_t init_ibrs(ಶೂನ್ಯ)
{
    cpuid_007h_edx_t edx;
    edx.ಪದಗಳು[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    ಆದರೆ ((config_ಗಣ(CONFIG_KERNEL_X86_IBRS_BASIC) || config_ಗಣ(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        ಬರೆ("IBRS not supported by CPU\n");
        ಹಿಂತಿರುಗಿ false;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        ಬರೆ("IBPB not supported by CPU\n");
        ಹಿಂತಿರುಗಿ false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    ಆದರೆ (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.ಪದಗಳು[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        ಆದರೆ (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        ಬರೆ("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_IBRS_ALL)) {
        ಆದರೆ (!enhanced_ibrs) {
            ಬರೆ("Enhanced IBRS not supported by CPU\n");
            ಹಿಂತಿರುಗಿ false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    ಹಿಂತಿರುಗಿ true;
}
