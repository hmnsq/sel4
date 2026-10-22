/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <benchmark/benchmark.h>
#ضمن <arch/benchmark.h>

#إذا CONFIG_MAX_انگ_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_انگ_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_انگ_TRACE_POINTS];
timestamp_t ksExit;
se_L4_لفظ ks_Log_اشاريو = 0;
se_L4_لفظ ks_Log_اشاريو_Finalized = 0;
#جيڪڏهن_پورو /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_ENABLE_PMU_حد_کان_مٿي_مداخلت
مٿي_STATE_DEFINE(uint64_t, ccnt_انگ_overflows);
#جيڪڏهن_پورو /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#جيڪڏهن_وصف_ٿيل CONFIG_ENABLE_BENCHMARKS
فراغ arm_init_ccnt(فراغ)
{

    uint32_t val = (ٻٽو_انگ(PMCR_ENABLE) | ٻٽو_انگ(PMCR_CCNT_RESET) | ٻٽو_انگ(PMCR_ECNT_RESET));
    SYSTEM_WRITE_لفظ(PMCR, val);

#جيڪڏهن_وصف_ٿيل PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_لفظ(PMCNTENSET, ٻٽو_انگ(CCNT_اشاريو));
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_ENABLE_PMU_حد_کان_مٿي_مداخلت
    armv_enable_حد_کان_مٿي_IRQ();
#جيڪڏهن_پورو /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#جيڪڏهن_پورو
