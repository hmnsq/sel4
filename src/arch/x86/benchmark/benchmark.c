/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਜੇ CONFIG_MAX_ਅੰਕ_TRACE_POINTS > 0

#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_ਅੰਕ_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_ਅੰਕ_TRACE_POINTS];
timestamp_t ksExit;
se_L4_ਸ਼ਬਦ ks_Log_ਸੂਚਕਾਂਕ = 0;
se_L4_ਸ਼ਬਦ ks_Log_ਸੂਚਕਾਂਕ_Finalized = 0;

#ਜੇ_ਅੰਤ /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
