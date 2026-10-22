/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <benchmark/benchmark.h>
#ضمن <arch/benchmark.h>

#إذا CONFIG_MAX_عدد_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_عدد_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_عدد_TRACE_POINTS];
timestamp_t ksExit;
se_L4_لفظ ks_Log_اشاریہ = 0;
se_L4_لفظ ks_Log_اشاریہ_Finalized = 0;
#جے_انت /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#جے_تعریف_شدہ CONFIG_ARM_ENABLE_PMU_حد_توں_ودھ_رکاوٹ
اتے_STATE_DEFINE(uint64_t, ccnt_عدد_overflows);
#جے_انت /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#جے_تعریف_شدہ CONFIG_ENABLE_BENCHMARKS
فراغ arm_init_ccnt(فراغ)
{

    uint32_t val = (ثنائی_ہندسہ(PMCR_ENABLE) | ثنائی_ہندسہ(PMCR_CCNT_RESET) | ثنائی_ہندسہ(PMCR_ECNT_RESET));
    SYSTEM_WRITE_لفظ(PMCR, val);

#جے_تعریف_شدہ PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_لفظ(PMCNTENSET, ثنائی_ہندسہ(CCNT_اشاریہ));
#جے_انت

#جے_تعریف_شدہ CONFIG_ARM_ENABLE_PMU_حد_توں_ودھ_رکاوٹ
    armv_enable_حد_توں_ودھ_IRQ();
#جے_انت /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#جے_انت
