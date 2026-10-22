/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
se_L4_ಶಬ್ದ ks_Log_ಸೂಚ್ಯಂಕ;
se_L4_ಶಬ್ದ ks_Log_ಸೂಚ್ಯಂಕ_Finalized;

ಶೂನ್ಯ benchmark_track_exit(ಶೂನ್ಯ)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_ನಮೂದು_t *ksLog = (benchmark_track_kernel_ನಮೂದು_t *) KS_LOG_PPTR;

    ಆದರೆ (likely(ks_User_Log_ಬಫರ್ != 0)) {
        /* If Log buffer is filled, do nothing */
        ಆದರೆ (likely(ks_Log_ಸೂಚ್ಯಂಕ < MAX_LOG_ಗಾತ್ರ)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_ಸೂಚ್ಯಂಕ].ನಮೂದು = ks_Kernel_ನಮೂದು;
            ksLog[ks_Log_ಸೂಚ್ಯಂಕ].ಆರಂಭ_ಸಮಯ = ksEnter;
            ksLog[ks_Log_ಸೂಚ್ಯಂಕ].duration = duration;
            ks_Log_ಸೂಚ್ಯಂಕ++;
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
