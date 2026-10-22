/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <benchmark/benchmark_utilisation.h>

#अगर_घोषित CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

व्योम benchmark_track_utilisation_dump(व्योम)
{
    uint64_t *बफर = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, गाँठ_STATE(ksCurThread)))->संदेश[0]));
    tcb_t *tcb = शून्य;
    word_t tcb_cptr = getRegister(गाँठ_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(गाँठ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    अगर (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        वापस;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    बफर[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    बफर[BENCHMARK_TCB_संख्या_SCHEDULES] = tcb->benchmark.संख्या_schedules; /* Number of times scheduled */
    बफर[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    बफर[BENCHMARK_TCB_संख्या_KERNEL_ENTRIES] = tcb->benchmark.संख्या_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    बफर[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = गाँठ_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#अगर_घोषित ENABLE_SMP_SUPPORT
    बफर[BENCHMARK_IDLE_TCBCPU_UTILISATION] = गाँठ_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#अन्यथा
    बफर[BENCHMARK_IDLE_TCBCPU_UTILISATION] = बफर[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#पूर्ण_अगर

    बफर[BENCHMARK_IDLE_संख्या_SCHEDULES] = गाँठ_STATE(
                                                  ksIdleThread)->benchmark.संख्या_schedules; /* Number of times scheduled */
    बफर[BENCHMARK_IDLE_KERNEL_UTILISATION] = गाँठ_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    बफर[BENCHMARK_IDLE_संख्या_KERNEL_ENTRIES] = गाँठ_STATE(
                                                       ksIdleThread)->benchmark.संख्या_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#अगर_घोषित CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    बफर[BENCHMARK_योग_UTILISATION] =
        (ARCH_गाँठ_STATE(ccnt_अंक_overflows) * 0xFFFFFFFFU) + गाँठ_STATE(benchmark_end_time) - गाँठ_STATE(benchmark_start_time);
#अन्यथा
    बफर[BENCHMARK_योग_UTILISATION] = गाँठ_STATE(benchmark_end_time) - गाँठ_STATE(
                                              benchmark_start_time); /* Overall time */
#पूर्ण_अगर /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    बफर[BENCHMARK_योग_संख्या_SCHEDULES] = गाँठ_STATE(benchmark_kernel_संख्या_schedules);
    बफर[BENCHMARK_योग_KERNEL_UTILISATION] = गाँठ_STATE(benchmark_kernel_time);
    बफर[BENCHMARK_योग_संख्या_KERNEL_ENTRIES] = गाँठ_STATE(benchmark_kernel_संख्या_entries);

}

व्योम benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.संख्या_schedules = 0;
    tcb->benchmark.संख्या_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.schedule_start_time = 0;
}
#पूर्ण_अगर /* CONFIG_BENCHMARK_TRACK_UTILISATION */
