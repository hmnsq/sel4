/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

ଗଣ୍ଠି_Info_t ଗଣ୍ଠି_info[CONFIG_MAX_ଅଙ୍କ_NODES] ALIGN(L1_CACHE_ଧାଡ଼ି_SIZE) VISIBLE;
ଅକ୍ଷର ଗଣ୍ଠି_Skim_Scratch[CONFIG_MAX_ଅଙ୍କ_NODES][ଆକାର(ଗଣ୍ଠି_Info_t)] ALIGN(L1_CACHE_ଧାଡ଼ି_SIZE);
ବାହ୍ୟ ଅକ୍ଷର kernel_ଗଦା_alloc[CONFIG_MAX_ଅଙ୍କ_NODES][BIT(CONFIG_KERNEL_ଗଦା_BITS)];

BOOT_CODE ଶୂନ୍ୟ mode_init_tls(cpu_id_t cpu_index)
{
    ଗଣ୍ଠି_info[cpu_index].ଗଦା_ଶୀର୍ଷ_2 = kernel_ଗଦା_alloc[cpu_index + 1];
    ଗଣ୍ଠି_info[cpu_index].irq_ଗଦା_2 = &x64KSIRQStack[cpu_index][0];
    ଗଣ୍ଠି_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&ଗଣ୍ଠି_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
