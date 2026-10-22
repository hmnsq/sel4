/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <benchmark/benchmark_utilisation.h>

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

فراغ benchmark_track_utilisation_dump(فراغ)
{
    uint64_t *buffer = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, تۈگۈن_STATE(ksCurThread)))->ئۇچۇر[0]));
    tcb_t *tcb = عدم;
    word_t tcb_cptr = getRegister(تۈگۈن_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(تۈگۈن_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    إذا (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        أرجع;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    buffer[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    buffer[BENCHMARK_TCB_سان_SCHEDULES] = tcb->benchmark.سان_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_TCB_سان_KERNEL_ENTRIES] = tcb->benchmark.سان_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = تۈگۈن_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#ifdef ENABLE_SMP_SUPPORT
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = تۈگۈن_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#وإلا
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#endif

    buffer[BENCHMARK_IDLE_سان_SCHEDULES] = تۈگۈن_STATE(
                                                  ksIdleThread)->benchmark.سان_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_IDLE_KERNEL_UTILISATION] = تۈگۈن_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_IDLE_سان_KERNEL_ENTRIES] = تۈگۈن_STATE(
                                                       ksIdleThread)->benchmark.سان_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    buffer[BENCHMARK_TOTAL_UTILISATION] =
        (ARCH_تۈگۈن_STATE(ccnt_رەقەم_overflows) * 0xFFFFFFFFU) + تۈگۈن_STATE(benchmark_end_time) - تۈگۈن_STATE(benchmark_start_time);
#وإلا
    buffer[BENCHMARK_TOTAL_UTILISATION] = تۈگۈن_STATE(benchmark_end_time) - تۈگۈن_STATE(
                                              benchmark_start_time); /* Overall time */
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    buffer[BENCHMARK_TOTAL_سان_SCHEDULES] = تۈگۈن_STATE(benchmark_kernel_سان_schedules);
    buffer[BENCHMARK_TOTAL_KERNEL_UTILISATION] = تۈگۈن_STATE(benchmark_kernel_time);
    buffer[BENCHMARK_TOTAL_سان_KERNEL_ENTRIES] = تۈگۈن_STATE(benchmark_kernel_سان_entries);

}

فراغ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.سان_schedules = 0;
    tcb->benchmark.سان_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.schedule_start_time = 0;
}
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
