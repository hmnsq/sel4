/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <arch/benchmark.h>

#ಆದರೆ CONFIG_MAX_ಅಂಕೆ_TRACE_POINTS > 0
timestamp_t ksEntries[CONFIG_MAX_ಅಂಕೆ_TRACE_POINTS];
bool_t ksStarted[CONFIG_MAX_ಅಂಕೆ_TRACE_POINTS];
timestamp_t ksExit;
se_L4_ಶಬ್ದ ks_Log_ಸೂಚ್ಯಂಕ = 0;
se_L4_ಶಬ್ದ ks_Log_ಸೂಚ್ಯಂಕ_Finalized = 0;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_MAX_NUM_TRACE_POINTS > 0 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_ENABLE_PMU_ಅತಿಪ್ರವಾಹ_ಅಡಚಣೆ
ಮೇಲೆ_STATE_DEFINE(uint64_t, ccnt_ಅಂಕೆ_overflows);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ENABLE_BENCHMARKS
ಶೂನ್ಯ arm_init_ccnt(ಶೂನ್ಯ)
{

    uint32_t val = (ಬಿಟ್(PMCR_ENABLE) | ಬಿಟ್(PMCR_CCNT_RESET) | ಬಿಟ್(PMCR_ECNT_RESET));
    SYSTEM_WRITE_ಶಬ್ದ(PMCR, val);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PMCNTENSET
    /* turn on the cycle counter */
    SYSTEM_WRITE_ಶಬ್ದ(PMCNTENSET, ಬಿಟ್(CCNT_ಸೂಚ್ಯಂಕ));
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_ENABLE_PMU_ಅತಿಪ್ರವಾಹ_ಅಡಚಣೆ
    armv_enable_ಅತಿಪ್ರವಾಹ_IRQ();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
}
#ಆದರೆ_ಅಂತ್ಯ
