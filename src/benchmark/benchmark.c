/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_ENABLE_BENCHMARKS

#अन्तर्भाव <types.h>
#अन्तर्भाव <mode/machine.h>
#अन्तर्भाव <benchmark/benchmark.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>


exception_t handle_SysBenchmarkFlushCaches(शून्य)
{
#यदि_परिभाषितम् CONFIG_ARCH_ARM
    tcb_t *thread = ग्रन्थिः_STATE(ksCurThread);
    यदि (get_पञ्जिकाकोशः(thread, cap_पञ्जिकाकोशः)) {
        arch_clean_invalidate_L1_caches(get_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः));
    } अन्यथा {
        arch_clean_invalidate_caches();
    }
#अन्यथा
    arch_clean_invalidate_caches();
#यदि_अन्तः
    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetLog(शून्य)
{
#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
    यदि (ks_User_Log_बफर == 0) {
        userError("A user-level बफर has गन्तव्यम् be समुच्चयः before resetting benchmark.\
                Use se_L4_Benchmark_समुच्चयः_Log_बफर\n");
        समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः, seL4_IllegalOperation);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    ks_Log_सूचकांक = 0;
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_UTILISATION
    ग्रन्थिः_STATE(benchmark_log_utilisation_enabled) = true;
    benchmark_track_reset_utilisation(ग्रन्थिः_STATE(ksIdleThread));
    ग्रन्थिः_STATE(ksCurThread)->benchmark.कालसारणी_start_time = ksEnter;
    ग्रन्थिः_STATE(ksCurThread)->benchmark.सङ्ख्या_schedules++;
    ग्रन्थिः_STATE(benchmark_start_time) = ksEnter;
    ग्रन्थिः_STATE(benchmark_kernel_time) = 0;
    ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_entries) = 0;
    ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_schedules) = 1;
    benchmark_arch_utilisation_reset();
#यदि_अन्तः /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः, seL4_NoError);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkFinalizeLog(शून्य)
{
#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
    ks_Log_सूचकांक_Finalized = ks_Log_सूचकांक;
    समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः, ks_Log_सूचकांक_Finalized);
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_finalise();
#यदि_अन्तः /* CONFIG_BENCHMARK_TRACK_UTILISATION */

    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
exception_t handle_Sys_Benchmark_समुच्चयः_Log_बफर(शून्य)
{
    word_t cptr_userFrame = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
    यदि (benchmark_arch_map_log_बफर(cptr_userFrame) != EXCEPTION_NONE) {
        समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः, seL4_IllegalOperation);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः, seL4_NoError);
    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_UTILISATION

exception_t handle_SysBenchmarkGetThreadUtilisation(शून्य)
{
    benchmark_track_utilisation_dump();
    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_SysBenchmarkResetThreadUtilisation(शून्य)
{
    word_t tcb_cptr = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
    lookupCap_ret_t lu_ret;
    word_t cap_type;

    lu_ret = lookupCap(ग्रन्थिः_STATE(ksCurThread), tcb_cptr);
    /* ensure we got a TCB cap */
    cap_type = cap_get_capType(lu_ret.cap);
    यदि (cap_type != cap_thread_cap) {
        userError("SysBenchmarkResetThreadUtilisation: cap is not a TCB, halting");
        प्रत्यागम EXCEPTION_NONE;
    }

    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap));

    benchmark_track_reset_utilisation(tcb);
    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD

exception_t handle_Sys_Benchmark_Dump_All_तन्तवः_Utilisation(शून्य)
{
    प्रारूप_लिख("{\n");
    प्रारूप_लिख("  \"BENCHMARK_TOTAL_UTILISATION\":%lu,\n",
           (word_t)(ग्रन्थिः_STATE(benchmark_अन्तः_time) - ग्रन्थिः_STATE(benchmark_start_time)));
    प्रारूप_लिख("  \"BENCHMARK_TOTAL_KERNEL_UTILISATION\":%lu,\n", (word_t) ग्रन्थिः_STATE(benchmark_kernel_time));
    प्रारूप_लिख("  \"BENCHMARK_TOTAL_NUMBER_KERNEL_ENTRIES\":%lu,\n", (word_t) ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_entries));
    प्रारूप_लिख("  \"BENCHMARK_TOTAL_NUMBER_SCHEDULES\":%lu,\n", (word_t) ग्रन्थिः_STATE(benchmark_kernel_सङ्ख्या_schedules));
    प्रारूप_लिख("  \"BENCHMARK_TCB_\": [\n");
    प्रति (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        प्रारूप_लिख("    {\n");
        प्रारूप_लिख("      \"NAME\":\"%s\",\n", TCB_PTR_DEBUG_PTR(curr)->tcbName);
        प्रारूप_लिख("      \"UTILISATION\":%lu,\n", (word_t) curr->benchmark.utilisation);
        प्रारूप_लिख("      \"NUMBER_SCHEDULES\":%lu,\n", (word_t) curr->benchmark.सङ्ख्या_schedules);
        प्रारूप_लिख("      \"KERNEL_UTILISATION\":%lu,\n", (word_t) curr->benchmark.kernel_utilisation);
        प्रारूप_लिख("      \"NUMBER_KERNEL_ENTRIES\":%lu\n", (word_t) curr->benchmark.सङ्ख्या_kernel_entries);
        प्रारूप_लिख("    }");
        यदि (TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र != रिक्त) {
            प्रारूप_लिख(",\n");
        } अन्यथा {
            प्रारूप_लिख("\n");
        }
    }
    प्रारूप_लिख("  ]\n}\n");
    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_Sys_Benchmark_Reset_All_तन्तवः_Utilisation(शून्य)
{
    प्रति (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        benchmark_track_reset_utilisation(curr);
    }
    प्रत्यागम EXCEPTION_NONE;
}

#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
#यदि_अन्तः /* CONFIG_BENCHMARK_TRACK_UTILISATION */
#यदि_अन्तः /* CONFIG_ENABLE_BENCHMARKS */
