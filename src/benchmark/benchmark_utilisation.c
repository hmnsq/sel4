/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <benchmark/benchmark_utilisation.h>

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

శూన్య benchmark_track_utilisation_dump(శూన్య)
{
    uint64_t *బఫరు = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, కణుపు_STATE(ksCurThread)))->సందేశం[0]));
    tcb_t *tcb = ఖాళీ;
    మాట_t tcb_cptr = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
    lookupCap_ret_t lu_ret;
    మాట_t cap_type;

    lu_ret = lookupCap(కణుపు_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ఒకవేళ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        తిరిగి;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    బఫరు[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    బఫరు[BENCHMARK_TCB_సంఖ్య_SCHEDULES] = tcb->benchmark.సంఖ్య_schedules; /* Number of times scheduled */
    బఫరు[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    బఫరు[BENCHMARK_TCB_సంఖ్య_KERNEL_ENTRIES] = tcb->benchmark.సంఖ్య_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    బఫరు[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = కణుపు_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    బఫరు[BENCHMARK_IDLE_TCBCPU_UTILISATION] = కణుపు_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#లేకపోతే
    బఫరు[BENCHMARK_IDLE_TCBCPU_UTILISATION] = బఫరు[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#అయితే_ముగింపు

    బఫరు[BENCHMARK_IDLE_సంఖ్య_SCHEDULES] = కణుపు_STATE(
                                                  ksIdleThread)->benchmark.సంఖ్య_schedules; /* Number of times scheduled */
    బఫరు[BENCHMARK_IDLE_KERNEL_UTILISATION] = కణుపు_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    బఫరు[BENCHMARK_IDLE_సంఖ్య_KERNEL_ENTRIES] = కణుపు_STATE(
                                                       ksIdleThread)->benchmark.సంఖ్య_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#నిర్వచితమైతే CONFIG_ARM_ENABLE_PMU_అధికప్రవాహం_అంతరాయం
    బఫరు[BENCHMARK_మొత్తం_UTILISATION] =
        (ARCH_కణుపు_STATE(ccnt_అంకె_overflows) * 0xFFFFFFFFU) + కణుపు_STATE(benchmark_చివర_సమయం) - కణుపు_STATE(benchmark_ప్రారంభం_సమయం);
#లేకపోతే
    బఫరు[BENCHMARK_మొత్తం_UTILISATION] = కణుపు_STATE(benchmark_చివర_సమయం) - కణుపు_STATE(
                                              benchmark_ప్రారంభం_సమయం); /* Overall time */
#అయితే_ముగింపు /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    బఫరు[BENCHMARK_మొత్తం_సంఖ్య_SCHEDULES] = కణుపు_STATE(benchmark_kernel_సంఖ్య_schedules);
    బఫరు[BENCHMARK_మొత్తం_KERNEL_UTILISATION] = కణుపు_STATE(benchmark_kernel_సమయం);
    బఫరు[BENCHMARK_మొత్తం_సంఖ్య_KERNEL_ENTRIES] = కణుపు_STATE(benchmark_kernel_సంఖ్య_entries);

}

శూన్య benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.సంఖ్య_schedules = 0;
    tcb->benchmark.సంఖ్య_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.కాలపట్టిక_ప్రారంభం_సమయం = 0;
}
#అయితే_ముగింపు /* CONFIG_BENCHMARK_TRACK_UTILISATION */
