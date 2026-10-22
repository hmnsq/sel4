/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <mode/machine.h>
#அந்தர்பாவ <benchmark/benchmark.h>
#அந்தர்பாவ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(வெறுமை)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = முனை_STATE(ksCurThread);
    எனில் (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, செய்தி_Info_Register));
    } இல்லையெனில் {
        arch_clean_invalidate_caches();
    }
#இல்லையெனில்
    arch_clean_invalidate_caches();
#endif
    திரும்பு EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(வெறுமை)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    எனில் (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(முனை_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    முனை_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(முனை_STATE(ksIdleThread));
    முனை_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    முனை_STATE(ksCurThread)->benchmark.எண்_schedules++;
    முனை_STATE(benchmark_start_time) = ksEnter;
    முனை_STATE(benchmark_kernel_time) = 0;
    முனை_STATE(benchmark_kernel_எண்_entries) = 0;
    முனை_STATE(benchmark_kernel_எண்_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(முனை_STATE(ksCurThread), capRegister, seL4_NoError);
    திரும்பு EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(வெறுமை)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(முனை_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    திரும்பு EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(வெறுமை)
{
    word_t cptr_userFrame = getRegister(முனை_STATE(ksCurThread), capRegister);
    எனில் (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(முனை_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(முனை_STATE(ksCurThread), capRegister, seL4_NoError);
    திரும்பு EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(வெறுமை)
{
    benchmark_track_utilisation_dump();
    திரும்பு EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(வெறுமை)
{
    word_t tcb_cptr = getRegister(முனை_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(முனை_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    எனில் (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        திரும்பு EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    திரும்பு EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(வெறுமை)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(முனை_STATE(benchmark_end_time) - முனை_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) முனை_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) முனை_STATE(benchmark_kernel_எண்_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) முனை_STATE(benchmark_kernel_எண்_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    க்கு (tcb_t *curr = முனை_STATE(ksDebugTCBs); curr != வெற்று; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.எண்_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.எண்_kernel_entries);
        printf("    }");
        எனில் (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != வெற்று) {
            printf(",\n");
        } இல்லையெனில் {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    திரும்பு EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(வெறுமை)
{
    க்கு (tcb_t *curr = முனை_STATE(ksDebugTCBs); curr != வெற்று; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    திரும்பு EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
