/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

गाँठो_Info_t गाँठो_info[CONFIG_MAX_अङ्क_NODES] ALIGN(L1_CACHE_पङ्क्ति_SIZE) VISIBLE;
वर्ण गाँठो_Skim_Scratch[CONFIG_MAX_अङ्क_NODES][परिमाण(गाँठो_Info_t)] ALIGN(L1_CACHE_पङ्क्ति_SIZE);
बाह्य वर्ण kernel_थाक_alloc[CONFIG_MAX_अङ्क_NODES][BIT(CONFIG_KERNEL_थाक_BITS)];

BOOT_CODE शून्य mode_init_tls(cpu_id_t cpu_index)
{
    गाँठो_info[cpu_index].थाक_शीर्ष_2 = kernel_थाक_alloc[cpu_index + 1];
    गाँठो_info[cpu_index].irq_थाक_2 = &x64KSIRQStack[cpu_index][0];
    गाँठो_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&गाँठो_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
