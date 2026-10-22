/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_ENABLE_BENCHMARKS

#شامل <types.h>
#شامل <mode/machine.h>
#شامل <benchmark/benchmark.h>
#شامل <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(خالی)
{
#اگر_تعریف_شدہ CONFIG_ARCH_ARM
    tcb_t *thread = گرہ_STATE(ksCurThread);
    اگر (get_رجسٹر(thread, cap_رجسٹر)) {
        arch_clean_invalidate_L1_caches(get_رجسٹر(thread, پیغام_Info_رجسٹر));
    } ورنہ {
        arch_clean_invalidate_caches();
    }
#ورنہ
    arch_clean_invalidate_caches();
#اگر_ختم
    واپس EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(خالی)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    اگر (ks_User_Log_بفر == 0) {
        userError("A user-level بفر has مقصد be سیٹ before resetting benchmark.\
                Use se_L4_Benchmark_سیٹ_Log_بفر\n");
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, seL4_IllegalOperation);
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_اشاریہ = 0;
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    گرہ_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(گرہ_STATE(ksIdleThread));
    گرہ_STATE(ksCurThread)->benchmark.جدول_وقت_آغاز_وقت = ksEnter;
    گرہ_STATE(ksCurThread)->benchmark.نمبر_schedules++;
    گرہ_STATE(benchmark_آغاز_وقت) = ksEnter;
    گرہ_STATE(benchmark_kernel_وقت) = 0;
    گرہ_STATE(benchmark_kernel_نمبر_entries) = 0;
    گرہ_STATE(benchmark_kernel_نمبر_schedules) = 1;
    benchmark_arch_utilisation_reset();
#اگر_ختم /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, seL4_NoError);
    واپس EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(خالی)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    ks_Log_اشاریہ_Finalized = ks_Log_اشاریہ;
    سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, ks_Log_اشاریہ_Finalized);
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#اگر_ختم /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
exception_t handle_Sys_Benchmark_سیٹ_Log_بفر(خالی)
{
    لفظ_t cptr_user_چوکھٹا = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
    اگر (benchmark_arch_map_log_بفر(cptr_user_چوکھٹا) != EXCEPTION_NONE) {
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, seL4_IllegalOperation);
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, seL4_NoError);
    واپس EXCEPTION_NONE;
}
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(خالی)
{
    benchmark_track_utilisation_dump();
    واپس EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(خالی)
{
    لفظ_t tcb_cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
    lookupCap_ret_t lu_ret;
    لفظ_t cap_type;

    lu_ret = lookupCap(گرہ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    اگر (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        واپس EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_دھاگے_Utilisation(خالی)
{
    لکھو("{\n");
    لکھو("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (لفظ_t)(گرہ_STATE(benchmark_انتہا_وقت) - گرہ_STATE(benchmark_آغاز_وقت)));
    لکھو("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (لفظ_t) گرہ_STATE(benchmark_kernel_وقت));
    لکھو("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (لفظ_t) گرہ_STATE(benchmark_kernel_نمبر_entries));
    لکھو("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (لفظ_t) گرہ_STATE(benchmark_kernel_نمبر_schedules));
    لکھو("  \"BENCHMARK_TCB_\": [\n");
    برائے (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != تہی; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        لکھو("    {\n");
        لکھو("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        لکھو("      \"UTILISATION\":%lu,\n", (لفظ_t) curr->benchmark.utilisation);
        لکھو("      \"NUMBER_SCHEDULES\":%lu,\n", (لفظ_t) curr->benchmark.نمبر_schedules);
        لکھو("      \"KERNEL_UTILISATION\":%lu,\n", (لفظ_t) curr->benchmark.kernel_utilisation);
        لکھو("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (لفظ_t) curr->benchmark.نمبر_kernel_entries);
        لکھو("    }");
        اگر (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا != تہی) {
            لکھو(",\n");
        } ورنہ {
            لکھو("\n");
        }
    }
    لکھو("  ]\n}\n");
    واپس EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_دھاگے_Utilisation(خالی)
{
    برائے (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != تہی; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        benchmark_track_reset_utilisation(curr);
    }
    واپس EXCEPTION_NONE;
}

#اگر_ختم /* CONFIG_DEBUG_BUILD */
#اگر_ختم /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#اگر_ختم /* CONFIG_ENABLE_BENCHMARKS */
