/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_utilisation.h>

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

ଶୂନ୍ୟ benchmark_track_utilisation_dump(ଶୂନ୍ୟ)
{
    uint64_t *buffer = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, ଗଣ୍ଠି_STATE(ksCurThread)))->ସନ୍ଦେଶ[0]));
    tcb_t *tcb = ଖାଲି;
    word_t tcb_cptr = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(ଗଣ୍ଠି_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ଯଦି (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        ଫେରନ୍ତୁ;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    buffer[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    buffer[BENCHMARK_TCB_ସଂଖ୍ୟା_SCHEDULES] = tcb->benchmark.ସଂଖ୍ୟା_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_TCB_ସଂଖ୍ୟା_KERNEL_ENTRIES] = tcb->benchmark.ସଂଖ୍ୟା_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = ଗଣ୍ଠି_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#ifdef ENABLE_SMP_SUPPORT
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = ଗଣ୍ଠି_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#ଅନ୍ୟଥା
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#endif

    buffer[BENCHMARK_IDLE_ସଂଖ୍ୟା_SCHEDULES] = ଗଣ୍ଠି_STATE(
                                                  ksIdleThread)->benchmark.ସଂଖ୍ୟା_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_IDLE_KERNEL_UTILISATION] = ଗଣ୍ଠି_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_IDLE_ସଂଖ୍ୟା_KERNEL_ENTRIES] = ଗଣ୍ଠି_STATE(
                                                       ksIdleThread)->benchmark.ସଂଖ୍ୟା_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    buffer[BENCHMARK_TOTAL_UTILISATION] =
        (ARCH_ଗଣ୍ଠି_STATE(ccnt_ଅଙ୍କ_overflows) * 0xFFFFFFFFU) + ଗଣ୍ଠି_STATE(benchmark_end_time) - ଗଣ୍ଠି_STATE(benchmark_start_time);
#ଅନ୍ୟଥା
    buffer[BENCHMARK_TOTAL_UTILISATION] = ଗଣ୍ଠି_STATE(benchmark_end_time) - ଗଣ୍ଠି_STATE(
                                              benchmark_start_time); /* Overall time */
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    buffer[BENCHMARK_TOTAL_ସଂଖ୍ୟା_SCHEDULES] = ଗଣ୍ଠି_STATE(benchmark_kernel_ସଂଖ୍ୟା_schedules);
    buffer[BENCHMARK_TOTAL_KERNEL_UTILISATION] = ଗଣ୍ଠି_STATE(benchmark_kernel_time);
    buffer[BENCHMARK_TOTAL_ସଂଖ୍ୟା_KERNEL_ENTRIES] = ଗଣ୍ଠି_STATE(benchmark_kernel_ସଂଖ୍ୟା_entries);

}

ଶୂନ୍ୟ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.ସଂଖ୍ୟା_schedules = 0;
    tcb->benchmark.ସଂଖ୍ୟା_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.schedule_start_time = 0;
}
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
