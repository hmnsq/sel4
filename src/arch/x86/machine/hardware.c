/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/machine/hardware.h>
#అన్తర్భావ <arch/machine/registerset.h>
#అన్తర్భావ <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_సంకేతం శూన్య init_sysenter_msrs(శూన్య)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(మాట_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(మాట_t)&handle_syscall);
    ఒకవేళ (config_సమితి(CONFIG_ARCH_IA32) && !config_సమితి(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(మాట_t)((అక్షరం *)&x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss.tss.పదాలు[0]
                                                            + 4));
    }
}

మాట_t శుద్ధ getRestartPC(tcb_t *thread)
{
    తిరిగి get_రిజిస్టరు(thread, FaultIP);
}

శూన్య సమితి_తదుపరి_PC(tcb_t *thread, మాట_t v)
{
    సమితి_రిజిస్టరు(thread, తదుపరి_IP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_సంకేతం uint32_t CONST get_Cache_పంక్తి_కొలత_బిట్లు(శూన్య)
{
    uint32_t పంక్తి_కొలత;
    uint32_t n;

    పంక్తి_కొలత = get_Cache_పంక్తి_కొలత();
    ఒకవేళ (పంక్తి_కొలత == 0) {
        రాయి("Cacheline size must be >0\n");
        తిరిగి 0;
    }

    /* determine size_bits */
    n = 0;
    అయితే (!(పంక్తి_కొలత & 1)) {
        పంక్తి_కొలత >>= 1;
        n++;
    }

    ఒకవేళ (పంక్తి_కొలత != 1) {
        రాయి("Cacheline size must be a power of two\n");
        తిరిగి 0;
    }

    తిరిగి n;
}

/* Flushes a specific memory range from the CPU cache */

శూన్య flushCacheRange(శూన్య *vaddr, uint32_t కొలత_బిట్లు)
{
    మాట_t v;

    ప్రతిజ్ఞా(కొలత_బిట్లు < se_L4_మాట_బిట్లు);
    ప్రతిజ్ఞా(IS_ALIGNED((మాట_t)vaddr, కొలత_బిట్లు));

    x86_mfence();

    కొరకు (v = విడత_కిందికి((మాట_t)vaddr, x86_KScache_పంక్తి_కొలత_బిట్లు);
         v < (మాట_t)vaddr + బిట్టు(కొలత_బిట్లు);
         v += బిట్టు(x86_KScache_పంక్తి_కొలత_బిట్లు)) {
        flush_Cache_పంక్తి((శూన్య *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_సంకేతం bool_t disablePrefetchers(శూన్య)
{
    x86_cpu_తటస్థం_t *model_info;
    uint32_t దిగువ, ఎగువ;
    మాట_t i;

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

    కొరకు (i = 0; i < ARRAY_కొలత(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        ఒకవేళ (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            దిగువ = x86_rdmsr_దిగువ(IA32_PREFETCHER_MSR);
            ఎగువ = x86_rdmsr_ఎగువ(IA32_PREFETCHER_MSR);

            దిగువ |= IA32_PREFETCHER_MSR_L2;
            దిగువ |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            దిగువ |= IA32_PREFETCHER_MSR_DCU;
            దిగువ |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)ఎగువ) << 32 | దిగువ);

            తిరిగి true;
        }
    }

    రాయి("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    తిరిగి false;
}

BOOT_సంకేతం శూన్య enablePMCUser(శూన్య)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_సంకేతం bool_t init_ibrs(శూన్య)
{
    cpuid_007h_edx_t edx;
    edx.పదాలు[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    ఒకవేళ ((config_సమితి(CONFIG_KERNEL_X86_IBRS_BASIC) || config_సమితి(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        రాయి("IBRS not supported by CPU\n");
        తిరిగి false;
    } లేకపోతే ఒకవేళ (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        రాయి("IBPB not supported by CPU\n");
        తిరిగి false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    ఒకవేళ (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.పదాలు[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        ఒకవేళ (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        రాయి("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_IBRS_ALL)) {
        ఒకవేళ (!enhanced_ibrs) {
            రాయి("Enhanced IBRS not supported by CPU\n");
            తిరిగి false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    తిరిగి true;
}
