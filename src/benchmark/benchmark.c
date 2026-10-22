/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_ENABLE_BENCHMARKS

#ضمن <types.h>
#ضمن <mode/machine.h>
#ضمن <benchmark/benchmark.h>
#ضمن <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(فراغ)
{
#جے_تعریف_شدہ CONFIG_ARCH_ARM
    tcb_t *thread = گرہ_STATE(ksCurThread);
    إذا (get_رجسٹر(thread, cap_رجسٹر)) {
        arch_clean_invalidate_L1_caches(get_رجسٹر(thread, پیغام_Info_رجسٹر));
    } وإلا {
        arch_clean_invalidate_caches();
    }
#وإلا
    arch_clean_invalidate_caches();
#جے_انت
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(فراغ)
{
#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    إذا (ks_User_Log_بفر == 0) {
        userError("A user-level بفر has منزل be سیٹ before resetting benchmark.\
                Use se_L4_Benchmark_سیٹ_Log_بفر\n");
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, seL4_IllegalOperation);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_اشاریہ = 0;
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */

#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    گرہ_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(گرہ_STATE(ksIdleThread));
    گرہ_STATE(ksCurThread)->benchmark.ویلا_سارنی_شروع_ویلا = ksEnter;
    گرہ_STATE(ksCurThread)->benchmark.نمبر_schedules++;
    گرہ_STATE(benchmark_شروع_ویلا) = ksEnter;
    گرہ_STATE(benchmark_kernel_ویلا) = 0;
    گرہ_STATE(benchmark_kernel_نمبر_entries) = 0;
    گرہ_STATE(benchmark_kernel_نمبر_schedules) = 1;
    benchmark_arch_utilisation_reset();
#جے_انت /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, seL4_NoError);
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(فراغ)
{
#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    ks_Log_اشاریہ_Finalized = ks_Log_اشاریہ;
    سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, ks_Log_اشاریہ_Finalized);
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */

#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#جے_انت /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    أرجع EXCEPTION_NONE;
}

#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
exception_t handle_Sys_Benchmark_سیٹ_Log_بفر(فراغ)
{
    لفظ_t cptr_user_چوکھٹا = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
    إذا (benchmark_arch_map_log_بفر(cptr_user_چوکھٹا) != EXCEPTION_NONE) {
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, seL4_IllegalOperation);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, seL4_NoError);
    أرجع EXCEPTION_NONE;
}
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */

#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(فراغ)
{
    benchmark_track_utilisation_dump();
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(فراغ)
{
    لفظ_t tcb_cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
    lookupCap_ret_t lu_ret;
    لفظ_t cap_type;

    lu_ret = lookupCap(گرہ_STATE(ksCurThread), tcb_cptr);
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

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_دھاگے_Utilisation(فراغ)
{
    لکھو("{\n");
    لکھو("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (لفظ_t)(گرہ_STATE(benchmark_سرا_ویلا) - گرہ_STATE(benchmark_شروع_ویلا)));
    لکھو("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (لفظ_t) گرہ_STATE(benchmark_kernel_ویلا));
    لکھو("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (لفظ_t) گرہ_STATE(benchmark_kernel_نمبر_entries));
    لکھو("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (لفظ_t) گرہ_STATE(benchmark_kernel_نمبر_schedules));
    لکھو("  \"BENCHMARK_TCB_\": [\n");
    لكل (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        لکھو("    {\n");
        لکھو("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        لکھو("      \"UTILISATION\":%lu,\n", (لفظ_t) curr->benchmark.utilisation);
        لکھو("      \"NUMBER_SCHEDULES\":%lu,\n", (لفظ_t) curr->benchmark.نمبر_schedules);
        لکھو("      \"KERNEL_UTILISATION\":%lu,\n", (لفظ_t) curr->benchmark.kernel_utilisation);
        لکھو("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (لفظ_t) curr->benchmark.نمبر_kernel_entries);
        لکھو("    }");
        إذا (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا != عدم) {
            لکھو(",\n");
        } وإلا {
            لکھو("\n");
        }
    }
    لکھو("  ]\n}\n");
    أرجع EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_دھاگے_Utilisation(فراغ)
{
    لكل (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        benchmark_track_reset_utilisation(curr);
    }
    أرجع EXCEPTION_NONE;
}

#جے_انت /* CONFIG_DEBUG_BUILD */
#جے_انت /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#جے_انت /* CONFIG_ENABLE_BENCHMARKS */
