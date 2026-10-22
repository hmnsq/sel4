/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <benchmark/benchmark_track.h>
#ضمن <model/statedata.h>

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
se_L4_لفظ ks_Log_اشاريو;
se_L4_لفظ ks_Log_اشاريو_Finalized;

فراغ benchmark_track_exit(فراغ)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_داخلا_t *ksLog = (benchmark_track_kernel_داخلا_t *) KS_LOG_PPTR;

    إذا (likely(ks_User_Log_بفر != 0)) {
        /* If Log buffer is filled, do nothing */
        إذا (likely(ks_Log_اشاريو < MAX_LOG_سائيز)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_اشاريو].داخلا = ks_Kernel_داخلا;
            ksLog[ks_Log_اشاريو].شروعات_وقت = ksEnter;
            ksLog[ks_Log_اشاريو].duration = duration;
            ks_Log_اشاريو++;
        }
    }
}
#جيڪڏهن_پورو /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
