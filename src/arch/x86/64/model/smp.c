/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

ग्रन्थिः_Info_t ग्रन्थिः_info[CONFIG_MAX_अङ्कः_NODES] ALIGN(L1_CACHE_पङ्क्तिः_SIZE) VISIBLE;
वर्ण ग्रन्थिः_Skim_Scratch[CONFIG_MAX_अङ्कः_NODES][परिमाण(ग्रन्थिः_Info_t)] ALIGN(L1_CACHE_पङ्क्तिः_SIZE);
बाह्य वर्ण kernel_राशिः_alloc[CONFIG_MAX_अङ्कः_NODES][BIT(CONFIG_KERNEL_राशिः_BITS)];

BOOT_CODE शून्य mode_init_tls(cpu_id_t cpu_index)
{
    ग्रन्थिः_info[cpu_index].राशिः_शिखरम्_2 = kernel_राशिः_alloc[cpu_index + 1];
    ग्रन्थिः_info[cpu_index].irq_राशिः_2 = &x64KSIRQStack[cpu_index][0];
    ग्रन्थिः_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&ग्रन्थिः_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
