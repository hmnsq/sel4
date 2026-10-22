/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <benchmark/benchmark.h>
#অন্তর্ভাব <arch/benchmark.h>

#যদি CONFIG_MAX_অঙ্ক_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_অঙ্ক_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_অঙ্ক_TRACE_POINTS];
timestamp_t ksExit;
seL4_Word ksLogIndex = 0;
seL4_Word ksLogIndexFinalized = 0;
#endif /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
UP_STATE_DEFINE(uint64_t, ccnt_অঙ্ক_overflows);
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#ifdef CONFIG_ENABLE_BENCHMARKS
শূন্য arm_init_ccnt(শূন্য)
{

    uint32_t val = (BIT(PMCR_ENABLE) | BIT(PMCR_CCNT_RESET) | BIT(PMCR_ECNT_RESET));
    SYSTEM_WRITE_WORD(PMCR, val);

#ifdef PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_WORD(PMCNTENSET, BIT(CCNT_INDEX));
#endif

#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    armv_enableOverflowIRQ();
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#endif
