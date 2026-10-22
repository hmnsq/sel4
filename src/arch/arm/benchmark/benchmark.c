/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <benchmark/benchmark.h>
#अन्तर्भाव <arch/benchmark.h>

#यदि CONFIG_MAX_अङ्कः_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_अङ्कः_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_अङ्कः_TRACE_POINTS];
timestamp_t ksExit;
seL4_Word ks_Log_सूचकांक = 0;
seL4_Word ks_Log_सूचकांक_Finalized = 0;
#यदि_अन्तः /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#यदि_परिभाषितम् CONFIG_ARM_ENABLE_PMU_अतिप्रवाहः_व्यवधानम्
ऊर्ध्वम्_STATE_DEFINE(uint64_t, ccnt_अङ्कः_overflows);
#यदि_अन्तः /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#यदि_परिभाषितम् CONFIG_ENABLE_BENCHMARKS
शून्य arm_init_ccnt(शून्य)
{

    uint32_t val = (BIT(PMCR_ENABLE) | BIT(PMCR_CCNT_RESET) | BIT(PMCR_ECNT_RESET));
    SYSTEM_WRITE_WORD(PMCR, val);

#यदि_परिभाषितम् PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_WORD(PMCNTENSET, BIT(CCNT_सूचकांक));
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARM_ENABLE_PMU_अतिप्रवाहः_व्यवधानम्
    armv_enable_अतिप्रवाहः_IRQ();
#यदि_अन्तः /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#यदि_अन्तः
