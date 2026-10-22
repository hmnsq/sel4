/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#यदि CONFIG_MAX_अङ्कः_TRACE_POINTS > 0

#अन्तर्भाव <benchmark/benchmark.h>
#अन्तर्भाव <arch/benchmark.h>
#अन्तर्भाव <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_अङ्कः_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_अङ्कः_TRACE_POINTS];
timestamp_t ksExit;
seL4_Word ks_Log_सूचकांक = 0;
seL4_Word ks_Log_सूचकांक_Finalized = 0;

#यदि_अन्तः /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
