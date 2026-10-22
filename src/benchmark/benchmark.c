/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_ENABLE_BENCHMARKS

#समावेश <types.h>
#समावेश <mode/machine.h>
#समावेश <benchmark/benchmark.h>
#समावेश <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(व्योम)
{
#अगर_घोषित CONFIG_ARCH_ARM
    tcb_t *thread = गाँठ_STATE(ksCurThread);
    अगर (getRegister(thread, capRegister)) {
        arch_clean_invalidate_L1_caches(getRegister(thread, संदेश_Info_Register));
    } अन्यथा {
        arch_clean_invalidate_caches();
    }
#अन्यथा
    arch_clean_invalidate_caches();
#पूर्ण_अगर
    वापस EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(व्योम)
{
#अगर_घोषित CONFIG_KERNEL_LOG_बफर
    अगर (ks_User_Log_बफर == 0) {
        userError("A user-level बफर has to be set before resetting benchmark.\
                Use se_L4_Benchmark_Set_Log_बफर\n");
        setRegister(गाँठ_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_सूचकांक = 0;
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */

#अगर_घोषित CONFIG_BENCHMARK_TRACK_UTILISATION
    गाँठ_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(गाँठ_STATE(ksIdleThread));
    गाँठ_STATE(ksCurThread)->benchmark.schedule_start_time = ksEnter;
    गाँठ_STATE(ksCurThread)->benchmark.संख्या_schedules++;
    गाँठ_STATE(benchmark_start_time) = ksEnter;
    गाँठ_STATE(benchmark_kernel_time) = 0;
    गाँठ_STATE(benchmark_kernel_संख्या_entries) = 0;
    गाँठ_STATE(benchmark_kernel_संख्या_schedules) = 1;
    benchmark_arch_utilisation_reset();
#पूर्ण_अगर /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    setRegister(गाँठ_STATE(ksCurThread), capRegister, seL4_NoError);
    वापस EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(व्योम)
{
#अगर_घोषित CONFIG_KERNEL_LOG_बफर
    ks_Log_सूचकांक_Finalized = ks_Log_सूचकांक;
    setRegister(गाँठ_STATE(ksCurThread), capRegister, ks_Log_सूचकांक_Finalized);
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */

#अगर_घोषित CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#पूर्ण_अगर /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_KERNEL_LOG_बफर
exception_t handle_Sys_Benchmark_Set_Log_बफर(व्योम)
{
    word_t cptr_userFrame = getRegister(गाँठ_STATE(ksCurThread), capRegister);
    अगर (benchmark_arch_map_log_बफर(cptr_userFrame) != EXCEPTION_NONE) {
        setRegister(गाँठ_STATE(ksCurThread), capRegister, seL4_IllegalOperation);
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setRegister(गाँठ_STATE(ksCurThread), capRegister, seL4_NoError);
    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */

#अगर_घोषित CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(व्योम)
{
    benchmark_track_utilisation_dump();
    वापस EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(व्योम)
{
    word_t tcb_cptr = getRegister(गाँठ_STATE(ksCurThread), capRegister);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(गाँठ_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    अगर (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        वापस EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_DEBUG_BUILD

exception_t handle_SysBenchmarkDumpAllThreadsUtilisation(व्योम)
{
    म_लिखो("{\n");
    म_लिखो("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(गाँठ_STATE(benchmark_end_time) - गाँठ_STATE(benchmark_start_time)));
    म_लिखो("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) गाँठ_STATE(benchmark_kernel_time));
    म_लिखो("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) गाँठ_STATE(benchmark_kernel_संख्या_entries));
    म_लिखो("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) गाँठ_STATE(benchmark_kernel_संख्या_schedules));
    म_लिखो("  \"BENCHMARK_TCB_\": [\n");
    क्रम (tcb_t *curr = गाँठ_STATE(ksDebugTCBs); curr != शून्य; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        म_लिखो("    {\n");
        म_लिखो("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        म_लिखो("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        म_लिखो("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.संख्या_schedules);
        म_लिखो("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        म_लिखो("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.संख्या_kernel_entries);
        म_लिखो("    }");
        अगर (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र != शून्य) {
            म_लिखो(",\n");
        } अन्यथा {
            म_लिखो("\n");
        }
    }
    म_लिखो("  ]\n}\n");
    वापस EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetAllThreadsUtilisation(व्योम)
{
    क्रम (tcb_t *curr = गाँठ_STATE(ksDebugTCBs); curr != शून्य; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        benchmark_track_reset_utilisation(curr);
    }
    वापस EXCEPTION_NONE;
}

#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
#पूर्ण_अगर /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#पूर्ण_अगर /* CONFIG_ENABLE_BENCHMARKS */
