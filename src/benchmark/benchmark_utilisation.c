/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <benchmark/benchmark_utilisation.h>

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

ശൂന്യം benchmark_track_utilisation_dump(ശൂന്യം)
{
    uint64_t *buffer = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, കണ്ണി_STATE(ksCurThread)))->സന്ദേശം[0]));
    tcb_t *tcb = ശൂന്യ;
    word_t tcb_cptr = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(കണ്ണി_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    എങ്കിൽ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        തിരികെ;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    buffer[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    buffer[BENCHMARK_TCB_സംഖ്യ_SCHEDULES] = tcb->benchmark.സംഖ്യ_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_TCB_സംഖ്യ_KERNEL_ENTRIES] = tcb->benchmark.സംഖ്യ_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = കണ്ണി_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#ifdef ENABLE_SMP_SUPPORT
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = കണ്ണി_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#അല്ലെങ്കിൽ
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#endif

    buffer[BENCHMARK_IDLE_സംഖ്യ_SCHEDULES] = കണ്ണി_STATE(
                                                  ksIdleThread)->benchmark.സംഖ്യ_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_IDLE_KERNEL_UTILISATION] = കണ്ണി_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_IDLE_സംഖ്യ_KERNEL_ENTRIES] = കണ്ണി_STATE(
                                                       ksIdleThread)->benchmark.സംഖ്യ_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    buffer[BENCHMARK_TOTAL_UTILISATION] =
        (ARCH_കണ്ണി_STATE(ccnt_അക്കം_overflows) * 0xFFFFFFFFU) + കണ്ണി_STATE(benchmark_end_time) - കണ്ണി_STATE(benchmark_start_time);
#അല്ലെങ്കിൽ
    buffer[BENCHMARK_TOTAL_UTILISATION] = കണ്ണി_STATE(benchmark_end_time) - കണ്ണി_STATE(
                                              benchmark_start_time); /* Overall time */
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    buffer[BENCHMARK_TOTAL_സംഖ്യ_SCHEDULES] = കണ്ണി_STATE(benchmark_kernel_സംഖ്യ_schedules);
    buffer[BENCHMARK_TOTAL_KERNEL_UTILISATION] = കണ്ണി_STATE(benchmark_kernel_time);
    buffer[BENCHMARK_TOTAL_സംഖ്യ_KERNEL_ENTRIES] = കണ്ണി_STATE(benchmark_kernel_സംഖ്യ_entries);

}

ശൂന്യം benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.സംഖ്യ_schedules = 0;
    tcb->benchmark.സംഖ്യ_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.schedule_start_time = 0;
}
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
