/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#જો CONFIG_MAX_અંક_TRACE_POINTS > 0

#અન્તર્ભાવ <benchmark/benchmark.h>
#અન્તર્ભાવ <arch/benchmark.h>
#અન્તર્ભાવ <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_અંક_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_અંક_TRACE_POINTS];
timestamp_t ksExit;
seL4_Word ksLogIndex = 0;
seL4_Word ksLogIndexFinalized = 0;

#endif /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
