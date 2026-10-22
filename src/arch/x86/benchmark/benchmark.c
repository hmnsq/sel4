/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#எனில் CONFIG_MAX_இலக்கம்_TRACE_POINTS > 0

#அந்தர்பாவ <benchmark/benchmark.h>
#அந்தர்பாவ <arch/benchmark.h>
#அந்தர்பாவ <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_இலக்கம்_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_இலக்கம்_TRACE_POINTS];
timestamp_t ksExit;
seL4_Word ksLogIndex = 0;
seL4_Word ksLogIndexFinalized = 0;

#endif /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
