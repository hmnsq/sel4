/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <mode/model/smp.h>

#ifdef ENABLE_SMP_SUPPORT

முனை_Info_t முனை_info[CONFIG_MAX_இலக்கம்_NODES] ALIGN(L1_CACHE_வரி_SIZE) VISIBLE;
எழுத்து முனை_Skim_Scratch[CONFIG_MAX_இலக்கம்_NODES][அளவு(முனை_Info_t)] ALIGN(L1_CACHE_வரி_SIZE);
வெளி எழுத்து kernel_அடுக்கு_alloc[CONFIG_MAX_இலக்கம்_NODES][BIT(CONFIG_KERNEL_அடுக்கு_BITS)];

BOOT_CODE வெறுமை mode_init_tls(cpu_id_t cpu_index)
{
    முனை_info[cpu_index].அடுக்கு_உச்சி_2 = kernel_அடுக்கு_alloc[cpu_index + 1];
    முனை_info[cpu_index].irq_அடுக்கு_2 = &x64KSIRQStack[cpu_index][0];
    முனை_info[cpu_index].index = cpu_index;
    x86_wrmsr(IA32_KERNEL_GS_BASE_MSR, (word_t)&முனை_info[cpu_index]);
    swapgs();
}

#endif /* ENABLE_SMP_SUPPORT */
