/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>

#ifdef CONFIG_ENABLE_BENCHMARKS

#include <types.h>
#include <mode/machine.h>
#include <benchmark/benchmark.h>
#include <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(void)
{
#ifdef CONFIG_ARCH_ARM
    tcb_t *thread = बुहोल_STATE(ksCurThread);
    if (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, मेनसाहे_Info_Register));
    } else {
        arch_clean_invalidate_caches();
    }
#else
    arch_clean_invalidate_caches();
#endif
    return EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(void)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    if (ksUserLogBuffer == 0) {
        userError("A user-level buffer has to be set before resetting benchmark.\
                Use seL4_BenchmarkSetLogBuffer\n");
        setRegister(बुहोल_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        return EXCEPTION_SYSCALL_ERROR;
    }

    ksLogIndex = 0;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    बुहोल_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(बुहोल_STATE(ksIdleThread));
    बुहोल_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    बुहोल_STATE(ksCurThread)->benchmark.नुमेरो_schedules++;
    बुहोल_STATE(benchmark_start_time) = ksEnter;
    बुहोल_STATE(benchmark_kernel_time) = 0;
    बुहोल_STATE(benchmark_kernel_नुमेरो_entries) = 0;
    बुहोल_STATE(benchmark_kernel_नुमेरो_schedules) = 1;
    benchmark_arch_utilisation_reset();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(बुहोल_STATE(ksCurThread), capRegister, seL4_NoError);
    return EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(void)
{
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ksLogIndexFinalized = ksLogIndex;
    setRegister(बुहोल_STATE(ksCurThread), capRegister, ksLogIndexFinalized);
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    return EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t handle_SysBenchmarkSetLogBuffer(void)
{
    word_t cptr_userFrame = getRegister(बुहोल_STATE(ksCurThread), capRegister);
    if (benchmark_arch_map_logBuffer(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(बुहोल_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        return EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(बुहोल_STATE(ksCurThread), capRegister, seL4_NoError);
    return EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(void)
{
    benchmark_track_utilisation_dump();
    return EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(void)
{
    word_t tcb_cptr = getRegister(बुहोल_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(बुहोल_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    if (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        return EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    return EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(void)
{
    printf("{\n");
    printf("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(बुहोल_STATE(benchmark_end_time) - बुहोल_STATE(benchmark_start_time)));
    printf("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) बुहोल_STATE(benchmark_kernel_time));
    printf("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) बुहोल_STATE(benchmark_kernel_नुमेरो_entries));
    printf("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) बुहोल_STATE(benchmark_kernel_नुमेरो_schedules));
    printf("  \"BENCHMARK_TCB_\": [\n");
    for (tcb_t *curr = बुहोल_STATE(ksDebugTCBs); curr != NULL; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        printf("    {\n");
        printf("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        printf("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        printf("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.नुमेरो_schedules);
        printf("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        printf("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.नुमेरो_kernel_entries);
        printf("    }");
        if (TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext != NULL) {
            printf(",\n");
        } else {
            printf("\n");
        }
    }
    printf("  ]\n}\n");
    return EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(void)
{
    for (tcb_t *curr = बुहोल_STATE(ksDebugTCBs); curr != NULL; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        benchmark_track_reset_utilisation(curr);
    }
    return EXCEPTION_NONE;
}

#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#endif /* CONFIG_ENABLE_BENCHMARKS */
