/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ఒకవేళ CONFIG_MAX_అంకె_TRACE_POINTS > 0

#అన్తర్భావ <benchmark/benchmark.h>
#అన్తర్భావ <arch/benchmark.h>
#అన్తర్భావ <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_అంకె_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_అంకె_TRACE_POINTS];
timestamp_t ksExit;
se_L4_మాట ks_Log_సూచీ = 0;
se_L4_మాట ks_Log_సూచీ_Finalized = 0;

#అయితే_ముగింపు /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
