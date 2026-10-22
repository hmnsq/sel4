/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <benchmark/benchmark_utilisation.h>

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

শূন্য benchmark_track_utilisation_dump(শূন্য)
{
    uint64_t *buffer = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, গ্ৰন্থি_STATE(ksCurThread)))->বাৰ্তা[0]));
    tcb_t *tcb = রিক্ত;
    word_t tcb_cptr = getRegister(গ্ৰন্থি_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(গ্ৰন্থি_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    যদি (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        ঘূৰাই;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    buffer[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    buffer[BENCHMARK_TCB_সংখ্যা_SCHEDULES] = tcb->benchmark.সংখ্যা_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_TCB_সংখ্যা_KERNEL_ENTRIES] = tcb->benchmark.সংখ্যা_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = গ্ৰন্থি_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#ifdef ENABLE_SMP_SUPPORT
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = গ্ৰন্থি_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#অন্যথা
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#endif

    buffer[BENCHMARK_IDLE_সংখ্যা_SCHEDULES] = গ্ৰন্থি_STATE(
                                                  ksIdleThread)->benchmark.সংখ্যা_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_IDLE_KERNEL_UTILISATION] = গ্ৰন্থি_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_IDLE_সংখ্যা_KERNEL_ENTRIES] = গ্ৰন্থি_STATE(
                                                       ksIdleThread)->benchmark.সংখ্যা_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    buffer[BENCHMARK_TOTAL_UTILISATION] =
        (ARCH_গ্ৰন্থি_STATE(ccnt_অঙ্ক_overflows) * 0xFFFFFFFFU) + গ্ৰন্থি_STATE(benchmark_end_time) - গ্ৰন্থি_STATE(benchmark_start_time);
#অন্যথা
    buffer[BENCHMARK_TOTAL_UTILISATION] = গ্ৰন্থি_STATE(benchmark_end_time) - গ্ৰন্থি_STATE(
                                              benchmark_start_time); /* Overall time */
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    buffer[BENCHMARK_TOTAL_সংখ্যা_SCHEDULES] = গ্ৰন্থি_STATE(benchmark_kernel_সংখ্যা_schedules);
    buffer[BENCHMARK_TOTAL_KERNEL_UTILISATION] = গ্ৰন্থি_STATE(benchmark_kernel_time);
    buffer[BENCHMARK_TOTAL_সংখ্যা_KERNEL_ENTRIES] = গ্ৰন্থি_STATE(benchmark_kernel_সংখ্যা_entries);

}

শূন্য benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.সংখ্যা_schedules = 0;
    tcb->benchmark.সংখ্যা_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.schedule_start_time = 0;
}
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
