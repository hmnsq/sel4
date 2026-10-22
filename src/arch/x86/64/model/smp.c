/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/model/smp.h>

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT

ग्रन्थिः_Info_t ग्रन्थिः_info[CONFIG_MAX_अङ्कः_NODES] ALIGN(L1_CACHE_पङ्क्तिः_आकार) VISIBLE;
वर्ण ग्रन्थिः_Skim_Scratch[CONFIG_MAX_अङ्कः_NODES][परिमाण(ग्रन्थिः_Info_t)] ALIGN(L1_CACHE_पङ्क्तिः_आकार);
बाह्य वर्ण kernel_राशिः_alloc[CONFIG_MAX_अङ्कः_NODES][BIT(CONFIG_KERNEL_राशिः_द्वयङ्काः)];

BOOT_संहिता शून्य बहुलकम्_init_tls(cpu_id_t cpu_सूचकांक)
{
    ग्रन्थिः_info[cpu_सूचकांक].राशिः_शिखरम्_2 = kernel_राशिः_alloc[cpu_सूचकांक + 1];
    ग्रन्थिः_info[cpu_सूचकांक].irq_राशिः_2 = &x64KSIRQStack[cpu_सूचकांक][0];
    ग्रन्थिः_info[cpu_सूचकांक].सूचकांक = cpu_सूचकांक;
    x86_wrmsr(IA32_KERNEL_GS_आधारः_MSR, (word_t)&ग्रन्थिः_info[cpu_सूचकांक]);
    swapgs();
}

#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
