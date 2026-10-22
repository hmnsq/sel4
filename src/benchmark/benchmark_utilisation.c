/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <benchmark/benchmark_utilisation.h>

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

فراغ benchmark_track_utilisation_dump(فراغ)
{
    uint64_t *بفر = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, ڳنڍ_STATE(ksCurThread)))->نياپو[0]));
    tcb_t *tcb = عدم;
    لفظ_t tcb_cptr = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
    lookupCap_ret_t lu_ret;
    لفظ_t cap_type;

    lu_ret = lookupCap(ڳنڍ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    إذا (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        أرجع;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    بفر[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    بفر[BENCHMARK_TCB_عدد_SCHEDULES] = tcb->benchmark.عدد_schedules; /* Number of times scheduled */
    بفر[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    بفر[BENCHMARK_TCB_عدد_KERNEL_ENTRIES] = tcb->benchmark.عدد_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    بفر[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = ڳنڍ_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    بفر[BENCHMARK_IDLE_TCBCPU_UTILISATION] = ڳنڍ_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#وإلا
    بفر[BENCHMARK_IDLE_TCBCPU_UTILISATION] = بفر[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#جيڪڏهن_پورو

    بفر[BENCHMARK_IDLE_عدد_SCHEDULES] = ڳنڍ_STATE(
                                                  ksIdleThread)->benchmark.عدد_schedules; /* Number of times scheduled */
    بفر[BENCHMARK_IDLE_KERNEL_UTILISATION] = ڳنڍ_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    بفر[BENCHMARK_IDLE_عدد_KERNEL_ENTRIES] = ڳنڍ_STATE(
                                                       ksIdleThread)->benchmark.عدد_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_ENABLE_PMU_حد_کان_مٿي_مداخلت
    بفر[BENCHMARK_ڪل_UTILISATION] =
        (ARCH_ڳنڍ_STATE(ccnt_انگ_overflows) * 0xFFFFFFFFU) + ڳنڍ_STATE(benchmark_پڇاڙي_وقت) - ڳنڍ_STATE(benchmark_شروعات_وقت);
#وإلا
    بفر[BENCHMARK_ڪل_UTILISATION] = ڳنڍ_STATE(benchmark_پڇاڙي_وقت) - ڳنڍ_STATE(
                                              benchmark_شروعات_وقت); /* Overall time */
#جيڪڏهن_پورو /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    بفر[BENCHMARK_ڪل_عدد_SCHEDULES] = ڳنڍ_STATE(benchmark_kernel_عدد_schedules);
    بفر[BENCHMARK_ڪل_KERNEL_UTILISATION] = ڳنڍ_STATE(benchmark_kernel_وقت);
    بفر[BENCHMARK_ڪل_عدد_KERNEL_ENTRIES] = ڳنڍ_STATE(benchmark_kernel_عدد_entries);

}

فراغ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.عدد_schedules = 0;
    tcb->benchmark.عدد_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.وقت_جدول_شروعات_وقت = 0;
}
#جيڪڏهن_پورو /* CONFIG_BENCHMARK_TRACK_UTILISATION */
