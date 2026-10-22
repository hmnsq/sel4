/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <model/statedata.h>

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
seL4_Word ks_Log_सूचकांक;
seL4_Word ks_Log_सूचकांक_Finalized;

शून्य benchmark_track_exit(शून्य)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_प्रविष्टिः_t *ksLog = (benchmark_track_kernel_प्रविष्टिः_t *) KS_LOG_PPTR;

    यदि (likely(ks_User_Log_बफर != 0)) {
        /* If Log buffer is filled, do nothing */
        यदि (likely(ks_Log_सूचकांक < MAX_LOG_आकार)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_सूचकांक].प्रविष्टिः = ks_Kernel_प्रविष्टिः;
            ksLog[ks_Log_सूचकांक].start_time = ksEnter;
            ksLog[ks_Log_सूचकांक].duration = duration;
            ks_Log_सूचकांक++;
        }
    }
}
#यदि_अन्तः /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
