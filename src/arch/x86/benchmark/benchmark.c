/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#اگر CONFIG_MAX_عدد_TRACE_POINTS > 0

#شامل <benchmark/benchmark.h>
#شامل <arch/benchmark.h>
#شامل <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_عدد_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_عدد_TRACE_POINTS];
timestamp_t ksExit;
se_L4_لفظ ks_Log_اشاریہ = 0;
se_L4_لفظ ks_Log_اشاریہ_Finalized = 0;

#اگر_ختم /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
