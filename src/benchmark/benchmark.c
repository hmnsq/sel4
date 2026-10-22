/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_BENCHMARKS

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_ARM
    tcb_t *thread = ਗੰਢ_STATE(ksCurThread);
    ਜੇ (get_ਰਜਿਸਟਰ(thread, cap_ਰਜਿਸਟਰ)) {
        arch_clean_invalidate_L1_caches(get_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ));
    } ਨਹੀਂਤਾਂ {
        arch_clean_invalidate_caches();
    }
#ਨਹੀਂਤਾਂ
    arch_clean_invalidate_caches();
#ਜੇ_ਅੰਤ
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
    ਜੇ (ks_User_Log_ਬਫਰ == 0) {
        userError("A user-level ਬਫਰ has ਮੰਜ਼ਿਲ be ਸੈੱਟ before resetting benchmark.\
                Use se_L4_Benchmark_ਸੈੱਟ_Log_ਬਫਰ\n");
        ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ, seL4_IllegalOperation);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_ਸੂਚਕਾਂਕ = 0;
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_UTILISATION
    ਗੰਢ_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(ਗੰਢ_STATE(ksIdleThread));
    ਗੰਢ_STATE(ksCurThread)->benchmark.ਸਮਾਂ_ਸਾਰਣੀ_ਸ਼ੁਰੂ_ਸਮਾਂ = ksEnter;
    ਗੰਢ_STATE(ksCurThread)->benchmark.ਸੰਖਿਆ_schedules++;
    ਗੰਢ_STATE(benchmark_ਸ਼ੁਰੂ_ਸਮਾਂ) = ksEnter;
    ਗੰਢ_STATE(benchmark_kernel_ਸਮਾਂ) = 0;
    ਗੰਢ_STATE(benchmark_kernel_ਸੰਖਿਆ_entries) = 0;
    ਗੰਢ_STATE(benchmark_kernel_ਸੰਖਿਆ_schedules) = 1;
    benchmark_arch_utilisation_reset();
#ਜੇ_ਅੰਤ /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ, seL4_NoError);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
    ks_Log_ਸੂਚਕਾਂਕ_Finalized = ks_Log_ਸੂਚਕਾਂਕ;
    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ, ks_Log_ਸੂਚਕਾਂਕ_Finalized);
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#ਜੇ_ਅੰਤ /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
exception_t handle_Sys_Benchmark_ਸੈੱਟ_Log_ਬਫਰ(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t cptr_user_ਚੌਖਟਾ = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
    ਜੇ (benchmark_arch_map_log_ਬਫਰ(cptr_user_ਚੌਖਟਾ) != EXCEPTION_NONE) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ, seL4_IllegalOperation);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ, seL4_NoError);
    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(ਖਾਲੀ)
{
    benchmark_track_utilisation_dump();
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t tcb_cptr = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
    lookupCap_ret_t lu_ret;
    ਸ਼ਬਦ_t cap_type;

    lu_ret = lookupCap(ਗੰਢ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ਜੇ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        ਵਾਪਸ EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_ਧਾਗੇ_Utilisation(ਖਾਲੀ)
{
    ਲਿਖੋ("{\n");
    ਲਿਖੋ("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (ਸ਼ਬਦ_t)(ਗੰਢ_STATE(benchmark_ਸਿਰਾ_ਸਮਾਂ) - ਗੰਢ_STATE(benchmark_ਸ਼ੁਰੂ_ਸਮਾਂ)));
    ਲਿਖੋ("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (ਸ਼ਬਦ_t) ਗੰਢ_STATE(benchmark_kernel_ਸਮਾਂ));
    ਲਿਖੋ("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (ਸ਼ਬਦ_t) ਗੰਢ_STATE(benchmark_kernel_ਸੰਖਿਆ_entries));
    ਲਿਖੋ("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (ਸ਼ਬਦ_t) ਗੰਢ_STATE(benchmark_kernel_ਸੰਖਿਆ_schedules));
    ਲਿਖੋ("  \"BENCHMARK_TCB_\": [\n");
    ਲਈ (tcb_t *curr = ਗੰਢ_STATE(ksDebugTCBs); curr != ਨਿਰਾ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ਅਗਲਾ) {
        ਲਿਖੋ("    {\n");
        ਲਿਖੋ("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        ਲਿਖੋ("      \"UTILISATION\":%lu,\n", (ਸ਼ਬਦ_t) curr->benchmark.utilisation);
        ਲਿਖੋ("      \"NUMBER_SCHEDULES\":%lu,\n", (ਸ਼ਬਦ_t) curr->benchmark.ਸੰਖਿਆ_schedules);
        ਲਿਖੋ("      \"KERNEL_UTILISATION\":%lu,\n", (ਸ਼ਬਦ_t) curr->benchmark.kernel_utilisation);
        ਲਿਖੋ("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (ਸ਼ਬਦ_t) curr->benchmark.ਸੰਖਿਆ_kernel_entries);
        ਲਿਖੋ("    }");
        ਜੇ (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ਅਗਲਾ != ਨਿਰਾ) {
            ਲਿਖੋ(",\n");
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("\n");
        }
    }
    ਲਿਖੋ("  ]\n}\n");
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_ਧਾਗੇ_Utilisation(ਖਾਲੀ)
{
    ਲਈ (tcb_t *curr = ਗੰਢ_STATE(ksDebugTCBs); curr != ਨਿਰਾ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ਅਗਲਾ) {
        benchmark_track_reset_utilisation(curr);
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
#ਜੇ_ਅੰਤ /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#ਜੇ_ਅੰਤ /* CONFIG_ENABLE_BENCHMARKS */
