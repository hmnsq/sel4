/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <mode/model/smp.h>

#నిర్వచితమైతే ENABLE_SMP_SUPPORT

కణుపు_Info_t కణుపు_info[CONFIG_MAX_అంకె_NODES] ALIGN(L1_CACHE_పంక్తి_కొలత) VISIBLE;
అక్షరం కణుపు_Skim_Scratch[CONFIG_MAX_అంకె_NODES][పరిమాణం(కణుపు_Info_t)] ALIGN(L1_CACHE_పంక్తి_కొలత);
బాహ్య అక్షరం kernel_దొంతర_alloc[CONFIG_MAX_అంకె_NODES][బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు)];

BOOT_సంకేతం శూన్య బహుళకం_init_tls(cpu_id_t cpu_సూచీ)
{
    కణుపు_info[cpu_సూచీ].దొంతర_శిఖరం_2 = kernel_దొంతర_alloc[cpu_సూచీ + 1];
    కణుపు_info[cpu_సూచీ].irq_దొంతర_2 = &x64KSIRQStack[cpu_సూచీ][0];
    కణుపు_info[cpu_సూచీ].సూచీ = cpu_సూచీ;
    x86_wrmsr(IA32_KERNEL_GS_పునాది_MSR, (మాట_t)&కణుపు_info[cpu_సూచీ]);
    swapgs();
}

#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
