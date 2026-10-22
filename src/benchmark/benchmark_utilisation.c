/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

शून्य benchmark_track_utilisation_dump(शून्य)
{
    uint64_t *बफर = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, ग्रन्थिः_STATE(ksCurThread)))->सन्देशः[0]));
    tcb_t *tcb = रिक्त;
    word_t tcb_cptr = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(ग्रन्थिः_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    यदि (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        प्रत्यागम;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    बफर[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    बफर[BENCHMARK_TCB_सङ्ख्या_SCHEDULES] = tcb->benchmark.सङ्ख्या_schedules; /* Number of times scheduled */
    बफर[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    बफर[BENCHMARK_TCB_सङ्ख्या_KERNEL_ENTRIES] = tcb->benchmark.सङ्ख्या_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    बफर[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = ग्रन्थिः_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    बफर[BENCHMARK_IDLE_TCBCPU_UTILISATION] = ग्रन्थिः_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#अन्यथा
    बफर[BENCHMARK_IDLE_TCBCPU_UTILISATION] = बफर[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#यदि_अन्तः

    बफर[BENCHMARK_IDLE_सङ्ख्या_SCHEDULES] = ग्रन्थिः_STATE(
                                                  ksIdleThread)->benchmark.सङ्ख्या_schedules; /* Number of times scheduled */
    बफर[BENCHMARK_IDLE_KERNEL_UTILISATION] = ग्रन्थिः_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    बफर[BENCHMARK_IDLE_सङ्ख्या_KERNEL_ENTRIES] = ग्रन्थिः_STATE(
                                                       ksIdleThread)->benchmark.सङ्ख्या_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#यदि_परिभाषितम् CONFIG_ARM_ENABLE_PMU_अतिप्रवाहः_व्यवधानम्
    बफर[BENCHMARK_योग_UTILISATION] =
        (ARCH_ग्रन्थिः_STATE(ccnt_अङ्कः_overflows) * 0xFFFFFFFFU) + ग्रन्थिः_STATE(benchmark_अन्तः_time) - ग्रन्थिः_STATE(benchmark_start_time);
#अन्यथा
    बफर[BENCHMARK_योग_UTILISATION] = ग्रन्थिः_STATE(benchmark_अन्तः_time) - ग्रन्थिः_STATE(
                                              benchmark_start_time); /* Overall time */
#यदि_अन्तः /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    बफर[BENCHMARK_योग_सङ्ख्या_SCHEDULES] = ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_schedules);
    बफर[BENCHMARK_योग_KERNEL_UTILISATION] = ग्रन्थिः_STATE(benchmark_kernel_time);
    बफर[BENCHMARK_योग_सङ्ख्या_KERNEL_ENTRIES] = ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_entries);

}

शून्य benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.सङ्ख्या_schedules = 0;
    tcb->benchmark.सङ्ख्या_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.कालसारणी_start_time = 0;
}
#यदि_अन्तः /* CONFIG_BENCHMARK_TRACK_UTILISATION */
