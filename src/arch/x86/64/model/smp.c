/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

گرہ_Info_t گرہ_info[CONFIG_MAX_عدد_NODES] ALIGN(L1_CACHE_سطر_SIZE) VISIBLE;
حرف گرہ_Skim_Scratch[CONFIG_MAX_عدد_NODES][حجم(گرہ_Info_t)] ALIGN(L1_CACHE_سطر_SIZE);
خارجي حرف kernel_ڈھیر_alloc[CONFIG_MAX_عدد_NODES][BIT(CONFIG_KERNEL_ڈھیر_BITS)];

BOOT_CODE فراغ mode_init_tls(cpu_id_t cpu_index)
{
    گرہ_info[cpu_index].ڈھیر_چوٹی_2 = kernel_ڈھیر_alloc[cpu_index + 1];
    گرہ_info[cpu_index].irq_ڈھیر_2 = &x64KSIRQStack[cpu_index][0];
    گرہ_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&گرہ_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
