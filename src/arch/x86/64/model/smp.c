/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

গ্রন্থি_Info_t গ্রন্থি_info[CONFIG_MAX_অঙ্ক_NODES] ALIGN(L1_CACHE_সারি_আকার) VISIBLE;
অক্ষর গ্রন্থি_Skim_Scratch[CONFIG_MAX_অঙ্ক_NODES][আকার(গ্রন্থি_Info_t)] ALIGN(L1_CACHE_সারি_আকার);
বাহ্যিক অক্ষর kernel_স্তূপ_alloc[CONFIG_MAX_অঙ্ক_NODES][BIT(CONFIG_KERNEL_স্তূপ_BITS)];

BOOT_CODE শূন্য mode_init_tls(cpu_id_t cpu_সূচক)
{
    গ্রন্থি_info[cpu_সূচক].স্তূপ_শীর্ষ_2 = kernel_স্তূপ_alloc[cpu_সূচক + 1];
    গ্রন্থি_info[cpu_সূচক].irq_স্তূপ_2 = &x64KSIRQStack[cpu_সূচক][0];
    গ্রন্থি_info[cpu_সূচক].সূচক = cpu_সূচক;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&গ্রন্থি_info[cpu_সূচক]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
