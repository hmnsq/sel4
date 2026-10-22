/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <benchmark/benchmark_track.h>
#অন্তর্ভাব <model/statedata.h>

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
seL4_Word ks_Log_সূচক;
seL4_Word ks_Log_সূচক_Finalized;

শূন্য benchmark_track_exit(শূন্য)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_entry_t *ksLog = (benchmark_track_kernel_entry_t *) KS_LOG_PPTR;

    যদি (likely(ks_User_Log_বাফার != 0)) {
        /* If Log buffer is filled, do nothing */
        যদি (likely(ks_Log_সূচক < MAX_LOG_আকার)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_সূচক].entry = ksKernelEntry;
            ksLog[ks_Log_সূচক].start_time = ksEnter;
            ksLog[ks_Log_সূচক].duration = duration;
            ks_Log_সূচক++;
        }
    }
}
#endif /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
