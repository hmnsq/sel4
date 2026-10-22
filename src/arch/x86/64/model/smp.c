/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

نود_Info_t نود_info[CONFIG_MAX_اڠک_NODES] ALIGN(L1_CACHE_باريس_SIZE) VISIBLE;
حرف نود_Skim_Scratch[CONFIG_MAX_اڠک_NODES][حجم(نود_Info_t)] ALIGN(L1_CACHE_باريس_SIZE);
خارجي حرف kernel_تيندنن_alloc[CONFIG_MAX_اڠک_NODES][BIT(CONFIG_KERNEL_تيندنن_BITS)];

BOOT_CODE فراغ mode_init_tls(cpu_id_t cpu_index)
{
    نود_info[cpu_index].تيندنن_اتس_2 = kernel_تيندنن_alloc[cpu_index + 1];
    نود_info[cpu_index].irq_تيندنن_2 = &x64KSIRQStack[cpu_index][0];
    نود_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&نود_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
