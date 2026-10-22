/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <benchmark/benchmark.h>
#అన్తర్భావ <arch/benchmark.h>

#ఒకవేళ CONFIG_MAX_అంకె_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_అంకె_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_అంకె_TRACE_POINTS];
timestamp_t ksExit;
se_L4_మాట ks_Log_సూచీ = 0;
se_L4_మాట ks_Log_సూచీ_Finalized = 0;
#అయితే_ముగింపు /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#నిర్వచితమైతే CONFIG_ARM_ENABLE_PMU_అధికప్రవాహం_అంతరాయం
పైకి_STATE_DEFINE(uint64_t, ccnt_అంకె_overflows);
#అయితే_ముగింపు /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#నిర్వచితమైతే CONFIG_ENABLE_BENCHMARKS
శూన్య arm_init_ccnt(శూన్య)
{

    uint32_t val = (బిట్టు(PMCR_ENABLE) | బిట్టు(PMCR_CCNT_RESET) | బిట్టు(PMCR_ECNT_RESET));
    SYSTEM_WRITE_మాట(PMCR, val);

#నిర్వచితమైతే PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_మాట(PMCNTENSET, బిట్టు(CCNT_సూచీ));
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARM_ENABLE_PMU_అధికప్రవాహం_అంతరాయం
    armv_enable_అధికప్రవాహం_IRQ();
#అయితే_ముగింపు /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#అయితే_ముగింపు
