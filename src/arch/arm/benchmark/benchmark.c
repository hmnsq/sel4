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
se_L4_کلمه ks_Log_شاخص = 0;
se_L4_کلمه ks_Log_شاخص_Finalized = 0;
#د_که_پای /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#که_تعریف_شوی CONFIG_ARM_ENABLE_PMU_ډکېدنه_مداخله
پورته_STATE_DEFINE(uint64_t, ccnt_عدد_overflows);
#د_که_پای /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#که_تعریف_شوی CONFIG_ENABLE_BENCHMARKS
فراغ arm_init_ccnt(فراغ)
{

    uint32_t val = (بټ(PMCR_ENABLE) | بټ(PMCR_CCNT_RESET) | بټ(PMCR_ECNT_RESET));
    SYSTEM_WRITE_کلمه(PMCR, val);

#که_تعریف_شوی PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_کلمه(PMCNTENSET, بټ(CCNT_شاخص));
#د_که_پای

#که_تعریف_شوی CONFIG_ARM_ENABLE_PMU_ډکېدنه_مداخله
    armv_enable_ډکېدنه_IRQ();
#د_که_پای /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#د_که_پای
