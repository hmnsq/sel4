/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/smp.h>

#اگر_تعریف_شده ENABLE_SMP_SUPPORT

گره_Info_t گره_info[CONFIG_MAX_رقم_NODES] ALIGN(L1_CACHE_سطر_اندازه) VISIBLE;
حرف گره_Skim_Scratch[CONFIG_MAX_رقم_NODES][حجم(گره_Info_t)] ALIGN(L1_CACHE_سطر_اندازه);
خارجي حرف kernel_پشته_alloc[CONFIG_MAX_رقم_NODES][بیت(CONFIG_KERNEL_پشته_بیتها)];

BOOT_کد فراغ نما_init_tls(cpu_id_t cpu_شاخص)
{
    گره_info[cpu_شاخص].پشته_بالاترین_2 = kernel_پشته_alloc[cpu_شاخص + 1];
    گره_info[cpu_شاخص].irq_پشته_2 = &x64KSIRQStack[cpu_شاخص][0];
    گره_info[cpu_شاخص].شاخص = cpu_شاخص;
    x86_wrmsr(IA32_KERNEL_GS_پایه_MSR, (کلمه_t)&گره_info[cpu_شاخص]);
    swapgs();
}

#پایان_اگر /* ENABLE_SMP_SUPPORT */
