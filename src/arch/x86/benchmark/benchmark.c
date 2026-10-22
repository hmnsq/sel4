/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಆದರೆ CONFIG_MAX_ಅಂಕೆ_TRACE_POINTS > 0

#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <arch/benchmark.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>

timestamp_t ksEntries[CONFIG_MAX_ಅಂಕೆ_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_ಅಂಕೆ_TRACE_POINTS];
timestamp_t ksExit;
se_L4_ಶಬ್ದ ks_Log_ಸೂಚ್ಯಂಕ = 0;
se_L4_ಶಬ್ದ ks_Log_ಸೂಚ್ಯಂಕ_Finalized = 0;

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */
