/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <benchmark/benchmark_utilisation.h>

#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

فراغ benchmark_track_utilisation_dump(فراغ)
{
    uint64_t *بفر = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, گرہ_STATE(ksCurThread)))->پیغام[0]));
    tcb_t *tcb = عدم;
    لفظ_t tcb_cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
    lookupCap_ret_t lu_ret;
    لفظ_t cap_type;

    lu_ret = lookupCap(گرہ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    إذا (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        أرجع;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    بفر[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    بفر[BENCHMARK_TCB_نمبر_SCHEDULES] = tcb->benchmark.نمبر_schedules; /* Number of times scheduled */
    بفر[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    بفر[BENCHMARK_TCB_نمبر_KERNEL_ENTRIES] = tcb->benchmark.نمبر_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    بفر[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = گرہ_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    بفر[BENCHMARK_IDLE_TCBCPU_UTILISATION] = گرہ_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#وإلا
    بفر[BENCHMARK_IDLE_TCBCPU_UTILISATION] = بفر[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#جے_انت

    بفر[BENCHMARK_IDLE_نمبر_SCHEDULES] = گرہ_STATE(
                                                  ksIdleThread)->benchmark.نمبر_schedules; /* Number of times scheduled */
    بفر[BENCHMARK_IDLE_KERNEL_UTILISATION] = گرہ_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    بفر[BENCHMARK_IDLE_نمبر_KERNEL_ENTRIES] = گرہ_STATE(
                                                       ksIdleThread)->benchmark.نمبر_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#جے_تعریف_شدہ CONFIG_ARM_ENABLE_PMU_حد_توں_ودھ_رکاوٹ
    بفر[BENCHMARK_کل_UTILISATION] =
        (ARCH_گرہ_STATE(ccnt_عدد_overflows) * 0xFFFFFFFFU) + گرہ_STATE(benchmark_سرا_ویلا) - گرہ_STATE(benchmark_شروع_ویلا);
#وإلا
    بفر[BENCHMARK_کل_UTILISATION] = گرہ_STATE(benchmark_سرا_ویلا) - گرہ_STATE(
                                              benchmark_شروع_ویلا); /* Overall time */
#جے_انت /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    بفر[BENCHMARK_کل_نمبر_SCHEDULES] = گرہ_STATE(benchmark_kernel_نمبر_schedules);
    بفر[BENCHMARK_کل_KERNEL_UTILISATION] = گرہ_STATE(benchmark_kernel_ویلا);
    بفر[BENCHMARK_کل_نمبر_KERNEL_ENTRIES] = گرہ_STATE(benchmark_kernel_نمبر_entries);

}

فراغ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.نمبر_schedules = 0;
    tcb->benchmark.نمبر_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.ویلا_سارنی_شروع_ویلا = 0;
}
#جے_انت /* CONFIG_BENCHMARK_TRACK_UTILISATION */
