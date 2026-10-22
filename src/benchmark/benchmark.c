/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_ENABLE_BENCHMARKS

#ضمن <types.h>
#ضمن <mode/machine.h>
#ضمن <benchmark/benchmark.h>
#ضمن <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(فراغ)
{
#که_تعریف_شوی CONFIG_ARCH_ARM
    tcb_t *thread = غوټه_STATE(ksCurThread);
    إذا (get_ثبتوونکی(thread, cap_ثبتوونکی)) {
        arch_clean_invalidate_L1_caches(get_ثبتوونکی(thread, پیغام_Info_ثبتوونکی));
    } وإلا {
        arch_clean_invalidate_caches();
    }
#وإلا
    arch_clean_invalidate_caches();
#د_که_پای
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(فراغ)
{
#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
    إذا (ks_User_Log_بفر == 0) {
        userError("A user-level بفر has منزل be سېټ before resetting benchmark.\
                Use se_L4_Benchmark_سېټ_Log_بفر\n");
        سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی, seL4_IllegalOperation);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_شاخص = 0;
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_UTILISATION
    غوټه_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(غوټه_STATE(ksIdleThread));
    غوټه_STATE(ksCurThread)->benchmark.مهالوېش_پیل_وخت = ksEnter;
    غوټه_STATE(ksCurThread)->benchmark.شمېره_schedules++;
    غوټه_STATE(benchmark_پیل_وخت) = ksEnter;
    غوټه_STATE(benchmark_kernel_وخت) = 0;
    غوټه_STATE(benchmark_kernel_شمېره_entries) = 0;
    غوټه_STATE(benchmark_kernel_شمېره_schedules) = 1;
    benchmark_arch_utilisation_reset();
#د_که_پای /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی, seL4_NoError);
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(فراغ)
{
#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
    ks_Log_شاخص_Finalized = ks_Log_شاخص;
    سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی, ks_Log_شاخص_Finalized);
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#د_که_پای /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    أرجع EXCEPTION_NONE;
}

#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
exception_t handle_Sys_Benchmark_سېټ_Log_بفر(فراغ)
{
    کلمه_t cptr_user_چوکاټ = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
    إذا (benchmark_arch_map_log_بفر(cptr_user_چوکاټ) != EXCEPTION_NONE) {
        سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی, seL4_IllegalOperation);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی, seL4_NoError);
    أرجع EXCEPTION_NONE;
}
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(فراغ)
{
    benchmark_track_utilisation_dump();
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(فراغ)
{
    کلمه_t tcb_cptr = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
    lookupCap_ret_t lu_ret;
    کلمه_t cap_type;

    lu_ret = lookupCap(غوټه_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    إذا (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        أرجع EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    أرجع EXCEPTION_NONE;
}

#که_تعریف_شوی CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_تارونه_Utilisation(فراغ)
{
    چاپ_کړه("{\n");
    چاپ_کړه("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (کلمه_t)(غوټه_STATE(benchmark_پای_ټکی_وخت) - غوټه_STATE(benchmark_پیل_وخت)));
    چاپ_کړه("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (کلمه_t) غوټه_STATE(benchmark_kernel_وخت));
    چاپ_کړه("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (کلمه_t) غوټه_STATE(benchmark_kernel_شمېره_entries));
    چاپ_کړه("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (کلمه_t) غوټه_STATE(benchmark_kernel_شمېره_schedules));
    چاپ_کړه("  \"BENCHMARK_TCB_\": [\n");
    لكل (tcb_t *curr = غوټه_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بل) {
        چاپ_کړه("    {\n");
        چاپ_کړه("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        چاپ_کړه("      \"UTILISATION\":%lu,\n", (کلمه_t) curr->benchmark.utilisation);
        چاپ_کړه("      \"NUMBER_SCHEDULES\":%lu,\n", (کلمه_t) curr->benchmark.شمېره_schedules);
        چاپ_کړه("      \"KERNEL_UTILISATION\":%lu,\n", (کلمه_t) curr->benchmark.kernel_utilisation);
        چاپ_کړه("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (کلمه_t) curr->benchmark.شمېره_kernel_entries);
        چاپ_کړه("    }");
        إذا (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بل != عدم) {
            چاپ_کړه(",\n");
        } وإلا {
            چاپ_کړه("\n");
        }
    }
    چاپ_کړه("  ]\n}\n");
    أرجع EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_تارونه_Utilisation(فراغ)
{
    لكل (tcb_t *curr = غوټه_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بل) {
        benchmark_track_reset_utilisation(curr);
    }
    أرجع EXCEPTION_NONE;
}

#د_که_پای /* CONFIG_DEBUG_BUILD */
#د_که_پای /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#د_که_پای /* CONFIG_ENABLE_BENCHMARKS */
