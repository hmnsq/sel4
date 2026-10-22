/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <mode/machine.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(ଶୂନ୍ୟ)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = ଗଣ୍ଠି_STATE(ksCurThread);
    ଯଦି (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, ସନ୍ଦେଶ_Info_Register));
    } ଅନ୍ୟଥା {
        arch_clean_invalidate_caches();
    }
#ଅନ୍ୟଥା
    arch_clean_invalidate_caches();
#endif
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(ଶୂନ୍ୟ)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ଯଦି (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    ଗଣ୍ଠି_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(ଗଣ୍ଠି_STATE(ksIdleThread));
    ଗଣ୍ଠି_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    ଗଣ୍ଠି_STATE(ksCurThread)->benchmark.ସଂଖ୍ୟା_schedules++;
    ଗଣ୍ଠି_STATE(benchmark_start_time) = ksEnter;
    ଗଣ୍ଠି_STATE(benchmark_kernel_time) = 0;
    ଗଣ୍ଠି_STATE(benchmark_kernel_ସଂଖ୍ୟା_entries) = 0;
    ଗଣ୍ଠି_STATE(benchmark_kernel_ସଂଖ୍ୟା_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister, seL4_NoError);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(ଶୂନ୍ୟ)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(ଶୂନ୍ୟ)
{
    word_t cptr_userFrame = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
    ଯଦି (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister, seL4_NoError);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(ଶୂନ୍ୟ)
{
    benchmark_track_utilisation_dump();
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(ଶୂନ୍ୟ)
{
    word_t tcb_cptr = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(ଗଣ୍ଠି_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ଯଦି (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(ଶୂନ୍ୟ)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(ଗଣ୍ଠି_STATE(benchmark_end_time) - ଗଣ୍ଠି_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) ଗଣ୍ଠି_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) ଗଣ୍ଠି_STATE(benchmark_kernel_ସଂଖ୍ୟା_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) ଗଣ୍ଠି_STATE(benchmark_kernel_ସଂଖ୍ୟା_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    ପାଇଁ (tcb_t *curr = ଗଣ୍ଠି_STATE(ksDebugTCBs); curr != ଖାଲି; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.ସଂଖ୍ୟା_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.ସଂଖ୍ୟା_kernel_entries);
        printf("    }");
        ଯଦି (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != ଖାଲି) {
            printf(",\n");
        } ଅନ୍ୟଥା {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(ଶୂନ୍ୟ)
{
    ପାଇଁ (tcb_t *curr = ଗଣ୍ଠି_STATE(ksDebugTCBs); curr != ଖାଲି; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
