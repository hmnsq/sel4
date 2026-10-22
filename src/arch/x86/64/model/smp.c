/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

गाठ_Info_t गाठ_info[CONFIG_MAX_अंक_NODES] ALIGN(L1_CACHE_ओळ_SIZE) VISIBLE;
वर्ण गाठ_Skim_Scratch[CONFIG_MAX_अंक_NODES][परिमाण(गाठ_Info_t)] ALIGN(L1_CACHE_ओळ_SIZE);
बाह्य वर्ण kernel_चळत_alloc[CONFIG_MAX_अंक_NODES][BIT(CONFIG_KERNEL_चळत_BITS)];

BOOT_CODE शून्य mode_init_tls(cpu_id_t cpu_index)
{
    गाठ_info[cpu_index].चळत_शीर्ष_2 = kernel_चळत_alloc[cpu_index + 1];
    गाठ_info[cpu_index].irq_चळत_2 = &x64KSIRQStack[cpu_index][0];
    गाठ_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&गाठ_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
