/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#എങ്കിൽ CONFIG_MAX_അക്കം_TRACE_POINTS > 0

#അന്തര്ഭാവ <benchmark/benchmark.h>
#അന്തര്ഭാവ <arch/benchmark.h>
#അന്തര്ഭാവ <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_അക്കം_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_അക്കം_TRACE_POINTS];
timestamp_t ksExit;
seL4_Word ksLogIndex = 0;
seL4_Word ksLogIndexFinalized = 0;

#endif /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
