/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <benchmark/benchmark.h>
#समावेश <arch/benchmark.h>

#अगर CONFIG_MAX_अंक_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_अंक_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_अंक_TRACE_POINTS];
timestamp_t ksExit;
seL4_Word ks_Log_सूचकांक = 0;
seL4_Word ks_Log_सूचकांक_Finalized = 0;
#पूर्ण_अगर /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#अगर_घोषित CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
UP_STATE_DEFINE(uint64_t, ccnt_अंक_overflows);
#पूर्ण_अगर /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#अगर_घोषित CONFIG_ENABLE_BENCHMARKS
व्योम arm_init_ccnt(व्योम)
{

    uint32_t val = (BIT(PMCR_ENABLE) | BIT(PMCR_CCNT_RESET) | BIT(PMCR_ECNT_RESET));
    SYSTEM_WRITE_WORD(PMCR, val);

#अगर_घोषित PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_WORD(PMCNTENSET, BIT(CCNT_सूचकांक));
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    armv_enableOverflowIRQ();
#पूर्ण_अगर /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#पूर्ण_अगर
