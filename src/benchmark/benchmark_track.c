/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <benchmark/benchmark_track.h>
#شامل <model/statedata.h>

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
se_L4_لفظ ks_Log_اشاریہ;
se_L4_لفظ ks_Log_اشاریہ_Finalized;

خالی benchmark_track_exit(خالی)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_اندراج_t *ksLog = (benchmark_track_kernel_اندراج_t *) KS_LOG_PPTR;

    اگر (likely(ks_User_Log_بفر != 0)) {
        /* If Log buffer is filled, do nothing */
        اگر (likely(ks_Log_اشاریہ < MAX_LOG_سائز)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_اشاریہ].اندراج_2 = ks_Kernel_اندراج;
            ksLog[ks_Log_اشاریہ].آغاز_وقت = ksEnter;
            ksLog[ks_Log_اشاریہ].duration = duration;
            ks_Log_اشاریہ++;
        }
    }
}
#اگر_ختم /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
