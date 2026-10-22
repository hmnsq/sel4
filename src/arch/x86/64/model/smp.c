/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/model/smp.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT

گرہ_Info_t گرہ_info[CONFIG_MAX_عدد_NODES] ALIGN(L1_CACHE_سطر_سائز) VISIBLE;
حرف گرہ_Skim_Scratch[CONFIG_MAX_عدد_NODES][حجم(گرہ_Info_t)] ALIGN(L1_CACHE_سطر_سائز);
خارجی حرف kernel_ڈھیر_alloc[CONFIG_MAX_عدد_NODES][ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ)];

BOOT_رمز خالی منوال_init_tls(cpu_id_t cpu_اشاریہ)
{
    گرہ_info[cpu_اشاریہ].ڈھیر_چوٹی_2 = kernel_ڈھیر_alloc[cpu_اشاریہ + 1];
    گرہ_info[cpu_اشاریہ].irq_ڈھیر_2 = &x64KSIRQStack[cpu_اشاریہ][0];
    گرہ_info[cpu_اشاریہ].اشاریہ = cpu_اشاریہ;
    x86_wrmsr(IA32_KERNEL_GS_بنیاد_MSR, (لفظ_t)&گرہ_info[cpu_اشاریہ]);
    swapgs();
}

#اگر_ختم /* ENABLE_SMP_SUPPORT */
