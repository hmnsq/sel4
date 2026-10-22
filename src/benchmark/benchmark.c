/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_ENABLE_BENCHMARKS

#ضمن <types.h>
#ضمن <mode/machine.h>
#ضمن <benchmark/benchmark.h>
#ضمن <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_ARM
    tcb_t *thread = ڳنڍ_STATE(ksCurThread);
    إذا (get_رجسٽر(thread, cap_رجسٽر)) {
        arch_clean_invalidate_L1_caches(get_رجسٽر(thread, نياپو_Info_رجسٽر));
    } وإلا {
        arch_clean_invalidate_caches();
    }
#وإلا
    arch_clean_invalidate_caches();
#جيڪڏهن_پورو
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
    إذا (ks_User_Log_بفر == 0) {
        userError("A user-level بفر has منزل be سيٽ before resetting benchmark.\
                Use se_L4_Benchmark_سيٽ_Log_بفر\n");
        سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر, seL4_IllegalOperation);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_اشاريو = 0;
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_UTILISATION
    ڳنڍ_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(ڳنڍ_STATE(ksIdleThread));
    ڳنڍ_STATE(ksCurThread)->benchmark.وقت_جدول_شروعات_وقت = ksEnter;
    ڳنڍ_STATE(ksCurThread)->benchmark.عدد_schedules++;
    ڳنڍ_STATE(benchmark_شروعات_وقت) = ksEnter;
    ڳنڍ_STATE(benchmark_kernel_وقت) = 0;
    ڳنڍ_STATE(benchmark_kernel_عدد_entries) = 0;
    ڳنڍ_STATE(benchmark_kernel_عدد_schedules) = 1;
    benchmark_arch_utilisation_reset();
#جيڪڏهن_پورو /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر, seL4_NoError);
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
    ks_Log_اشاريو_Finalized = ks_Log_اشاريو;
    سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر, ks_Log_اشاريو_Finalized);
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#جيڪڏهن_پورو /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
exception_t handle_Sys_Benchmark_سيٽ_Log_بفر(فراغ)
{
    لفظ_t cptr_user_چوکٺو = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
    إذا (benchmark_arch_map_log_بفر(cptr_user_چوکٺو) != EXCEPTION_NONE) {
        سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر, seL4_IllegalOperation);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر, seL4_NoError);
    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(فراغ)
{
    benchmark_track_utilisation_dump();
    أرجع EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(فراغ)
{
    لفظ_t tcb_cptr = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
    lookupCap_ret_t lu_ret;
    لفظ_t cap_type;

    lu_ret = lookupCap(ڳنڍ_STATE(ksCurThread), tcb_cptr);
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

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_ڌاڳا_Utilisation(فراغ)
{
    لکو("{\n");
    لکو("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (لفظ_t)(ڳنڍ_STATE(benchmark_پڇاڙي_وقت) - ڳنڍ_STATE(benchmark_شروعات_وقت)));
    لکو("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (لفظ_t) ڳنڍ_STATE(benchmark_kernel_وقت));
    لکو("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (لفظ_t) ڳنڍ_STATE(benchmark_kernel_عدد_entries));
    لکو("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (لفظ_t) ڳنڍ_STATE(benchmark_kernel_عدد_schedules));
    لکو("  \"BENCHMARK_TCB_\": [\n");
    لكل (tcb_t *curr = ڳنڍ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ايندڙ) {
        لکو("    {\n");
        لکو("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        لکو("      \"UTILISATION\":%lu,\n", (لفظ_t) curr->benchmark.utilisation);
        لکو("      \"NUMBER_SCHEDULES\":%lu,\n", (لفظ_t) curr->benchmark.عدد_schedules);
        لکو("      \"KERNEL_UTILISATION\":%lu,\n", (لفظ_t) curr->benchmark.kernel_utilisation);
        لکو("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (لفظ_t) curr->benchmark.عدد_kernel_entries);
        لکو("    }");
        إذا (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ايندڙ != عدم) {
            لکو(",\n");
        } وإلا {
            لکو("\n");
        }
    }
    لکو("  ]\n}\n");
    أرجع EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_ڌاڳا_Utilisation(فراغ)
{
    لكل (tcb_t *curr = ڳنڍ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ايندڙ) {
        benchmark_track_reset_utilisation(curr);
    }
    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
#جيڪڏهن_پورو /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#جيڪڏهن_پورو /* CONFIG_ENABLE_BENCHMARKS */
