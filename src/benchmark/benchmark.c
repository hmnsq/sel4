/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <mode/machine.h>
#અન્તર્ભાવ <benchmark/benchmark.h>
#અન્તર્ભાવ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(શૂન્ય)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = ગાંઠ_STATE(ksCurThread);
    જો (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, સંદેશ_Info_Register));
    } નહીંતર {
        arch_clean_invalidate_caches();
    }
#નહીંતર
    arch_clean_invalidate_caches();
#endif
    પાછા EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(શૂન્ય)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    જો (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(ગાંઠ_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    ગાંઠ_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(ગાંઠ_STATE(ksIdleThread));
    ગાંઠ_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    ગાંઠ_STATE(ksCurThread)->benchmark.સંખ્યા_schedules++;
    ગાંઠ_STATE(benchmark_start_time) = ksEnter;
    ગાંઠ_STATE(benchmark_kernel_time) = 0;
    ગાંઠ_STATE(benchmark_kernel_સંખ્યા_entries) = 0;
    ગાંઠ_STATE(benchmark_kernel_સંખ્યા_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(ગાંઠ_STATE(ksCurThread), capRegister, seL4_NoError);
    પાછા EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(શૂન્ય)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(ગાંઠ_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(શૂન્ય)
{
    word_t cptr_userFrame = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);
    જો (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(ગાંઠ_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(ગાંઠ_STATE(ksCurThread), capRegister, seL4_NoError);
    પાછા EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(શૂન્ય)
{
    benchmark_track_utilisation_dump();
    પાછા EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(શૂન્ય)
{
    word_t tcb_cptr = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(ગાંઠ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    જો (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        પાછા EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(શૂન્ય)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(ગાંઠ_STATE(benchmark_end_time) - ગાંઠ_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) ગાંઠ_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) ગાંઠ_STATE(benchmark_kernel_સંખ્યા_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) ગાંઠ_STATE(benchmark_kernel_સંખ્યા_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    માટે (tcb_t *curr = ગાંઠ_STATE(ksDebugTCBs); curr != ખાલી; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.સંખ્યા_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.સંખ્યા_kernel_entries);
        printf("    }");
        જો (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != ખાલી) {
            printf(",\n");
        } નહીંતર {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    પાછા EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(શૂન્ય)
{
    માટે (tcb_t *curr = ગાંઠ_STATE(ksDebugTCBs); curr != ખાલી; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    પાછા EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
