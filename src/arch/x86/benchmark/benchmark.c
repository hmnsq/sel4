/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#إذا CONFIG_MAX_رقم_TRACE_POINTS > 0

#ضمن <benchmark/benchmark.h>
#ضمن <arch/benchmark.h>
#ضمن <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_رقم_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_رقم_TRACE_POINTS];
timestamp_t ksExit;
se_L4_کلمه ks_Log_شاخص = 0;
se_L4_کلمه ks_Log_شاخص_Finalized = 0;

#پایان_اگر /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
