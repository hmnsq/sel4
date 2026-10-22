/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/benchmark.h>

#ਜੇ CONFIG_MAX_ਅੰਕ_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_ਅੰਕ_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_ਅੰਕ_TRACE_POINTS];
timestamp_t ksExit;
se_L4_ਸ਼ਬਦ ks_Log_ਸੂਚਕਾਂਕ = 0;
se_L4_ਸ਼ਬਦ ks_Log_ਸੂਚਕਾਂਕ_Finalized = 0;
#ਜੇ_ਅੰਤ /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_ENABLE_PMU_ਉਛਾਲ_ਰੁਕਾਵਟ
ਉੱਪਰ_STATE_DEFINE(uint64_t, ccnt_ਅੰਕ_overflows);
#ਜੇ_ਅੰਤ /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_BENCHMARKS
ਖਾਲੀ arm_init_ccnt(ਖਾਲੀ)
{

    uint32_t val = (ਦੋਆਧਾਰੀ_ਅੰਕ(PMCR_ENABLE) | ਦੋਆਧਾਰੀ_ਅੰਕ(PMCR_CCNT_RESET) | ਦੋਆਧਾਰੀ_ਅੰਕ(PMCR_ECNT_RESET));
    SYSTEM_WRITE_ਸ਼ਬਦ(PMCR, val);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_ਸ਼ਬਦ(PMCNTENSET, ਦੋਆਧਾਰੀ_ਅੰਕ(CCNT_ਸੂਚਕਾਂਕ));
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_ENABLE_PMU_ਉਛਾਲ_ਰੁਕਾਵਟ
    armv_enable_ਉਛਾਲ_IRQ();
#ਜੇ_ਅੰਤ /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#ਜੇ_ਅੰਤ
