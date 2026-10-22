/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <mode/machine.h>
#অন্তর্ভাব <benchmark/benchmark.h>
#অন্তর্ভাব <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(শূন্য)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = গ্রন্থি_STATE(ksCurThread);
    যদি (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, বার্তা_Info_Register));
    } অন্যথা {
        arch_clean_invalidate_caches();
    }
#অন্যথা
    arch_clean_invalidate_caches();
#endif
    ফেরত EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(শূন্য)
{
#ifdef CONFIG_KERNEL_LOG_বাফার
    যদি (ks_User_Log_বাফার == 0) {
        userError("A user-level বাফার has to be set before resetting benchmark.\
                Use se_L4_Benchmark_Set_Log_বাফার\n");
        setRegister(গ্রন্থি_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_সূচক = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    গ্রন্থি_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(গ্রন্থি_STATE(ksIdleThread));
    গ্রন্থি_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    গ্রন্থি_STATE(ksCurThread)->benchmark.সংখ্যা_schedules++;
    গ্রন্থি_STATE(benchmark_start_time) = ksEnter;
    গ্রন্থি_STATE(benchmark_kernel_time) = 0;
    গ্রন্থি_STATE(benchmark_kernel_সংখ্যা_entries) = 0;
    গ্রন্থি_STATE(benchmark_kernel_সংখ্যা_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(গ্রন্থি_STATE(ksCurThread), capRegister, seL4_NoError);
    ফেরত EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(শূন্য)
{
#ifdef CONFIG_KERNEL_LOG_বাফার
    ks_Log_সূচক_Finalized = ks_Log_সূচক;
    setRegister(গ্রন্থি_STATE(ksCurThread), capRegister, ks_Log_সূচক_Finalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ফেরত EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_বাফার
exception_t handle_Sys_Benchmark_Set_Log_বাফার(শূন্য)
{
    word_t cptr_userFrame = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);
    যদি (benchmark_arch_map_log_বাফার(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(গ্রন্থি_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(গ্রন্থি_STATE(ksCurThread), capRegister, seL4_NoError);
    ফেরত EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(শূন্য)
{
    benchmark_track_utilisation_dump();
    ফেরত EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(শূন্য)
{
    word_t tcb_cptr = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(গ্রন্থি_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    যদি (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        ফেরত EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    ফেরত EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(শূন্য)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(গ্রন্থি_STATE(benchmark_end_time) - গ্রন্থি_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) গ্রন্থি_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) গ্রন্থি_STATE(benchmark_kernel_সংখ্যা_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) গ্রন্থি_STATE(benchmark_kernel_সংখ্যা_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    জন্য (tcb_t *curr = গ্রন্থি_STATE(ksDebugTCBs); curr != ফাঁকা; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_পরবর্তী) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.সংখ্যা_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.সংখ্যা_kernel_entries);
        printf("    }");
        যদি (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_পরবর্তী != ফাঁকা) {
            printf(",\n");
        } অন্যথা {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    ফেরত EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(শূন্য)
{
    জন্য (tcb_t *curr = গ্রন্থি_STATE(ksDebugTCBs); curr != ফাঁকা; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_পরবর্তী) {
        benchmark_track_reset_utilisation(curr);
    }
    ফেরত EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
