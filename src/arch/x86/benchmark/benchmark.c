/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अगर CONFIG_MAX_अंक_TRACE_POINTS > 0

#समावेश <benchmark/benchmark.h>
#समावेश <arch/benchmark.h>
#समावेश <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_अंक_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_अंक_TRACE_POINTS];
timestamp_t ksExit;
seL4_Word ks_Log_सूचकांक = 0;
seL4_Word ks_Log_सूचकांक_Finalized = 0;

#पूर्ण_अगर /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
