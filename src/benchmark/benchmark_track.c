/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <benchmark/benchmark_track.h>
#ضمن <model/statedata.h>

#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
se_L4_لفظ ks_Log_اشاریہ;
se_L4_لفظ ks_Log_اشاریہ_Finalized;

فراغ benchmark_track_exit(فراغ)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_اندراج_t *ksLog = (benchmark_track_kernel_اندراج_t *) KS_LOG_PPTR;

    إذا (likely(ks_User_Log_بفر != 0)) {
        /* If Log buffer is filled, do nothing */
        إذا (likely(ks_Log_اشاریہ < MAX_LOG_ناپ)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_اشاریہ].اندراج = ks_Kernel_اندراج;
            ksLog[ks_Log_اشاریہ].شروع_ویلا = ksEnter;
            ksLog[ks_Log_اشاریہ].duration = duration;
            ks_Log_اشاریہ++;
        }
    }
}
#جے_انت /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
