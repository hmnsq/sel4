/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_ENABLE_BENCHMARKS

#అన్తర్భావ <types.h>
#అన్తర్భావ <mode/machine.h>
#అన్తర్భావ <benchmark/benchmark.h>
#అన్తర్భావ <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(శూన్య)
{
#నిర్వచితమైతే CONFIG_ARCH_ARM
    tcb_t *thread = కణుపు_STATE(ksCurThread);
    ఒకవేళ (get_రిజిస్టరు(thread, cap_రిజిస్టరు)) {
        arch_clean_invalidate_L1_caches(get_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు));
    } లేకపోతే {
        arch_clean_invalidate_caches();
    }
#లేకపోతే
    arch_clean_invalidate_caches();
#అయితే_ముగింపు
    తిరిగి EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(శూన్య)
{
#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
    ఒకవేళ (ks_User_Log_బఫరు == 0) {
        userError("A user-level బఫరు has గమ్యం be సమితి before resetting benchmark.\
                Use se_L4_Benchmark_సమితి_Log_బఫరు\n");
        సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు, seL4_IllegalOperation);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_సూచీ = 0;
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_UTILISATION
    కణుపు_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(కణుపు_STATE(ksIdleThread));
    కణుపు_STATE(ksCurThread)->benchmark.కాలపట్టిక_ప్రారంభం_సమయం = ksEnter;
    కణుపు_STATE(ksCurThread)->benchmark.సంఖ్య_schedules++;
    కణుపు_STATE(benchmark_ప్రారంభం_సమయం) = ksEnter;
    కణుపు_STATE(benchmark_kernel_సమయం) = 0;
    కణుపు_STATE(benchmark_kernel_సంఖ్య_entries) = 0;
    కణుపు_STATE(benchmark_kernel_సంఖ్య_schedules) = 1;
    benchmark_arch_utilisation_reset();
#అయితే_ముగింపు /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు, seL4_NoError);
    తిరిగి EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(శూన్య)
{
#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
    ks_Log_సూచీ_Finalized = ks_Log_సూచీ;
    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు, ks_Log_సూచీ_Finalized);
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#అయితే_ముగింపు /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
exception_t handle_Sys_Benchmark_సమితి_Log_బఫరు(శూన్య)
{
    మాట_t cptr_user_చట్రం = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
    ఒకవేళ (benchmark_arch_map_log_బఫరు(cptr_user_చట్రం) != EXCEPTION_NONE) {
        సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు, seL4_IllegalOperation);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు, seL4_NoError);
    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(శూన్య)
{
    benchmark_track_utilisation_dump();
    తిరిగి EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(శూన్య)
{
    మాట_t tcb_cptr = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
    lookupCap_ret_t lu_ret;
    మాట_t cap_type;

    lu_ret = lookupCap(కణుపు_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    ఒకవేళ (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        తిరిగి EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_దారాలు_Utilisation(శూన్య)
{
    రాయి("{\n");
    రాయి("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (మాట_t)(కణుపు_STATE(benchmark_చివర_సమయం) - కణుపు_STATE(benchmark_ప్రారంభం_సమయం)));
    రాయి("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (మాట_t) కణుపు_STATE(benchmark_kernel_సమయం));
    రాయి("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (మాట_t) కణుపు_STATE(benchmark_kernel_సంఖ్య_entries));
    రాయి("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (మాట_t) కణుపు_STATE(benchmark_kernel_సంఖ్య_schedules));
    రాయి("  \"BENCHMARK_TCB_\": [\n");
    కొరకు (tcb_t *curr = కణుపు_STATE(ksDebugTCBs); curr != ఖాళీ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_తదుపరి) {
        రాయి("    {\n");
        రాయి("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        రాయి("      \"UTILISATION\":%lu,\n", (మాట_t) curr->benchmark.utilisation);
        రాయి("      \"NUMBER_SCHEDULES\":%lu,\n", (మాట_t) curr->benchmark.సంఖ్య_schedules);
        రాయి("      \"KERNEL_UTILISATION\":%lu,\n", (మాట_t) curr->benchmark.kernel_utilisation);
        రాయి("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (మాట_t) curr->benchmark.సంఖ్య_kernel_entries);
        రాయి("    }");
        ఒకవేళ (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_తదుపరి != ఖాళీ) {
            రాయి(",\n");
        } లేకపోతే {
            రాయి("\n");
        }
    }
    రాయి("  ]\n}\n");
    తిరిగి EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_దారాలు_Utilisation(శూన్య)
{
    కొరకు (tcb_t *curr = కణుపు_STATE(ksDebugTCBs); curr != ఖాళీ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_తదుపరి) {
        benchmark_track_reset_utilisation(curr);
    }
    తిరిగి EXCEPTION_NONE;
}

#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
#అయితే_ముగింపు /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#అయితే_ముగింపు /* CONFIG_ENABLE_BENCHMARKS */
