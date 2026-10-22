/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/model/smp.h>

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT

ڳنڍ_Info_t ڳنڍ_info[CONFIG_MAX_انگ_NODES] ALIGN(L1_CACHE_سٽ_سائيز) VISIBLE;
حرف ڳنڍ_Skim_Scratch[CONFIG_MAX_انگ_NODES][حجم(ڳنڍ_Info_t)] ALIGN(L1_CACHE_سٽ_سائيز);
خارجي حرف kernel_ڍير_alloc[CONFIG_MAX_انگ_NODES][ٻٽو_انگ(CONFIG_KERNEL_ڍير_بٽ)];

BOOT_ڪوڊ فراغ گهڻائي_init_tls(cpu_id_t cpu_اشاريو)
{
    ڳنڍ_info[cpu_اشاريو].ڍير_چوٽي_2 = kernel_ڍير_alloc[cpu_اشاريو + 1];
    ڳنڍ_info[cpu_اشاريو].irq_ڍير_2 = &x64KSIRQStack[cpu_اشاريو][0];
    ڳنڍ_info[cpu_اشاريو].اشاريو = cpu_اشاريو;
    x86_wrmsr(IA32_KERNEL_GS_بنياد_MSR, (لفظ_t)&ڳنڍ_info[cpu_اشاريو]);
    swapgs();
}

#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
