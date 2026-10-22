/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

ಶೂನ್ಯ benchmark_track_utilisation_dump(ಶೂನ್ಯ)
{
    uint64_t *buffer = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, ಗಂಟು_STATE(ksCurThread)))->ಸಂದೇಶ[0]));
    tcb_t *tcb = ರಿಕ್ತ;
    word_t tcb_cptr = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(ಗಂಟು_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ಯದಿ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        ಪ್ರತ್ಯಾಗಮ;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    buffer[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    buffer[BENCHMARK_TCB_ಸಂಖ್ಯೆ_SCHEDULES] = tcb->benchmark.ಸಂಖ್ಯೆ_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_TCB_ಸಂಖ್ಯೆ_KERNEL_ENTRIES] = tcb->benchmark.ಸಂಖ್ಯೆ_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = ಗಂಟು_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#ifdef ENABLE_SMP_SUPPORT
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = ಗಂಟು_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#ಅನ್ಯಥಾ
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#endif

    buffer[BENCHMARK_IDLE_ಸಂಖ್ಯೆ_SCHEDULES] = ಗಂಟು_STATE(
                                                  ksIdleThread)->benchmark.ಸಂಖ್ಯೆ_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_IDLE_KERNEL_UTILISATION] = ಗಂಟು_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_IDLE_ಸಂಖ್ಯೆ_KERNEL_ENTRIES] = ಗಂಟು_STATE(
                                                       ksIdleThread)->benchmark.ಸಂಖ್ಯೆ_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    buffer[BENCHMARK_TOTAL_UTILISATION] =
        (ARCH_ಗಂಟು_STATE(ccnt_ಅಂಕೆ_overflows) * 0xFFFFFFFFU) + ಗಂಟು_STATE(benchmark_end_time) - ಗಂಟು_STATE(benchmark_start_time);
#ಅನ್ಯಥಾ
    buffer[BENCHMARK_TOTAL_UTILISATION] = ಗಂಟು_STATE(benchmark_end_time) - ಗಂಟು_STATE(
                                              benchmark_start_time); /* Overall time */
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    buffer[BENCHMARK_TOTAL_ಸಂಖ್ಯೆ_SCHEDULES] = ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_schedules);
    buffer[BENCHMARK_TOTAL_KERNEL_UTILISATION] = ಗಂಟು_STATE(benchmark_kernel_time);
    buffer[BENCHMARK_TOTAL_ಸಂಖ್ಯೆ_KERNEL_ENTRIES] = ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_entries);

}

ಶೂನ್ಯ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.ಸಂಖ್ಯೆ_schedules = 0;
    tcb->benchmark.ಸಂಖ್ಯೆ_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.schedule_start_time = 0;
}
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
