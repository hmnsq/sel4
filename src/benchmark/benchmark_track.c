/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <benchmark/benchmark_track.h>
#समावेश <model/statedata.h>

#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
seL4_Word ks_Log_सूचकांक;
seL4_Word ks_Log_सूचकांक_Finalized;

व्योम benchmark_track_exit(व्योम)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_entry_t *ksLog = (benchmark_track_kernel_entry_t *) KS_LOG_PPTR;

    अगर (likely(ks_User_Log_बफर != 0)) {
        /* If Log buffer is filled, do nothing */
        अगर (likely(ks_Log_सूचकांक < MAX_LOG_आकार)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_सूचकांक].entry = ksKernelEntry;
            ksLog[ks_Log_सूचकांक].start_time = ksEnter;
            ksLog[ks_Log_सूचकांक].duration = duration;
            ks_Log_सूचकांक++;
        }
    }
}
#पूर्ण_अगर /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
