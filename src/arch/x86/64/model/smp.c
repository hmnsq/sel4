/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <mode/model/smp.h>

#अगर_घोषित ENABLE_SMP_SUPPORT

गाँठ_Info_t गाँठ_info[CONFIG_MAX_अंक_NODES] ALIGN(L1_CACHE_पंक्ति_आकार) VISIBLE;
अक्षर गाँठ_Skim_Scratch[CONFIG_MAX_अंक_NODES][माप(गाँठ_Info_t)] ALIGN(L1_CACHE_पंक्ति_आकार);
बाह्य अक्षर kernel_ढेर_alloc[CONFIG_MAX_अंक_NODES][BIT(CONFIG_KERNEL_ढेर_BITS)];

BOOT_CODE व्योम mode_init_tls(cpu_id_t cpu_सूचकांक)
{
    गाँठ_info[cpu_सूचकांक].ढेर_शीर्ष_2 = kernel_ढेर_alloc[cpu_सूचकांक + 1];
    गाँठ_info[cpu_सूचकांक].irq_ढेर_2 = &x64KSIRQStack[cpu_सूचकांक][0];
    गाँठ_info[cpu_सूचकांक].सूचकांक = cpu_सूचकांक;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&गाँठ_info[cpu_सूचकांक]);
    swapgs();
}

#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
