/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

बुहोल_Info_t बुहोल_info[CONFIG_MAX_नुमेरो_NODES] ALIGN(L1_CACHE_लिन्या_SIZE) VISIBLE;
char बुहोल_Skim_Scratch[CONFIG_MAX_नुमेरो_NODES][sizeof(बुहोल_Info_t)] ALIGN(L1_CACHE_लिन्या_SIZE);
extern char kernel_सालानसान_alloc[CONFIG_MAX_नुमेरो_NODES][BIT(CONFIG_KERNEL_सालानसान_BITS)];

BOOT_CODE void mode_init_tls(cpu_id_t cpu_index)
{
    बुहोल_info[cpu_index].सालानसान_इताअस_2 = kernel_सालानसान_alloc[cpu_index + 1];
    बुहोल_info[cpu_index].irq_सालानसान_2 = &x64KSIRQStack[cpu_index][0];
    बुहोल_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&बुहोल_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
