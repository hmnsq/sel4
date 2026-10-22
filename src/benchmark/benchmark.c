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
    tcb_t *thread = गाँठ_STATE(ksCurThread);
    यदि (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, संदेश_Info_Register));
    } अन्यथा {
        arch_clean_invalidate_caches();
    }
#अन्यथा
    arch_clean_invalidate_caches();
#endif
    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(शून्य)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    यदि (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(गाँठ_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    गाँठ_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(गाँठ_STATE(ksIdleThread));
    गाँठ_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    गाँठ_STATE(ksCurThread)->benchmark.संख्या_schedules++;
    गाँठ_STATE(benchmark_start_time) = ksEnter;
    गाँठ_STATE(benchmark_kernel_time) = 0;
    गाँठ_STATE(benchmark_kernel_संख्या_entries) = 0;
    गाँठ_STATE(benchmark_kernel_संख्या_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(गाँठ_STATE(ksCurThread), capRegister, seL4_NoError);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(शून्य)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(गाँठ_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    प्रत्यागम EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(शून्य)
{
    word_t cptr_userFrame = getRegister(गाँठ_STATE(ksCurThread), capRegister);
    यदि (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(गाँठ_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(गाँठ_STATE(ksCurThread), capRegister, seL4_NoError);
    प्रत्यागम EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(शून्य)
{
    benchmark_track_utilisation_dump();
    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(शून्य)
{
    word_t tcb_cptr = getRegister(गाँठ_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(गाँठ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    यदि (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        प्रत्यागम EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    प्रत्यागम EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(शून्य)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(गाँठ_STATE(benchmark_end_time) - गाँठ_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) गाँठ_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) गाँठ_STATE(benchmark_kernel_संख्या_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) गाँठ_STATE(benchmark_kernel_संख्या_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    प्रति (tcb_t *curr = गाँठ_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.संख्या_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.संख्या_kernel_entries);
        printf("    }");
        यदि (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != रिक्त) {
            printf(",\n");
        } अन्यथा {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(शून्य)
{
    प्रति (tcb_t *curr = गाँठ_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    प्रत्यागम EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
