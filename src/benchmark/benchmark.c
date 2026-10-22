/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#ضمن <types.h>
#ضمن <mode/machine.h>
#ضمن <benchmark/benchmark.h>
#ضمن <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(فراغ)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = NODE_STATE(ksCurThread);
    ܐܢ (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, msgInfoRegister));
    } ܐܚܪܢܐ {
        arch_clean_invalidate_caches();
    }
#ܐܚܪܢܐ
    arch_clean_invalidate_caches();
#endif
    ܦܢܐ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(فراغ)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ܐܢ (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(NODE_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ܦܢܐ EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    NODE_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(NODE_STATE(ksIdleThread));
    NODE_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    NODE_STATE(ksCurThread)->benchmark.number_schedules++;
    NODE_STATE(benchmark_start_time) = ksEnter;
    NODE_STATE(benchmark_kernel_time) = 0;
    NODE_STATE(benchmark_kernel_number_entries) = 0;
    NODE_STATE(benchmark_kernel_number_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(NODE_STATE(ksCurThread), capRegister, seL4_NoError);
    ܦܢܐ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(فراغ)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(NODE_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ܦܢܐ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(فراغ)
{
    word_t cptr_userFrame = getRegister(NODE_STATE(ksCurThread), capRegister);
    ܐܢ (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(NODE_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ܦܢܐ EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(NODE_STATE(ksCurThread), capRegister, seL4_NoError);
    ܦܢܐ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(فراغ)
{
    benchmark_track_utilisation_dump();
    ܦܢܐ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(فراغ)
{
    word_t tcb_cptr = getRegister(NODE_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(NODE_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ܐܢ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        ܦܢܐ EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    ܦܢܐ EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(فراغ)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(NODE_STATE(benchmark_end_time) - NODE_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) NODE_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) NODE_STATE(benchmark_kernel_number_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) NODE_STATE(benchmark_kernel_number_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    لكل (tcb_t *curr = NODE_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.number_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.number_kernel_entries);
        printf("    }");
        ܐܢ (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != عدم) {
            printf(",\n");
        } ܐܚܪܢܐ {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    ܦܢܐ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(فراغ)
{
    لكل (tcb_t *curr = NODE_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    ܦܢܐ EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
