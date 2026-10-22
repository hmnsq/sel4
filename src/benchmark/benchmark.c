/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ENABLE_BENCHMARKS

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <mode/machine.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_ARM
    tcb_t *thread = ಗಂಟು_STATE(ksCurThread);
    ಆದರೆ (get_ರಿಜಿಸ್ಟರ್(thread, cap_ರಿಜಿಸ್ಟರ್)) {
        arch_clean_invalidate_L1_caches(get_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್));
    } ಇಲ್ಲದಿದ್ದರೆ {
        arch_clean_invalidate_caches();
    }
#ಇಲ್ಲದಿದ್ದರೆ
    arch_clean_invalidate_caches();
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
    ಆದರೆ (ks_User_Log_ಬಫರ್ == 0) {
        userError("A user-level ಬಫರ್ has ಗಮ್ಯ be ಗಣ before resetting benchmark.\
                Use se_L4_Benchmark_ಗಣ_Log_ಬಫರ್\n");
        ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್, seL4_IllegalOperation);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_ಸೂಚ್ಯಂಕ = 0;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_UTILISATION
    ಗಂಟು_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(ಗಂಟು_STATE(ksIdleThread));
    ಗಂಟು_STATE(ksCurThread)->benchmark.ವೇಳಾಪಟ್ಟಿ_ಆರಂಭ_ಸಮಯ = ksEnter;
    ಗಂಟು_STATE(ksCurThread)->benchmark.ಸಂಖ್ಯೆ_schedules++;
    ಗಂಟು_STATE(benchmark_ಆರಂಭ_ಸಮಯ) = ksEnter;
    ಗಂಟು_STATE(benchmark_kernel_ಸಮಯ) = 0;
    ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_entries) = 0;
    ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_schedules) = 1;
    benchmark_arch_utilisation_reset();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್, seL4_NoError);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
    ks_Log_ಸೂಚ್ಯಂಕ_Finalized = ks_Log_ಸೂಚ್ಯಂಕ;
    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್, ks_Log_ಸೂಚ್ಯಂಕ_Finalized);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
exception_t handle_Sys_Benchmark_ಗಣ_Log_ಬಫರ್(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t cptr_user_ಚೌಕಟ್ಟು = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
    ಆದರೆ (benchmark_arch_map_log_ಬಫರ್(cptr_user_ಚೌಕಟ್ಟು) != EXCEPTION_NONE) {
        ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್, seL4_IllegalOperation);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್, seL4_NoError);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(ಶೂನ್ಯ)
{
    benchmark_track_utilisation_dump();
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t tcb_cptr = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
    lookupCap_ret_t lu_ret;
    ಶಬ್ದ_t cap_type;

    lu_ret = lookupCap(ಗಂಟು_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ಆದರೆ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_ಎಳೆಗಳು_Utilisation(ಶೂನ್ಯ)
{
    ಬರೆ("{\n");
    ಬರೆ("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (ಶಬ್ದ_t)(ಗಂಟು_STATE(benchmark_ಕೊನೆ_ಸಮಯ) - ಗಂಟು_STATE(benchmark_ಆರಂಭ_ಸಮಯ)));
    ಬರೆ("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (ಶಬ್ದ_t) ಗಂಟು_STATE(benchmark_kernel_ಸಮಯ));
    ಬರೆ("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (ಶಬ್ದ_t) ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_entries));
    ಬರೆ("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (ಶಬ್ದ_t) ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_schedules));
    ಬರೆ("  \"BENCHMARK_TCB_\": [\n");
    ಗಾಗಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ಖಾಲಿ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ಮುಂದಿನ) {
        ಬರೆ("    {\n");
        ಬರೆ("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        ಬರೆ("      \"UTILISATION\":%lu,\n", (ಶಬ್ದ_t) curr->benchmark.utilisation);
        ಬರೆ("      \"NUMBER_SCHEDULES\":%lu,\n", (ಶಬ್ದ_t) curr->benchmark.ಸಂಖ್ಯೆ_schedules);
        ಬರೆ("      \"KERNEL_UTILISATION\":%lu,\n", (ಶಬ್ದ_t) curr->benchmark.kernel_utilisation);
        ಬರೆ("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (ಶಬ್ದ_t) curr->benchmark.ಸಂಖ್ಯೆ_kernel_entries);
        ಬರೆ("    }");
        ಆದರೆ (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ಮುಂದಿನ != ಖಾಲಿ) {
            ಬರೆ(",\n");
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("\n");
        }
    }
    ಬರೆ("  ]\n}\n");
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_ಎಳೆಗಳು_Utilisation(ಶೂನ್ಯ)
{
    ಗಾಗಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ಖಾಲಿ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ಮುಂದಿನ) {
        benchmark_track_reset_utilisation(curr);
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ENABLE_BENCHMARKS */
