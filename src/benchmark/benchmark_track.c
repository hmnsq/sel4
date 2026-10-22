/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <benchmark/benchmark_track.h>
#ضمن <model/statedata.h>

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
se_L4_کلمه ks_Log_شاخص;
se_L4_کلمه ks_Log_شاخص_Finalized;

فراغ benchmark_track_exit(فراغ)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_ثبت_t *ksLog = (benchmark_track_kernel_ثبت_t *) KS_LOG_PPTR;

    إذا (likely(ks_User_Log_بفر != 0)) {
        /* If Log buffer is filled, do nothing */
        إذا (likely(ks_Log_شاخص < MAX_LOG_اندازه)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_شاخص].ثبت = ks_Kernel_ثبت;
            ksLog[ks_Log_شاخص].پیل_وخت = ksEnter;
            ksLog[ks_Log_شاخص].duration = duration;
            ks_Log_شاخص++;
        }
    }
}
#د_که_پای /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
