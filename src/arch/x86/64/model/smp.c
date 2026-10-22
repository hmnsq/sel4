/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

تۈگۈن_Info_t تۈگۈن_info[CONFIG_MAX_رەقەم_NODES] ALIGN(L1_CACHE_قۇر_SIZE) VISIBLE;
حرف تۈگۈن_Skim_Scratch[CONFIG_MAX_رەقەم_NODES][حجم(تۈگۈن_Info_t)] ALIGN(L1_CACHE_قۇر_SIZE);
خارجي حرف kernel_دۆۋە_alloc[CONFIG_MAX_رەقەم_NODES][BIT(CONFIG_KERNEL_دۆۋە_BITS)];

BOOT_CODE فراغ mode_init_tls(cpu_id_t cpu_index)
{
    تۈگۈن_info[cpu_index].دۆۋە_چوققا_2 = kernel_دۆۋە_alloc[cpu_index + 1];
    تۈگۈن_info[cpu_index].irq_دۆۋە_2 = &x64KSIRQStack[cpu_index][0];
    تۈگۈن_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&تۈگۈن_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
