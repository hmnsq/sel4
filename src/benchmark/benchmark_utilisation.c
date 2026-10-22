/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

शून्य benchmark_track_utilisation_dump(शून्य)
{
    uint64_t *buffer = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, गाँठो_STATE(ksCurThread)))->सन्देश[0]));
    tcb_t *tcb = रिक्त;
    word_t tcb_cptr = getRegister(गाँठो_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(गाँठो_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    यदि (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        फर्काऊ;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    buffer[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    buffer[BENCHMARK_TCB_सङ्ख्या_SCHEDULES] = tcb->benchmark.सङ्ख्या_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_TCB_सङ्ख्या_KERNEL_ENTRIES] = tcb->benchmark.सङ्ख्या_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = गाँठो_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#ifdef ENABLE_SMP_SUPPORT
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = गाँठो_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#अन्यथा
    buffer[BENCHMARK_IDLE_TCBCPU_UTILISATION] = buffer[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#endif

    buffer[BENCHMARK_IDLE_सङ्ख्या_SCHEDULES] = गाँठो_STATE(
                                                  ksIdleThread)->benchmark.सङ्ख्या_schedules; /* Number of times scheduled */
    buffer[BENCHMARK_IDLE_KERNEL_UTILISATION] = गाँठो_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    buffer[BENCHMARK_IDLE_सङ्ख्या_KERNEL_ENTRIES] = गाँठो_STATE(
                                                       ksIdleThread)->benchmark.सङ्ख्या_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
    buffer[BENCHMARK_TOTAL_UTILISATION] =
        (ARCH_गाँठो_STATE(ccnt_अङ्क_overflows) * 0xFFFFFFFFU) + गाँठो_STATE(benchmark_end_time) - गाँठो_STATE(benchmark_start_time);
#अन्यथा
    buffer[BENCHMARK_TOTAL_UTILISATION] = गाँठो_STATE(benchmark_end_time) - गाँठो_STATE(
                                              benchmark_start_time); /* Overall time */
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    buffer[BENCHMARK_TOTAL_सङ्ख्या_SCHEDULES] = गाँठो_STATE(benchmark_kernel_सङ्ख्या_schedules);
    buffer[BENCHMARK_TOTAL_KERNEL_UTILISATION] = गाँठो_STATE(benchmark_kernel_time);
    buffer[BENCHMARK_TOTAL_सङ्ख्या_KERNEL_ENTRIES] = गाँठो_STATE(benchmark_kernel_सङ्ख्या_entries);

}

शून्य benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.सङ्ख्या_schedules = 0;
    tcb->benchmark.सङ्ख्या_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.schedule_start_time = 0;
}
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
