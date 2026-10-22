/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <model/statedata.h>

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES

timestamp_t ksEnter;
se_L4_మాట ks_Log_సూచీ;
se_L4_మాట ks_Log_సూచీ_Finalized;

శూన్య benchmark_track_exit(శూన్య)
{
    timestamp_t duration = 0;
    timestamp_t ksExit = timestamp();
    benchmark_track_kernel_నమోదు_t *ksLog = (benchmark_track_kernel_నమోదు_t *) KS_LOG_PPTR;

    ఒకవేళ (likely(ks_User_Log_బఫరు != 0)) {
        /* If Log buffer is filled, do nothing */
        ఒకవేళ (likely(ks_Log_సూచీ < MAX_LOG_కొలత)) {
            duration = ksExit - ksEnter;
            ksLog[ks_Log_సూచీ].నమోదు_2 = ks_Kernel_నమోదు;
            ksLog[ks_Log_సూచీ].ప్రారంభం_సమయం = ksEnter;
            ksLog[ks_Log_సూచీ].duration = duration;
            ks_Log_సూచీ++;
        }
    }
}
#అయితే_ముగింపు /* CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES */
