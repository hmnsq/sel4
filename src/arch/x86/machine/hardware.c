/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <machine/registerset.h>
#ضمن <model/statedata.h>
#ضمن <object/structures.h>
#ضمن <arch/machine.h>
#ضمن <arch/machine/hardware.h>
#ضمن <arch/machine/registerset.h>
#ضمن <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_کد فراغ init_sysenter_msrs(فراغ)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(کلمه_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(کلمه_t)&handle_syscall);
    إذا (config_مجموعه(CONFIG_ARCH_IA32) && !config_مجموعه(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(کلمه_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.tss.واژگان[0]
                                                            + 4));
    }
}

کلمه_t خالص getRestartPC(tcb_t *thread)
{
    أرجع get_ثبات(thread, FaultIP);
}

فراغ مجموعه_بعدی_PC(tcb_t *thread, کلمه_t v)
{
    مجموعه_ثبات(thread, بعدی_IP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_کد uint32_t CONST get_Cache_سطر_اندازه_بیتها(فراغ)
{
    uint32_t سطر_اندازه;
    uint32_t n;

    سطر_اندازه = get_Cache_سطر_اندازه();
    إذا (سطر_اندازه == 0) {
        چاپ_کن("Cacheline size must be >0\n");
        أرجع 0;
    }

    /* determine size_bits */
    n = 0;
    طالما (!(سطر_اندازه & 1)) {
        سطر_اندازه >>= 1;
        n++;
    }

    إذا (سطر_اندازه != 1) {
        چاپ_کن("Cacheline size must be a power of two\n");
        أرجع 0;
    }

    أرجع n;
}

/* Flushes a specific memory range from the CPU cache */

فراغ flushCacheRange(فراغ *vaddr, uint32_t اندازه_بیتها)
{
    کلمه_t v;

    أكد(اندازه_بیتها < se_L4_کلمه_بیتها);
    أكد(IS_ALIGNED((کلمه_t)vaddr, اندازه_بیتها));

    x86_mfence();

    لكل (v = دور_پایین((کلمه_t)vaddr, x86_KScache_سطر_اندازه_بیتها);
         v < (کلمه_t)vaddr + بیت(اندازه_بیتها);
         v += بیت(x86_KScache_سطر_اندازه_بیتها)) {
        flush_Cache_سطر((فراغ *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_کد bool_t disablePrefetchers(فراغ)
{
    x86_cpu_عنصر_خنثی_t *model_info;
    uint32_t کم, بالا;
    کلمه_t i;

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

    لكل (i = 0; i < ARRAY_اندازه(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        إذا (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            کم = x86_rdmsr_کم(IA32_PREFETCHER_MSR);
            بالا = x86_rdmsr_بالا(IA32_PREFETCHER_MSR);

            کم |= IA32_PREFETCHER_MSR_L2;
            کم |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            کم |= IA32_PREFETCHER_MSR_DCU;
            کم |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)بالا) << 32 | کم);

            أرجع true;
        }
    }

    چاپ_کن("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    أرجع false;
}

BOOT_کد فراغ enablePMCUser(فراغ)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_کد bool_t init_ibrs(فراغ)
{
    cpuid_007h_edx_t edx;
    edx.واژگان[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    إذا ((config_مجموعه(CONFIG_KERNEL_X86_IBRS_BASIC) || config_مجموعه(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        چاپ_کن("IBRS not supported by CPU\n");
        أرجع false;
    } وإلا إذا (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    إذا (config_مجموعه(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        چاپ_کن("IBPB not supported by CPU\n");
        أرجع false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    إذا (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.واژگان[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        إذا (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    إذا (config_مجموعه(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        چاپ_کن("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    إذا (config_مجموعه(CONFIG_KERNEL_X86_IBRS_ALL)) {
        إذا (!enhanced_ibrs) {
            چاپ_کن("Enhanced IBRS not supported by CPU\n");
            أرجع false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    أرجع true;
}
