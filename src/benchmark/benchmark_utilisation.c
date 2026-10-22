/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_utilisation.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

ਖਾਲੀ benchmark_track_utilisation_dump(ਖਾਲੀ)
{
    uint64_t *ਬਫਰ = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, ਗੰਢ_STATE(ksCurThread)))->ਸੁਨੇਹਾ[0]));
    tcb_t *tcb = ਨਿਰਾ;
    ਸ਼ਬਦ_t tcb_cptr = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
    lookupCap_ret_t lu_ret;
    ਸ਼ਬਦ_t cap_type;

    lu_ret = lookupCap(ਗੰਢ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ਜੇ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        ਵਾਪਸ;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    ਬਫਰ[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    ਬਫਰ[BENCHMARK_TCB_ਸੰਖਿਆ_SCHEDULES] = tcb->benchmark.ਸੰਖਿਆ_schedules; /* Number of times scheduled */
    ਬਫਰ[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    ਬਫਰ[BENCHMARK_TCB_ਸੰਖਿਆ_KERNEL_ENTRIES] = tcb->benchmark.ਸੰਖਿਆ_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    ਬਫਰ[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = ਗੰਢ_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਬਫਰ[BENCHMARK_IDLE_TCBCPU_UTILISATION] = ਗੰਢ_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#ਨਹੀਂਤਾਂ
    ਬਫਰ[BENCHMARK_IDLE_TCBCPU_UTILISATION] = ਬਫਰ[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#ਜੇ_ਅੰਤ

    ਬਫਰ[BENCHMARK_IDLE_ਸੰਖਿਆ_SCHEDULES] = ਗੰਢ_STATE(
                                                  ksIdleThread)->benchmark.ਸੰਖਿਆ_schedules; /* Number of times scheduled */
    ਬਫਰ[BENCHMARK_IDLE_KERNEL_UTILISATION] = ਗੰਢ_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    ਬਫਰ[BENCHMARK_IDLE_ਸੰਖਿਆ_KERNEL_ENTRIES] = ਗੰਢ_STATE(
                                                       ksIdleThread)->benchmark.ਸੰਖਿਆ_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_ENABLE_PMU_ਉਛਾਲ_ਰੁਕਾਵਟ
    ਬਫਰ[BENCHMARK_ਕੁੱਲ_UTILISATION] =
        (ARCH_ਗੰਢ_STATE(ccnt_ਅੰਕ_overflows) * 0xFFFFFFFFU) + ਗੰਢ_STATE(benchmark_ਸਿਰਾ_ਸਮਾਂ) - ਗੰਢ_STATE(benchmark_ਸ਼ੁਰੂ_ਸਮਾਂ);
#ਨਹੀਂਤਾਂ
    ਬਫਰ[BENCHMARK_ਕੁੱਲ_UTILISATION] = ਗੰਢ_STATE(benchmark_ਸਿਰਾ_ਸਮਾਂ) - ਗੰਢ_STATE(
                                              benchmark_ਸ਼ੁਰੂ_ਸਮਾਂ); /* Overall time */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    ਬਫਰ[BENCHMARK_ਕੁੱਲ_ਸੰਖਿਆ_SCHEDULES] = ਗੰਢ_STATE(benchmark_kernel_ਸੰਖਿਆ_schedules);
    ਬਫਰ[BENCHMARK_ਕੁੱਲ_KERNEL_UTILISATION] = ਗੰਢ_STATE(benchmark_kernel_ਸਮਾਂ);
    ਬਫਰ[BENCHMARK_ਕੁੱਲ_ਸੰਖਿਆ_KERNEL_ENTRIES] = ਗੰਢ_STATE(benchmark_kernel_ਸੰਖਿਆ_entries);

}

ਖਾਲੀ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.ਸੰਖਿਆ_schedules = 0;
    tcb->benchmark.ਸੰਖਿਆ_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.ਸਮਾਂ_ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਸਮਾਂ = 0;
}
#ਜੇ_ਅੰਤ /* CONFIG_BENCHMARK_TRACK_UTILISATION */
