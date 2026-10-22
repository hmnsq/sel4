/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#अन्तर्भाव <types.h>
#अन्तर्भाव <mode/machine.h>
#अन्तर्भाव <benchmark/benchmark.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(शून्य)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = ग्रन्थिः_STATE(ksCurThread);
    सचे (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, सन्देशः_Info_Register));
    } अञ्ञथा {
        arch_clean_invalidate_caches();
    }
#अञ्ञथा
    arch_clean_invalidate_caches();
#endif
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(शून्य)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    सचे (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(ग्रन्थिः_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    ग्रन्थिः_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(ग्रन्थिः_STATE(ksIdleThread));
    ग्रन्थिः_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    ग्रन्थिः_STATE(ksCurThread)->benchmark.सङ्ख्या_schedules++;
    ग्रन्थिः_STATE(benchmark_start_time) = ksEnter;
    ग्रन्थिः_STATE(benchmark_kernel_time) = 0;
    ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_entries) = 0;
    ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(ग्रन्थिः_STATE(ksCurThread), capRegister, seL4_NoError);
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(शून्य)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(ग्रन्थिः_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    पटिनिवत्त EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(शून्य)
{
    word_t cptr_userFrame = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);
    सचे (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(ग्रन्थिः_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(ग्रन्थिः_STATE(ksCurThread), capRegister, seL4_NoError);
    पटिनिवत्त EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(शून्य)
{
    benchmark_track_utilisation_dump();
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(शून्य)
{
    word_t tcb_cptr = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(ग्रन्थिः_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    सचे (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        पटिनिवत्त EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    पटिनिवत्त EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(शून्य)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(ग्रन्थिः_STATE(benchmark_end_time) - ग्रन्थिः_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) ग्रन्थिः_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    अत्थाय (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.सङ्ख्या_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.सङ्ख्या_kernel_entries);
        printf("    }");
        सचे (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != रिक्त) {
            printf(",\n");
        } अञ्ञथा {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(शून्य)
{
    अत्थाय (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    पटिनिवत्त EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
