/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_ENABLE_BENCHMARKS

#ضمن <types.h>
#ضمن <mode/machine.h>
#ضمن <benchmark/benchmark.h>
#ضمن <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(فراغ)
{
#اگر_تعریف_شده CONFIG_ARCH_ARM
    tcb_t *thread = گره_STATE(ksCurThread);
    إذا (get_ثبات(thread, cap_ثبات)) {
        arch_clean_invalidate_L1_caches(get_ثبات(thread, پیام_Info_ثبات));
    } وإلا {
        arch_clean_invalidate_caches();
    }
#وإلا
    arch_clean_invalidate_caches();
#پایان_اگر
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(فراغ)
{
#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
    إذا (ks_User_Log_میانگیر == 0) {
        userError("A user-level میانگیر has مقصد be مجموعه before resetting benchmark.\
                Use se_L4_Benchmark_مجموعه_Log_میانگیر\n");
        مجموعه_ثبات(گره_STATE(ksCurThread), cap_ثبات, seL4_IllegalOperation);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_شاخص = 0;
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_UTILISATION
    گره_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(گره_STATE(ksIdleThread));
    گره_STATE(ksCurThread)->benchmark.زمانبندی_شروع_زمان = ksEnter;
    گره_STATE(ksCurThread)->benchmark.عدد_schedules++;
    گره_STATE(benchmark_شروع_زمان) = ksEnter;
    گره_STATE(benchmark_kernel_زمان) = 0;
    گره_STATE(benchmark_kernel_عدد_entries) = 0;
    گره_STATE(benchmark_kernel_عدد_schedules) = 1;
    benchmark_arch_utilisation_reset();
#پایان_اگر /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    مجموعه_ثبات(گره_STATE(ksCurThread), cap_ثبات, seL4_NoError);
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(فراغ)
{
#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
    ks_Log_شاخص_Finalized = ks_Log_شاخص;
    مجموعه_ثبات(گره_STATE(ksCurThread), cap_ثبات, ks_Log_شاخص_Finalized);
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#پایان_اگر /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    أرجع EXCEPTION_NONE;
}

#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
exception_t handle_Sys_Benchmark_مجموعه_Log_میانگیر(فراغ)
{
    کلمه_t cptr_user_قاب = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
    إذا (benchmark_arch_map_log_میانگیر(cptr_user_قاب) != EXCEPTION_NONE) {
        مجموعه_ثبات(گره_STATE(ksCurThread), cap_ثبات, seL4_IllegalOperation);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_ثبات(گره_STATE(ksCurThread), cap_ثبات, seL4_NoError);
    أرجع EXCEPTION_NONE;
}
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(فراغ)
{
    benchmark_track_utilisation_dump();
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(فراغ)
{
    کلمه_t tcb_cptr = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
    lookupCap_ret_t lu_ret;
    کلمه_t cap_type;

    lu_ret = lookupCap(گره_STATE(ksCurThread), tcb_cptr);
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

#اگر_تعریف_شده CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_نخها_Utilisation(فراغ)
{
    چاپ_کن("{\n");
    چاپ_کن("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (کلمه_t)(گره_STATE(benchmark_انتها_زمان) - گره_STATE(benchmark_شروع_زمان)));
    چاپ_کن("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (کلمه_t) گره_STATE(benchmark_kernel_زمان));
    چاپ_کن("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (کلمه_t) گره_STATE(benchmark_kernel_عدد_entries));
    چاپ_کن("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (کلمه_t) گره_STATE(benchmark_kernel_عدد_schedules));
    چاپ_کن("  \"BENCHMARK_TCB_\": [\n");
    لكل (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بعدی) {
        چاپ_کن("    {\n");
        چاپ_کن("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        چاپ_کن("      \"UTILISATION\":%lu,\n", (کلمه_t) curr->benchmark.utilisation);
        چاپ_کن("      \"NUMBER_SCHEDULES\":%lu,\n", (کلمه_t) curr->benchmark.عدد_schedules);
        چاپ_کن("      \"KERNEL_UTILISATION\":%lu,\n", (کلمه_t) curr->benchmark.kernel_utilisation);
        چاپ_کن("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (کلمه_t) curr->benchmark.عدد_kernel_entries);
        چاپ_کن("    }");
        إذا (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بعدی != عدم) {
            چاپ_کن(",\n");
        } وإلا {
            چاپ_کن("\n");
        }
    }
    چاپ_کن("  ]\n}\n");
    أرجع EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_نخها_Utilisation(فراغ)
{
    لكل (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بعدی) {
        benchmark_track_reset_utilisation(curr);
    }
    أرجع EXCEPTION_NONE;
}

#پایان_اگر /* CONFIG_DEBUG_BUILD */
#پایان_اگر /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#پایان_اگر /* CONFIG_ENABLE_BENCHMARKS */
