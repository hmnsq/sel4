/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

ಶೂನ್ಯ benchmark_track_utilisation_dump(ಶೂನ್ಯ)
{
    uint64_t *ಬಫರ್ = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, ಗಂಟು_STATE(ksCurThread)))->ಸಂದೇಶ[0]));
    tcb_t *tcb = ಖಾಲಿ;
    ಶಬ್ದ_t tcb_cptr = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
    lookupCap_ret_t lu_ret;
    ಶಬ್ದ_t cap_type;

    lu_ret = lookupCap(ಗಂಟು_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ಆದರೆ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        ಹಿಂತಿರುಗಿ;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    ಬಫರ್[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    ಬಫರ್[BENCHMARK_TCB_ಸಂಖ್ಯೆ_SCHEDULES] = tcb->benchmark.ಸಂಖ್ಯೆ_schedules; /* Number of times scheduled */
    ಬಫರ್[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    ಬಫರ್[BENCHMARK_TCB_ಸಂಖ್ಯೆ_KERNEL_ENTRIES] = tcb->benchmark.ಸಂಖ್ಯೆ_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    ಬಫರ್[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = ಗಂಟು_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಬಫರ್[BENCHMARK_IDLE_TCBCPU_UTILISATION] = ಗಂಟು_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#ಇಲ್ಲದಿದ್ದರೆ
    ಬಫರ್[BENCHMARK_IDLE_TCBCPU_UTILISATION] = ಬಫರ್[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#ಆದರೆ_ಅಂತ್ಯ

    ಬಫರ್[BENCHMARK_IDLE_ಸಂಖ್ಯೆ_SCHEDULES] = ಗಂಟು_STATE(
                                                  ksIdleThread)->benchmark.ಸಂಖ್ಯೆ_schedules; /* Number of times scheduled */
    ಬಫರ್[BENCHMARK_IDLE_KERNEL_UTILISATION] = ಗಂಟು_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    ಬಫರ್[BENCHMARK_IDLE_ಸಂಖ್ಯೆ_KERNEL_ENTRIES] = ಗಂಟು_STATE(
                                                       ksIdleThread)->benchmark.ಸಂಖ್ಯೆ_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_ENABLE_PMU_ಅತಿಪ್ರವಾಹ_ಅಡಚಣೆ
    ಬಫರ್[BENCHMARK_ಒಟ್ಟು_UTILISATION] =
        (ARCH_ಗಂಟು_STATE(ccnt_ಅಂಕೆ_overflows) * 0xFFFFFFFFU) + ಗಂಟು_STATE(benchmark_ಕೊನೆ_ಸಮಯ) - ಗಂಟು_STATE(benchmark_ಆರಂಭ_ಸಮಯ);
#ಇಲ್ಲದಿದ್ದರೆ
    ಬಫರ್[BENCHMARK_ಒಟ್ಟು_UTILISATION] = ಗಂಟು_STATE(benchmark_ಕೊನೆ_ಸಮಯ) - ಗಂಟು_STATE(
                                              benchmark_ಆರಂಭ_ಸಮಯ); /* Overall time */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    ಬಫರ್[BENCHMARK_ಒಟ್ಟು_ಸಂಖ್ಯೆ_SCHEDULES] = ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_schedules);
    ಬಫರ್[BENCHMARK_ಒಟ್ಟು_KERNEL_UTILISATION] = ಗಂಟು_STATE(benchmark_kernel_ಸಮಯ);
    ಬಫರ್[BENCHMARK_ಒಟ್ಟು_ಸಂಖ್ಯೆ_KERNEL_ENTRIES] = ಗಂಟು_STATE(benchmark_kernel_ಸಂಖ್ಯೆ_entries);

}

ಶೂನ್ಯ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.ಸಂಖ್ಯೆ_schedules = 0;
    tcb->benchmark.ಸಂಖ್ಯೆ_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.ವೇಳಾಪಟ್ಟಿ_ಆರಂಭ_ಸಮಯ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_BENCHMARK_TRACK_UTILISATION */
