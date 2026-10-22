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
    tcb_t *thread = गाँठो_STATE(ksCurThread);
    यदि (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, सन्देश_Info_Register));
    } अन्यथा {
        arch_clean_invalidate_caches();
    }
#अन्यथा
    arch_clean_invalidate_caches();
#endif
    फर्काऊ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(शून्य)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    यदि (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(गाँठो_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    गाँठो_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(गाँठो_STATE(ksIdleThread));
    गाँठो_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    गाँठो_STATE(ksCurThread)->benchmark.सङ्ख्या_schedules++;
    गाँठो_STATE(benchmark_start_time) = ksEnter;
    गाँठो_STATE(benchmark_kernel_time) = 0;
    गाँठो_STATE(benchmark_kernel_सङ्ख्या_entries) = 0;
    गाँठो_STATE(benchmark_kernel_सङ्ख्या_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(गाँठो_STATE(ksCurThread), capRegister, seL4_NoError);
    फर्काऊ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(शून्य)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(गाँठो_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    फर्काऊ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(शून्य)
{
    word_t cptr_userFrame = getRegister(गाँठो_STATE(ksCurThread), capRegister);
    यदि (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(गाँठो_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(गाँठो_STATE(ksCurThread), capRegister, seL4_NoError);
    फर्काऊ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(शून्य)
{
    benchmark_track_utilisation_dump();
    फर्काऊ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(शून्य)
{
    word_t tcb_cptr = getRegister(गाँठो_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(गाँठो_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    यदि (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        फर्काऊ EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    फर्काऊ EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(शून्य)
{
    म_लिखो("{\n");
    म_लिखो("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(गाँठो_STATE(benchmark_end_time) - गाँठो_STATE(benchmark_start_time)));
    म_लिखो("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) गाँठो_STATE(benchmark_kernel_time));
    म_लिखो("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) गाँठो_STATE(benchmark_kernel_सङ्ख्या_entries));
    म_लिखो("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) गाँठो_STATE(benchmark_kernel_सङ्ख्या_schedules));
    म_लिखो("  \"BENCHMARK_TCB_\": [\n");
    लागि (tcb_t *curr = गाँठो_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        म_लिखो("    {\n");
        म_लिखो("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        म_लिखो("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        म_लिखो("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.सङ्ख्या_schedules);
        म_लिखो("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        म_लिखो("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.सङ्ख्या_kernel_entries);
        म_लिखो("    }");
        यदि (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != रिक्त) {
            म_लिखो(",\n");
        } अन्यथा {
            म_लिखो("\n");
        }
    }
    म_लिखो("  ]\n}\n");
    फर्काऊ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(शून्य)
{
    लागि (tcb_t *curr = गाँठो_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    फर्काऊ EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
