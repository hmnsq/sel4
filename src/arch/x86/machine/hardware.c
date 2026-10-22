/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <machine/registerset.h>
#شامل <model/statedata.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>
#شامل <arch/machine/hardware.h>
#شامل <arch/machine/registerset.h>
#شامل <linker.h>

/* initialises MSRs required to setup sysenter and sysexit */
BOOT_رمز خالی init_sysenter_msrs(خالی)
{
    x86_wrmsr(IA32_SYSENTER_CS_MSR, (uint64_t)(لفظ_t)SEL_CS_0);
    x86_wrmsr(IA32_SYSENTER_EIP_MSR, (uint64_t)(لفظ_t)&handle_syscall);
    اگر (config_سیٹ(CONFIG_ARCH_IA32) && !config_سیٹ(CONFIG_HARDWARE_DEBUG_API)) {
        /* manually add 4 bytes to x86KStss so that it is valid for both
         * 32-bit and 64-bit, although only ia32 actually requires a valid
         * sysenter esp */
        x86_wrmsr(IA32_SYSENTER_ESP_MSR, (uint64_t)(لفظ_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss.tss.الفاظ[0]
                                                            + 4));
    }
}

لفظ_t خالص getRestartPC(tcb_t *thread)
{
    واپس get_رجسٹر(thread, FaultIP);
}

خالی سیٹ_اگلا_PC(tcb_t *thread, لفظ_t v)
{
    سیٹ_رجسٹر(thread, اگلا_IP, v);
}

/* Returns the size of CPU's cacheline */
BOOT_رمز uint32_t CONST get_Cache_سطر_سائز_بٹ(خالی)
{
    uint32_t سطر_سائز;
    uint32_t n;

    سطر_سائز = get_Cache_سطر_سائز();
    اگر (سطر_سائز == 0) {
        لکھو("Cacheline size must be >0\n");
        واپس 0;
    }

    /* determine size_bits */
    n = 0;
    جب (!(سطر_سائز & 1)) {
        سطر_سائز >>= 1;
        n++;
    }

    اگر (سطر_سائز != 1) {
        لکھو("Cacheline size must be a power of two\n");
        واپس 0;
    }

    واپس n;
}

/* Flushes a specific memory range from the CPU cache */

خالی flushCacheRange(خالی *vaddr, uint32_t سائز_بٹ)
{
    لفظ_t v;

    أكد(سائز_بٹ < se_L4_لفظ_بٹ);
    أكد(IS_ALIGNED((لفظ_t)vaddr, سائز_بٹ));

    x86_mfence();

    برائے (v = باری_نیچے_کو((لفظ_t)vaddr, x86_KScache_سطر_سائز_بٹ);
         v < (لفظ_t)vaddr + ثنائی_ہندسہ(سائز_بٹ);
         v += ثنائی_ہندسہ(x86_KScache_سطر_سائز_بٹ)) {
        flush_Cache_سطر((خالی *)v);
    }
    x86_mfence();
}

/* Disables as many prefetchers as possible */
BOOT_رمز bool_t disablePrefetchers(خالی)
{
    x86_cpu_شناختی_عنصر_t *model_info;
    uint32_t کم, بلند;
    لفظ_t i;

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

    برائے (i = 0; i < ARRAY_سائز(valid_models); i++) {
        /* The model ID is only useful when hashed together with the family ID.
         * They are both meant to be combined to form a unique identifier.
         *
         * As far as I can tell though, we might be able to actually just
         * disable prefetching on anything that matches family_ID==0x6, and
         * there is no need to also look at the model_ID.
         */
        اگر (model_info->family == IA32_PREFETCHER_COMPATIBLE_FAMILIES_ID
            && model_info->model == valid_models[i]) {
            کم = x86_rdmsr_کم(IA32_PREFETCHER_MSR);
            بلند = x86_rdmsr_بلند(IA32_PREFETCHER_MSR);

            کم |= IA32_PREFETCHER_MSR_L2;
            کم |= IA32_PREFETCHER_MSR_L2_ADJACENT;
            کم |= IA32_PREFETCHER_MSR_DCU;
            کم |= IA32_PREFETCHER_MSR_DCU_IP;

            x86_wrmsr(IA32_PREFETCHER_MSR, ((uint64_t)بلند) << 32 | کم);

            واپس true;
        }
    }

    لکھو("Disabling prefetchers not implemented for CPU fam %x model %x\n",
           model_info->family, model_info->model);
    واپس false;
}

BOOT_رمز خالی enablePMCUser(خالی)
{
    write_cr4(read_cr4() | CR4_PCE);
}

BOOT_رمز bool_t init_ibrs(خالی)
{
    cpuid_007h_edx_t edx;
    edx.الفاظ[0] = x86_cpuid_edx(0x7, 0);
    bool_t support_ibrs = cpuid_007h_edx_get_ibrs_ibpb(edx);
    اگر ((config_سیٹ(CONFIG_KERNEL_X86_IBRS_BASIC) || config_سیٹ(CONFIG_KERNEL_X86_IBRS_STIBP)) && !support_ibrs) {
        لکھو("IBRS not supported by CPU\n");
        واپس false;
    } ورنہ اگر (support_ibrs) {
        /* 'disable' IBRS. For IBRS_BASIC this does nothing, and for STIBP this will cause
         * us to enable STIBP, and we can then forget about it */
        x86_disable_ibrs();
    }
    /* IBRS is also the feature flag for IBPB */
    اگر (config_سیٹ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH) && !support_ibrs) {
        لکھو("IBPB not supported by CPU\n");
        واپس false;
    }
    /* check for enhanced IBRS */
    bool_t enhanced_ibrs = false;
    اگر (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.الفاظ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        اگر (ia32_arch_capabilities_msr_get_ibrs_all(cap_msr)) {
            enhanced_ibrs = true;
        }
    }
    اگر (config_سیٹ(CONFIG_KERNEL_X86_IBRS_BASIC) && enhanced_ibrs) {
        لکھو("Kernel configured for basic IBRS, but CPU supports enhanced IBRS. "
               "Enable enhanced IBRS for improved performance\n");
    }
    اگر (config_سیٹ(CONFIG_KERNEL_X86_IBRS_ALL)) {
        اگر (!enhanced_ibrs) {
            لکھو("Enhanced IBRS not supported by CPU\n");
            واپس false;
        }
        /* enable IBRS and then we can forget about it */
        x86_enable_ibrs();
    }
    واپس true;
}
