/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/model/smp.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT

ਗੰਢ_Info_t ਗੰਢ_info[CONFIG_MAX_ਅੰਕ_NODES] ALIGN(L1_CACHE_ਸਤਰ_ਮਾਪ) VISIBLE;
ਅੱਖਰ ਗੰਢ_Skim_Scratch[CONFIG_MAX_ਅੰਕ_NODES][ਆਕਾਰ(ਗੰਢ_Info_t)] ALIGN(L1_CACHE_ਸਤਰ_ਮਾਪ);
ਬਾਹਰੀ ਅੱਖਰ kernel_ਢੇਰ_alloc[CONFIG_MAX_ਅੰਕ_NODES][ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ)];

BOOT_ਕੋਡ ਖਾਲੀ ਬਹੁਲਕ_init_tls(cpu_id_t cpu_ਸੂਚਕਾਂਕ)
{
    ਗੰਢ_info[cpu_ਸੂਚਕਾਂਕ].ਢੇਰ_ਸਿਖਰ_2 = kernel_ਢੇਰ_alloc[cpu_ਸੂਚਕਾਂਕ + 1];
    ਗੰਢ_info[cpu_ਸੂਚਕਾਂਕ].irq_ਢੇਰ_2 = &x64KSIRQStack[cpu_ਸੂਚਕਾਂਕ][0];
    ਗੰਢ_info[cpu_ਸੂਚਕਾਂਕ].ਸੂਚਕਾਂਕ = cpu_ਸੂਚਕਾਂਕ;
    x86_wrmsr(IA32_KERNEL_GS_ਅਧਾਰ_MSR, (ਸ਼ਬਦ_t)&ਗੰਢ_info[cpu_ਸੂਚਕਾਂਕ]);
    swapgs();
}

#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
