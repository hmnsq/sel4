/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

گرێ_Info_t گرێ_info[CONFIG_MAX_ژمارە_NODES] ALIGN(L1_CACHE_هێڵ_SIZE) VISIBLE;
حرف گرێ_Skim_Scratch[CONFIG_MAX_ژمارە_NODES][حجم(گرێ_Info_t)] ALIGN(L1_CACHE_هێڵ_SIZE);
خارجي حرف kernel_کەڵەکە_alloc[CONFIG_MAX_ژمارە_NODES][BIT(CONFIG_KERNEL_کەڵەکە_BITS)];

BOOT_CODE فراغ mode_init_tls(cpu_id_t cpu_index)
{
    گرێ_info[cpu_index].کەڵەکە_لووتکە_2 = kernel_کەڵەکە_alloc[cpu_index + 1];
    گرێ_info[cpu_index].irq_کەڵەکە_2 = &x64KSIRQStack[cpu_index][0];
    گرێ_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&گرێ_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
