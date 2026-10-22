/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <mode/machine.h>
#അന്തര്ഭാവ <benchmark/benchmark.h>
#അന്തര്ഭാവ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(ശൂന്യം)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = കണ്ണി_STATE(ksCurThread);
    എങ്കിൽ (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, സന്ദേശം_Info_Register));
    } അല്ലെങ്കിൽ {
        arch_clean_invalidate_caches();
    }
#അല്ലെങ്കിൽ
    arch_clean_invalidate_caches();
#endif
    തിരികെ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(ശൂന്യം)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    എങ്കിൽ (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(കണ്ണി_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    കണ്ണി_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(കണ്ണി_STATE(ksIdleThread));
    കണ്ണി_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    കണ്ണി_STATE(ksCurThread)->benchmark.സംഖ്യ_schedules++;
    കണ്ണി_STATE(benchmark_start_time) = ksEnter;
    കണ്ണി_STATE(benchmark_kernel_time) = 0;
    കണ്ണി_STATE(benchmark_kernel_സംഖ്യ_entries) = 0;
    കണ്ണി_STATE(benchmark_kernel_സംഖ്യ_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(കണ്ണി_STATE(ksCurThread), capRegister, seL4_NoError);
    തിരികെ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(ശൂന്യം)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(കണ്ണി_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    തിരികെ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(ശൂന്യം)
{
    word_t cptr_userFrame = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
    എങ്കിൽ (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(കണ്ണി_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(കണ്ണി_STATE(ksCurThread), capRegister, seL4_NoError);
    തിരികെ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(ശൂന്യം)
{
    benchmark_track_utilisation_dump();
    തിരികെ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(ശൂന്യം)
{
    word_t tcb_cptr = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(കണ്ണി_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    എങ്കിൽ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        തിരികെ EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    തിരികെ EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(ശൂന്യം)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(കണ്ണി_STATE(benchmark_end_time) - കണ്ണി_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) കണ്ണി_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) കണ്ണി_STATE(benchmark_kernel_സംഖ്യ_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) കണ്ണി_STATE(benchmark_kernel_സംഖ്യ_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    വേണ്ടി (tcb_t *curr = കണ്ണി_STATE(ksDebugTCBs); curr != ശൂന്യ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.സംഖ്യ_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.സംഖ്യ_kernel_entries);
        printf("    }");
        എങ്കിൽ (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != ശൂന്യ) {
            printf(",\n");
        } അല്ലെങ്കിൽ {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    തിരികെ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(ശൂന്യം)
{
    വേണ്ടി (tcb_t *curr = കണ്ണി_STATE(ksDebugTCBs); curr != ശൂന്യ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    തിരികെ EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
