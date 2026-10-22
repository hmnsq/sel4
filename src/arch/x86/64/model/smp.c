/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/smp.h>

#که_تعریف_شوی ENABLE_SMP_SUPPORT

غوټه_Info_t غوټه_info[CONFIG_MAX_عدد_NODES] ALIGN(L1_CACHE_کرښه_اندازه) VISIBLE;
حرف غوټه_Skim_Scratch[CONFIG_MAX_عدد_NODES][حجم(غوټه_Info_t)] ALIGN(L1_CACHE_کرښه_اندازه);
خارجي حرف kernel_ډېری_alloc[CONFIG_MAX_عدد_NODES][بټ(CONFIG_KERNEL_ډېری_بټونه)];

BOOT_کوډ فراغ ډېرپېښ_init_tls(cpu_id_t cpu_شاخص)
{
    غوټه_info[cpu_شاخص].ډېری_سر_2 = kernel_ډېری_alloc[cpu_شاخص + 1];
    غوټه_info[cpu_شاخص].irq_ډېری_2 = &x64KSIRQStack[cpu_شاخص][0];
    غوټه_info[cpu_شاخص].شاخص = cpu_شاخص;
    x86_wrmsr(IA32_KERNEL_GS_بنسټ_MSR, (کلمه_t)&غوټه_info[cpu_شاخص]);
    swapgs();
}

#د_که_پای /* ENABLE_SMP_SUPPORT */
