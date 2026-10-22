/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <benchmark/benchmark_track.h>
#அந்தர்பாவ <model/statedata.h>

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
seL4_Word ksLogIndex;
seL4_Word ksLogIndexFinalized;

வெறுமை benchmark_track_exit(வெறுமை)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_entry_t *ksLog = (benchmark_track_kernel_entry_t *) KS_LOG_PPTR;

    எனில் (likely(ksUserLogBuffer != 0)) {
        /* If Log buffer is filled, do nothing */
        எனில் (likely(ksLogIndex < MAX_LOG_SIZE)) {
            duration = ksExit - ksEnter;
            ksLog[ksLogIndex].entry = ksKernelEntry;
            ksLog[ksLogIndex].start_time = ksEnter;
            ksLog[ksLogIndex].duration = duration;
            ksLogIndex++;
        }
    }
}
#endif /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
