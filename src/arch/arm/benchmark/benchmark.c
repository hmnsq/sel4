/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <benchmark/benchmark.h>
#شامل <arch/benchmark.h>

#اگر CONFIG_MAX_عدد_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_عدد_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_عدد_TRACE_POINTS];
timestamp_t ksExit;
se_L4_لفظ ks_Log_اشاریہ = 0;
se_L4_لفظ ks_Log_اشاریہ_Finalized = 0;
#اگر_ختم /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#اگر_تعریف_شدہ CONFIG_ARM_ENABLE_PMU_حد_سے_تجاوز_مداخلت
اوپر_STATE_DEFINE(uint64_t, ccnt_عدد_overflows);
#اگر_ختم /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#اگر_تعریف_شدہ CONFIG_ENABLE_BENCHMARKS
خالی arm_init_ccnt(خالی)
{

    uint32_t val = (ثنائی_ہندسہ(PMCR_ENABLE) | ثنائی_ہندسہ(PMCR_CCNT_RESET) | ثنائی_ہندسہ(PMCR_ECNT_RESET));
    SYSTEM_WRITE_لفظ(PMCR, val);

#اگر_تعریف_شدہ PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_لفظ(PMCNTENSET, ثنائی_ہندسہ(CCNT_اشاریہ));
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARM_ENABLE_PMU_حد_سے_تجاوز_مداخلت
    armv_enable_حد_سے_تجاوز_IRQ();
#اگر_ختم /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#اگر_ختم
