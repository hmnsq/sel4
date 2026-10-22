/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

عقدة_Info_t عقدة_info[CONFIG_MAX_رقم_NODES] ALIGN(L1_CACHE_سطر_SIZE) VISIBLE;
حرف عقدة_Skim_Scratch[CONFIG_MAX_رقم_NODES][حجم(عقدة_Info_t)] ALIGN(L1_CACHE_سطر_SIZE);
خارجي حرف kernel_مكدس_alloc[CONFIG_MAX_رقم_NODES][BIT(CONFIG_KERNEL_مكدس_BITS)];

BOOT_CODE فراغ mode_init_tls(cpu_id_t cpu_index)
{
    عقدة_info[cpu_index].مكدس_قمة_2 = kernel_مكدس_alloc[cpu_index + 1];
    عقدة_info[cpu_index].irq_مكدس_2 = &x64KSIRQStack[cpu_index][0];
    عقدة_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&عقدة_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
