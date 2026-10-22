/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <benchmark/benchmark_utilisation.h>

#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_UTILISATION

timestamp_t ksEnter;

فراغ benchmark_track_utilisation_dump(فراغ)
{
    uint64_t *میانگیر = ((uint64_t *) & (((seL4_IPCBuffer *)lookupIPCBuffer(true, گره_STATE(ksCurThread)))->پیام[0]));
    tcb_t *tcb = عدم;
    کلمه_t tcb_cptr = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
    lookupCap_ret_t lu_ret;
    کلمه_t cap_type;

    lu_ret = lookupCap(گره_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    إذا (cap_type != cap_thread_cap) {
        userError("SysBenchmarkFinalizeLog: cap is not a TCB, halting");
        أرجع;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    /* Selected TCB counters */
    میانگیر[BENCHMARK_TCB_UTILISATION] = tcb->benchmark.utilisation; /* Requested thread utilisation */
    میانگیر[BENCHMARK_TCB_عدد_SCHEDULES] = tcb->benchmark.عدد_schedules; /* Number of times scheduled */
    میانگیر[BENCHMARK_TCB_KERNEL_UTILISATION] = tcb->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    میانگیر[BENCHMARK_TCB_عدد_KERNEL_ENTRIES] = tcb->benchmark.عدد_kernel_entries; /* Number of kernel entries */

    /* Idle counters */
    میانگیر[BENCHMARK_IDLE_LOCALCPU_UTILISATION] = گره_STATE(
                                                      ksIdleThread)->benchmark.utilisation; /* Idle thread utilisation of current CPU */
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    میانگیر[BENCHMARK_IDLE_TCBCPU_UTILISATION] = گره_STATE_ON_CORE(ksIdleThread,
                                                                   tcb->tcbAffinity)->benchmark.utilisation; /* Idle thread utilisation of CPU the TCB is running on */
#وإلا
    میانگیر[BENCHMARK_IDLE_TCBCPU_UTILISATION] = میانگیر[BENCHMARK_IDLE_LOCALCPU_UTILISATION];
#پایان_اگر

    میانگیر[BENCHMARK_IDLE_عدد_SCHEDULES] = گره_STATE(
                                                  ksIdleThread)->benchmark.عدد_schedules; /* Number of times scheduled */
    میانگیر[BENCHMARK_IDLE_KERNEL_UTILISATION] = گره_STATE(
                                                    ksIdleThread)->benchmark.kernel_utilisation; /* Utilisation spent in kernel */
    میانگیر[BENCHMARK_IDLE_عدد_KERNEL_ENTRIES] = گره_STATE(
                                                       ksIdleThread)->benchmark.عدد_kernel_entries; /* Number of kernel entries */


    /* Total counters */
#اگر_تعریف_شده CONFIG_ARM_ENABLE_PMU_سرریز_وقفه
    میانگیر[BENCHMARK_مجموع_UTILISATION] =
        (ARCH_گره_STATE(ccnt_رقم_overflows) * 0xFFFFFFFFU) + گره_STATE(benchmark_انتها_زمان) - گره_STATE(benchmark_شروع_زمان);
#وإلا
    میانگیر[BENCHMARK_مجموع_UTILISATION] = گره_STATE(benchmark_انتها_زمان) - گره_STATE(
                                              benchmark_شروع_زمان); /* Overall time */
#پایان_اگر /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */
    میانگیر[BENCHMARK_مجموع_عدد_SCHEDULES] = گره_STATE(benchmark_kernel_عدد_schedules);
    میانگیر[BENCHMARK_مجموع_KERNEL_UTILISATION] = گره_STATE(benchmark_kernel_زمان);
    میانگیر[BENCHMARK_مجموع_عدد_KERNEL_ENTRIES] = گره_STATE(benchmark_kernel_عدد_entries);

}

فراغ benchmark_track_reset_utilisation(tcb_t *tcb)
{
    tcb->benchmark.utilisation = 0;
    tcb->benchmark.عدد_schedules = 0;
    tcb->benchmark.عدد_kernel_entries = 0;
    tcb->benchmark.kernel_utilisation = 0;
    tcb->benchmark.زمانبندی_شروع_زمان = 0;
}
#پایان_اگر /* CONFIG_BENCHMARK_TRACK_UTILISATION */
