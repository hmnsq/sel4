/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#شامل <types.h>
#شامل <mode/machine.h>
#شامل <benchmark/benchmark.h>
#شامل <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(تهی)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = گره_STATE(ksCurThread);
    اگر (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, پیام_Info_Register));
    } وگرنه {
        arch_clean_invalidate_caches();
    }
#وگرنه
    arch_clean_invalidate_caches();
#endif
    بازگشت EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(تهی)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    اگر (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(گره_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    گره_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(گره_STATE(ksIdleThread));
    گره_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    گره_STATE(ksCurThread)->benchmark.عدد_schedules++;
    گره_STATE(benchmark_start_time) = ksEnter;
    گره_STATE(benchmark_kernel_time) = 0;
    گره_STATE(benchmark_kernel_عدد_entries) = 0;
    گره_STATE(benchmark_kernel_عدد_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(گره_STATE(ksCurThread), capRegister, seL4_NoError);
    بازگشت EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(تهی)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(گره_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    بازگشت EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(تهی)
{
    word_t cptr_userFrame = getRegister(گره_STATE(ksCurThread), capRegister);
    اگر (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(گره_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(گره_STATE(ksCurThread), capRegister, seL4_NoError);
    بازگشت EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(تهی)
{
    benchmark_track_utilisation_dump();
    بازگشت EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(تهی)
{
    word_t tcb_cptr = getRegister(گره_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(گره_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    اگر (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        بازگشت EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    بازگشت EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(تهی)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(گره_STATE(benchmark_end_time) - گره_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) گره_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) گره_STATE(benchmark_kernel_عدد_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) گره_STATE(benchmark_kernel_عدد_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    برای (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != پوچ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.عدد_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.عدد_kernel_entries);
        printf("    }");
        اگر (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != پوچ) {
            printf(",\n");
        } وگرنه {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    بازگشت EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(تهی)
{
    برای (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != پوچ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    بازگشت EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
