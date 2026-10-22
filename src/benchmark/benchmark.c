/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <mode/machine.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(ಶೂನ್ಯ)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = ಗಂಟು_STATE(ksCurThread);
    ಯದಿ (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, ಸಂದೇಶ_Info_Register));
    } ಅನ್ಯಥಾ {
        arch_clean_invalidate_caches();
    }
#ಅನ್ಯಥಾ
    arch_clean_invalidate_caches();
#endif
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(ಶೂನ್ಯ)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ಯದಿ (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(ಗಂಟು_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    ಗಂಟು_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(ಗಂಟು_STATE(ksIdleThread));
    ಗಂಟು_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    ಗಂಟು_STATE(ksCurThread)->benchmark.ಸಂಖ್ಯೆ_schedules++;
    ಗಂಟು_STATE(benchmark_start_time) = ksEnter;
    ಗಂಟು_STATE(benchmark_kernel_time) = 0;
    ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_entries) = 0;
    ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(ಗಂಟು_STATE(ksCurThread), capRegister, seL4_NoError);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(ಶೂನ್ಯ)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(ಗಂಟು_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(ಶೂನ್ಯ)
{
    word_t cptr_userFrame = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
    ಯದಿ (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(ಗಂಟು_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(ಗಂಟು_STATE(ksCurThread), capRegister, seL4_NoError);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(ಶೂನ್ಯ)
{
    benchmark_track_utilisation_dump();
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(ಶೂನ್ಯ)
{
    word_t tcb_cptr = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(ಗಂಟು_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ಯದಿ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(ಶೂನ್ಯ)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(ಗಂಟು_STATE(benchmark_end_time) - ಗಂಟು_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) ಗಂಟು_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    ಪ್ರತಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ರಿಕ್ತ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.ಸಂಖ್ಯೆ_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.ಸಂಖ್ಯೆ_kernel_entries);
        printf("    }");
        ಯದಿ (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != ರಿಕ್ತ) {
            printf(",\n");
        } ಅನ್ಯಥಾ {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(ಶೂನ್ಯ)
{
    ಪ್ರತಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ರಿಕ್ತ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
