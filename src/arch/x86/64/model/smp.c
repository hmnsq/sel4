/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

ગાંઠ_Info_t ગાંઠ_info[CONFIG_MAX_અંક_NODES] ALIGN(L1_CACHE_લીટી_SIZE) VISIBLE;
અક્ષર ગાંઠ_Skim_Scratch[CONFIG_MAX_અંક_NODES][કદ(ગાંઠ_Info_t)] ALIGN(L1_CACHE_લીટી_SIZE);
બાહ્ય અક્ષર kernel_ઢગલો_alloc[CONFIG_MAX_અંક_NODES][BIT(CONFIG_KERNEL_ઢગલો_BITS)];

BOOT_CODE શૂન્ય mode_init_tls(cpu_id_t cpu_index)
{
    ગાંઠ_info[cpu_index].ઢગલો_ટોચ_2 = kernel_ઢગલો_alloc[cpu_index + 1];
    ગાંઠ_info[cpu_index].irq_ઢગલો_2 = &x64KSIRQStack[cpu_index][0];
    ગાંઠ_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&ગાંઠ_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
