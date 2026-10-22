/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

گره_Info_t گره_info[CONFIG_MAX_رقم_NODES] ALIGN(L1_CACHE_سطر_SIZE) VISIBLE;
نویسه گره_Skim_Scratch[CONFIG_MAX_رقم_NODES][اندازه(گره_Info_t)] ALIGN(L1_CACHE_سطر_SIZE);
بیرونی نویسه kernel_پشته_alloc[CONFIG_MAX_رقم_NODES][BIT(CONFIG_KERNEL_پشته_BITS)];

BOOT_CODE تهی mode_init_tls(cpu_id_t cpu_index)
{
    گره_info[cpu_index].پشته_بالاترین_2 = kernel_پشته_alloc[cpu_index + 1];
    گره_info[cpu_index].irq_پشته_2 = &x64KSIRQStack[cpu_index][0];
    گره_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&گره_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
