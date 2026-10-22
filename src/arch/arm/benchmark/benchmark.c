/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <benchmark/benchmark.h>
#ضمن <arch/benchmark.h>

#إذا CONFIG_MAX_رقم_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_رقم_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_رقم_TRACE_POINTS];
timestamp_t ksExit;
se_L4_کلمه ks_Log_شاخص = 0;
se_L4_کلمه ks_Log_شاخص_Finalized = 0;
#پایان_اگر /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#اگر_تعریف_شده CONFIG_ARM_ENABLE_PMU_سرریز_وقفه
به_بالا_STATE_DEFINE(uint64_t, ccnt_رقم_overflows);
#پایان_اگر /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#اگر_تعریف_شده CONFIG_ENABLE_BENCHMARKS
فراغ arm_init_ccnt(فراغ)
{

    uint32_t val = (بیت(PMCR_ENABLE) | بیت(PMCR_CCNT_RESET) | بیت(PMCR_ECNT_RESET));
    SYSTEM_WRITE_کلمه(PMCR, val);

#اگر_تعریف_شده PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_کلمه(PMCNTENSET, بیت(CCNT_شاخص));
#پایان_اگر

#اگر_تعریف_شده CONFIG_ARM_ENABLE_PMU_سرریز_وقفه
    armv_enable_سرریز_IRQ();
#پایان_اگر /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#پایان_اگر
