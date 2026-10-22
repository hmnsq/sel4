/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <benchmark/benchmark_utilisation.h>

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

فراغ benchmark_track_utilisation_dump(فراغ)
{
    uint64_t *بفر = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, غوټه_STATE(ksCurThread)))->پیغام[0]));
    tcb_t *tcb = عدم;
    کلمه_t tcb_cptr = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
    lookupCap_ret_t lu_ret;
    کلمه_t cap_type;

    lu_ret = lookupCap(غوټه_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    إذا (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        أرجع;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    بفر[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    بفر[BENCHMARK_TCB_شمېره_SCHEDULES] = tcb->benchmark.شمېره_schedules; /* Number of times scheduled */
    بفر[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    بفر[BENCHMARK_TCB_شمېره_KERNEL_ENTRIES] = tcb->benchmark.شمېره_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    بفر[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = غوټه_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    بفر[BENCHMARK_IDLE_TCBCPU_UTILISATION] = غوټه_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#وإلا
    بفر[BENCHMARK_IDLE_TCBCPU_UTILISATION] = بفر[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#د_که_پای

    بفر[BENCHMARK_IDLE_شمېره_SCHEDULES] = غوټه_STATE(
                                                  ksIdleThread)->benchmark.شمېره_schedules; /* Number of times scheduled */
    بفر[BENCHMARK_IDLE_KERNEL_UTILISATION] = غوټه_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    بفر[BENCHMARK_IDLE_شمېره_KERNEL_ENTRIES] = غوټه_STATE(
                                                       ksIdleThread)->benchmark.شمېره_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#که_تعریف_شوی CONFIG_ARM_ENABLE_PMU_ډکېدنه_مداخله
    بفر[BENCHMARK_ټول_UTILISATION] =
        (ARCH_غوټه_STATE(ccnt_عدد_overflows) * 0xFFFFFFFFU) + غوټه_STATE(benchmark_پای_ټکی_وخت) - غوټه_STATE(benchmark_پیل_وخت);
#وإلا
    بفر[BENCHMARK_ټول_UTILISATION] = غوټه_STATE(benchmark_پای_ټکی_وخت) - غوټه_STATE(
                                              benchmark_پیل_وخت); /* Overall time */
#د_که_پای /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    بفر[BENCHMARK_ټول_شمېره_SCHEDULES] = غوټه_STATE(benchmark_kernel_شمېره_schedules);
    بفر[BENCHMARK_ټول_KERNEL_UTILISATION] = غوټه_STATE(benchmark_kernel_وخت);
    بفر[BENCHMARK_ټول_شمېره_KERNEL_ENTRIES] = غوټه_STATE(benchmark_kernel_شمېره_entries);

}

فراغ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.شمېره_schedules = 0;
    tcb->benchmark.شمېره_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.مهالوېش_پیل_وخت = 0;
}
#د_که_پای /* CONFIG_BENCHMARK_TRACK_UTILISATION */
