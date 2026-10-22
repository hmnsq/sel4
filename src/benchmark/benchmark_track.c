/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
se_L4_ਸ਼ਬਦ ks_Log_ਸੂਚਕਾਂਕ;
se_L4_ਸ਼ਬਦ ks_Log_ਸੂਚਕਾਂਕ_Finalized;

ਖਾਲੀ benchmark_track_exit(ਖਾਲੀ)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_ਇੰਦਰਾਜ_t *ksLog = (benchmark_track_kernel_ਇੰਦਰਾਜ_t *) KS_LOG_PPTR;

    ਜੇ (likely(ks_User_Log_ਬਫਰ != 0)) {
        /* If Log buffer is filled, do nothing */
        ਜੇ (likely(ks_Log_ਸੂਚਕਾਂਕ < MAX_LOG_ਮਾਪ)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_ਸੂਚਕਾਂਕ].ਇੰਦਰਾਜ = ks_Kernel_ਇੰਦਰਾਜ;
            ksLog[ks_Log_ਸੂਚਕਾਂਕ].ਸ਼ੁਰੂ_ਸਮਾਂ = ksEnter;
            ksLog[ks_Log_ਸੂਚਕਾਂਕ].duration = duration;
            ks_Log_ਸੂਚਕਾਂਕ++;
        }
    }
}
#ਜੇ_ਅੰਤ /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
