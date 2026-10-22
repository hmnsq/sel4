/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <mode/machine.h>
#অন্তর্ভাৱ <benchmark/benchmark.h>
#অন্তর্ভাৱ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(শূন্য)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = গ্ৰন্থি_STATE(ksCurThread);
    যদি (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, বাৰ্তা_Info_Register));
    } অন্যথা {
        arch_clean_invalidate_caches();
    }
#অন্যথা
    arch_clean_invalidate_caches();
#endif
    ঘূৰাই EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(শূন্য)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    যদি (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(গ্ৰন্থি_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    গ্ৰন্থি_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(গ্ৰন্থি_STATE(ksIdleThread));
    গ্ৰন্থি_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    গ্ৰন্থি_STATE(ksCurThread)->benchmark.সংখ্যা_schedules++;
    গ্ৰন্থি_STATE(benchmark_start_time) = ksEnter;
    গ্ৰন্থি_STATE(benchmark_kernel_time) = 0;
    গ্ৰন্থি_STATE(benchmark_kernel_সংখ্যা_entries) = 0;
    গ্ৰন্থি_STATE(benchmark_kernel_সংখ্যা_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(গ্ৰন্থি_STATE(ksCurThread), capRegister, seL4_NoError);
    ঘূৰাই EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(শূন্য)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(গ্ৰন্থি_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ঘূৰাই EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(শূন্য)
{
    word_t cptr_userFrame = getRegister(গ্ৰন্থি_STATE(ksCurThread), capRegister);
    যদি (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(গ্ৰন্থি_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(গ্ৰন্থি_STATE(ksCurThread), capRegister, seL4_NoError);
    ঘূৰাই EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(শূন্য)
{
    benchmark_track_utilisation_dump();
    ঘূৰাই EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(শূন্য)
{
    word_t tcb_cptr = getRegister(গ্ৰন্থি_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(গ্ৰন্থি_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    যদি (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        ঘূৰাই EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    ঘূৰাই EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(শূন্য)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(গ্ৰন্থি_STATE(benchmark_end_time) - গ্ৰন্থি_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) গ্ৰন্থি_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) গ্ৰন্থি_STATE(benchmark_kernel_সংখ্যা_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) গ্ৰন্থি_STATE(benchmark_kernel_সংখ্যা_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    বাবে (tcb_t *curr = গ্ৰন্থি_STATE(ksDebugTCBs); curr != রিক্ত; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.সংখ্যা_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.সংখ্যা_kernel_entries);
        printf("    }");
        যদি (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != রিক্ত) {
            printf(",\n");
        } অন্যথা {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    ঘূৰাই EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(শূন্য)
{
    বাবে (tcb_t *curr = গ্ৰন্থি_STATE(ksDebugTCBs); curr != রিক্ত; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    ঘূৰাই EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
