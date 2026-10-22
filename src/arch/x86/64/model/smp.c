/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/model/smp.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT

ಗಂಟು_Info_t ಗಂಟು_info[CONFIG_MAX_ಅಂಕೆ_NODES] ALIGN(L1_CACHE_ಸಾಲು_ಗಾತ್ರ) VISIBLE;
ಅಕ್ಷರ ಗಂಟು_Skim_Scratch[CONFIG_MAX_ಅಂಕೆ_NODES][ಗಾತ್ರ(ಗಂಟು_Info_t)] ALIGN(L1_CACHE_ಸಾಲು_ಗಾತ್ರ);
ಬಾಹ್ಯ ಅಕ್ಷರ kernel_ರಾಶಿ_alloc[CONFIG_MAX_ಅಂಕೆ_NODES][ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು)];

BOOT_ಸಂಕೇತ ಶೂನ್ಯ ಬಹುಲಕ_init_tls(cpu_id_t cpu_ಸೂಚ್ಯಂಕ)
{
    ಗಂಟು_info[cpu_ಸೂಚ್ಯಂಕ].ರಾಶಿ_ತುದಿ_2 = kernel_ರಾಶಿ_alloc[cpu_ಸೂಚ್ಯಂಕ + 1];
    ಗಂಟು_info[cpu_ಸೂಚ್ಯಂಕ].irq_ರಾಶಿ_2 = &x64KSIRQStack[cpu_ಸೂಚ್ಯಂಕ][0];
    ಗಂಟು_info[cpu_ಸೂಚ್ಯಂಕ].ಸೂಚ್ಯಂಕ = cpu_ಸೂಚ್ಯಂಕ;
    x86_wrmsr(IA32_KERNEL_GS_ತಳ_MSR, (ಶಬ್ದ_t)&ಗಂಟು_info[cpu_ಸೂಚ್ಯಂಕ]);
    swapgs();
}

#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
