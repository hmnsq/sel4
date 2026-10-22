/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <benchmark/benchmark.h>
#अन्तर्भाव <arch/benchmark.h>
#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/faults.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/faulthandler.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <object/interrupt.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/traps.h>
#अन्तर्भाव <arch/machine.h>
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
#अन्तर्भाव <smp/ipi.h>
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
#अन्तर्भाव <arch/machine/capdl.h>
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
#अन्तर्भाव <mode/machine/debug.h>
#यदि_अन्तः

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#यदि_परिभाषितम् CONFIG_IRQ_REPORTING
स्थिर अन्तःस्थ शून्य check_व्यवधानम्(bool_t was_व्यवधानम्_प्रविष्टिः)
#अन्यथा
स्थिर अन्तःस्थ शून्य check_व्यवधानम्(शून्य)
#यदि_अन्तः
{
    irq_t irq;

    irq = get_सक्रिय_IRQ();
    यदि (IRQT_गन्तव्यम्_IRQ(irq) != IRQT_गन्तव्यम्_IRQ(irqInvalid)) {
        handle_व्यवधानम्(irq);
    } अन्यथा {
#यदि_परिभाषितम् CONFIG_IRQ_REPORTING
        यदि (was_व्यवधानम्_प्रविष्टिः) {
            userError("Spurious interrupt!");
        }
#यदि_अन्तः
        handleSpuriousIRQ();
    }
}

#यदि_अपरिभाषितम् CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#परिभाषय check_व्यवधानम्(was_व्यवधानम्_प्रविष्टिः) check_व्यवधानम्()
#यदि_अन्तः

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_व्यवधानम्_प्रविष्टिः(शून्य)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (SMP_TERNARY(clh_is_self_अन्तर्_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#यदि_अन्तः

    check_व्यवधानम्(/* was_interrupt_entry */ true);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (SMP_TERNARY(clh_is_self_अन्तर्_queue(), 1)) {
#यदि_अन्तः
        कालसारणी();
        activateThread();
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    }
#यदि_अन्तः

    प्रत्यागम EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#यदि_परिभाषितम् CONFIG_PRINTING
    यदि (w == SysDebugPutChar) {
        kernel_putchar(get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः));
        प्रत्यागम EXCEPTION_NONE;
    }
    यदि (w == SysDebugDumpScheduler) {
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#यदि_अन्तः
        प्रत्यागम EXCEPTION_NONE;
    }
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    यदि (w == SysDebugHalt) {
        tcb_t *अप्रयुक्तम् tptr = ग्रन्थिः_STATE(ksCurThread);
        प्रारूप_लिख("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    यदि (w == SysDebugSnapshot) {
        tcb_t *अप्रयुक्तम् tptr = ग्रन्थिः_STATE(ksCurThread);
        प्रारूप_लिख("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        प्रत्यागम EXCEPTION_NONE;
    }
    यदि (w == SysDebugCapIdentify) {
        word_t cptr = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ग्रन्थिः_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः, cap_type);
        प्रत्यागम EXCEPTION_NONE;
    }

    यदि (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ध्रुव वर्ण *name;
        word_t len;
        word_t cptr = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ग्रन्थिः_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        यदि (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ध्रुव वर्ण *)(lookupIPCBuffer(true, ग्रन्थिः_STATE(ksCurThread)) + 1);
        यदि (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_सन्देशः_Max_दीर्घता * परिमाण(word_t));
        यदि (len == se_L4_सन्देशः_Max_दीर्घता * परिमाण(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        समुच्चयः_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        प्रत्यागम EXCEPTION_NONE;
    }
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    यदि (w == SysDebugGetThreadAffinity) {
        word_t cptr = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ग्रन्थिः_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        यदि (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः, affinity);
        प्रत्यागम EXCEPTION_NONE;
    }
    यदि (w == SysDebugSendIPI) {
        प्रत्यागम handle_SysDebugSendIPI();
    }
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */

#यदि_परिभाषितम् CONFIG_DANGEROUS_संहिता_INJECTION
    यदि (w == Sys_Debug_सततम्) {
        ((शून्य (*)(शून्य *))get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः))((शून्य *)get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread),
                                                                                                  सन्देशः_Info_पञ्जिकाकोशः));
        प्रत्यागम EXCEPTION_NONE;
    }
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_X86_DANGEROUS_MSR
    यदि (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
        यदि (CONFIG_WORD_आकार == 32) {
            val = (uint64_t)getSyscallArg(0, रिक्त) | ((uint64_t)getSyscallArg(1, रिक्त) << 32);
        } अन्यथा {
            val = getSyscallArg(0, रिक्त);
        }
        x86_wrmsr(reg, val);
        प्रत्यागम EXCEPTION_NONE;
    } अन्यथा यदि (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
        val = x86_rdmsr(reg);
        पूर्णाङ्क अङ्कः = 1;
        यदि (CONFIG_WORD_आकार == 32) {
            समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), रिक्त, 0, val & 0xffffffff);
            समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), रिक्त, 1, val >> 32);
            अङ्कः++;
        } अन्यथा {
            समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), रिक्त, 0, val);
        }
        समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(se_L4_सन्देशः_Info_new(0, 0, 0, अङ्कः)));
        प्रत्यागम EXCEPTION_NONE;
    }
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ENABLE_BENCHMARKS
    विकल्प (w) {
    पक्ष SysBenchmarkFlushCaches:
        प्रत्यागम handle_SysBenchmarkFlushCaches();
    पक्ष SysBenchmarkResetLog:
        प्रत्यागम handle_SysBenchmarkResetLog();
    पक्ष SysBenchmarkFinalizeLog:
        प्रत्यागम handle_SysBenchmarkFinalizeLog();
#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
    पक्ष Sys_Benchmark_समुच्चयः_Log_बफर:
        प्रत्यागम handle_Sys_Benchmark_समुच्चयः_Log_बफर();
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */
#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_UTILISATION
    पक्ष SysBenchmarkGetThreadUtilisation:
        प्रत्यागम handle_SysBenchmarkGetThreadUtilisation();
    पक्ष SysBenchmarkResetThreadUtilisation:
        प्रत्यागम handle_SysBenchmarkResetThreadUtilisation();
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    पक्ष Sys_Benchmark_Dump_All_तन्तवः_Utilisation:
        प्रत्यागम handle_Sys_Benchmark_Dump_All_तन्तवः_Utilisation();
    पक्ष Sys_Benchmark_Reset_All_तन्तवः_Utilisation:
        प्रत्यागम handle_Sys_Benchmark_Reset_All_तन्तवः_Utilisation();
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
#यदि_अन्तः /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    पक्ष SysBenchmarkNullSyscall:
        प्रत्यागम EXCEPTION_NONE;
    स्वभाव:
        भङ्ग; /* syscall is not for benchmarking */
    } /* end switch(w) */
#यदि_अन्तः /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#यदि_परिभाषितम् CONFIG_समुच्चयः_TLS_आधारः_SELF
        यदि (w == Sys_समुच्चयः_TLSBase)
        {
            word_t tls_आधारः = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            प्रत्यागम Arch_समुच्चयः_TLSRegister(tls_आधारः);
        }
#यदि_अन्तः
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
    })

    कालसारणी();
    activateThread();

    प्रत्यागम EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
    })
    कालसारणी();
    activateThread();

    प्रत्यागम EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t स्थितिः = handleVMFault(ग्रन्थिः_STATE(ksCurThread), vm_faultType);
        यदि (स्थितिः != EXCEPTION_NONE)
        {
            handleFault(ग्रन्थिः_STATE(ksCurThread));
        }
    })

    कालसारणी();
    activateThread();

    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#अन्यथा
स्थिर exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#यदि_अन्तः
{
    se_L4_सन्देशः_Info_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *बफर;
    exception_t स्थितिः;
    word_t दीर्घता;
    tcb_t *thread;

    thread = ग्रन्थिः_STATE(ksCurThread);

    info = सन्देशः_Info_From_Word(get_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः));
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    cptr_t cptr = get_पञ्जिकाकोशः(thread, cap_पञ्जिकाकोशः);
#यदि_अन्तः

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        यदि (isBlocking) {
            handleFault(thread);
        }

        प्रत्यागम EXCEPTION_NONE;
    }

    बफर = lookupIPCBuffer(false, thread);

    स्थितिः = lookupExtraCaps(thread, बफर, info);

    यदि (unlikely(स्थितिः != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        यदि (isBlocking) {
            handleFault(thread);
        }
        प्रत्यागम EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    दीर्घता = se_L4_सन्देशः_Info_get_दीर्घता(info);
    यदि (unlikely(दीर्घता > n_सन्देशः_Registers && !बफर)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_सन्देशः_Registers);
        दीर्घता = n_सन्देशः_Registers;
    }
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    स्थितिः = decodeInvocation(se_L4_सन्देशः_Info_get_label(info), दीर्घता,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, बफर);
#अन्यथा
    स्थितिः = decodeInvocation(se_L4_सन्देशः_Info_get_label(info), दीर्घता,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, बफर);
#यदि_अन्तः

    यदि (unlikely(स्थितिः == EXCEPTION_PREEMPTED)) {
        प्रत्यागम स्थितिः;
    }

    यदि (unlikely(स्थितिः == EXCEPTION_SYSCALL_ERROR)) {
        यदि (isCall) {
            प्रत्युत्तरम्_From_Kernel_error(thread);
        }
        प्रत्यागम EXCEPTION_NONE;
    }

    यदि (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        यदि (isCall) {
            प्रत्युत्तरम्_From_Kernel_success_रिक्तानि(thread);
        }
        समुच्चयः_Thread_State(thread, ThreadState_Running);
    }

    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ lookupCap_ret_t lookup_प्रत्युत्तरम्(शून्य)
{
    word_t प्रत्युत्तरम्_CPtr = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), प्रत्युत्तरम्_पञ्जिकाकोशः);
    lookupCap_ret_t lu_ret = lookupCap(ग्रन्थिः_STATE(ksCurThread), प्रत्युत्तरम्_CPtr);
    यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(प्रत्युत्तरम्_CPtr, true);
        प्रत्यागम lu_ret;
    }

    यदि (unlikely(cap_get_capType(lu_ret.cap) != cap_प्रत्युत्तरम्_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(प्रत्युत्तरम्_CPtr, true);
        lu_ret.स्थितिः = EXCEPTION_FAULT;
        प्रत्यागम lu_ret;
    }

    प्रत्यागम lu_ret;
}
#अन्यथा
स्थिर शून्य handle_प्रत्युत्तरम्(शून्य)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    विकल्प (cap_get_capType(callerCap)) {
    पक्ष cap_प्रत्युत्तरम्_cap: {
        tcb_t *caller;

        यदि (cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Master(callerCap)) {
            भङ्ग;
        }
        caller = TCB_PTR(cap_प्रत्युत्तरम्_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        प्रतिज्ञा(caller != ग्रन्थिः_STATE(ksCurThread));
        do_प्रत्युत्तरम्_Transfer(ग्रन्थिः_STATE(ksCurThread), caller, callerSlot,
                        cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Can_Grant(callerCap));
        प्रत्यागम;
    }

    पक्ष cap_null_cap:
        /* Do nothing when no caller is pending */
        प्रत्यागम;

    स्वभाव:
        भङ्ग;
    }

    fail("handleReply: invalid caller cap");
}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर शून्य handleRecv(bool_t isBlocking, bool_t can_प्रत्युत्तरम्)
#अन्यथा
स्थिर शून्य handleRecv(bool_t isBlocking)
#यदि_अन्तः
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);

    lu_ret = lookupCap(ग्रन्थिः_STATE(ksCurThread), epCPtr);

    यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
        प्रत्यागम;
    }

    विकल्प (cap_get_capType(lu_ret.cap)) {
    पक्ष cap_endpoint_cap:
        यदि (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ग्रन्थिः_STATE(ksCurThread));
            भङ्ग;
        }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t प्रत्युत्तरम्_cap = cap_null_cap_new();
        यदि (can_प्रत्युत्तरम्) {
            lu_ret = lookup_प्रत्युत्तरम्();
            यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(ग्रन्थिः_STATE(ksCurThread));
                प्रत्यागम;
            } अन्यथा {
                प्रत्युत्तरम्_cap = lu_ret.cap;
            }
        }
        receiveIPC(ग्रन्थिः_STATE(ksCurThread), ep_cap, isBlocking, प्रत्युत्तरम्_cap);
#अन्यथा
        deleteCallerCap(ग्रन्थिः_STATE(ksCurThread));
        receiveIPC(ग्रन्थिः_STATE(ksCurThread), lu_ret.cap, isBlocking);
#यदि_अन्तः
        भङ्ग;

    पक्ष cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        यदि (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != ग्रन्थिः_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ग्रन्थिः_STATE(ksCurThread));
            भङ्ग;
        }

        receiveSignal(ग्रन्थिः_STATE(ksCurThread), lu_ret.cap, isBlocking);
        भङ्ग;
    }
    स्वभाव:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
        भङ्ग;
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य mcsPreemptionPoint(शून्य)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    यदि (isSchedulable(ग्रन्थिः_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } अन्यथा यदि (sc_सक्रिय(ग्रन्थिः_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(ग्रन्थिः_STATE(ksConsumed), false);
    } अन्यथा {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        ग्रन्थिः_STATE(ksConsumed) = 0;
    }
}
#अन्यथा
#परिभाषय handleRecv(isBlocking, can_प्रत्युत्तरम्) handleRecv(isBlocking)
#परिभाषय mcsPreemptionPoint()
#परिभाषय handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#यदि_अन्तः

स्थिर शून्य handleYield(शून्य)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = ग्रन्थिः_STATE(ksCurSC)->scConsumed + ग्रन्थिः_STATE(ksConsumed);
    refill_t शीर्ष = *refill_शीर्ष(ग्रन्थिः_STATE(ksCurSC));
    chargeBudget(शीर्ष.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    ग्रन्थिः_STATE(ksCurSC)->scConsumed = consumed;
#अन्यथा
    tcb_Sched_पङ्क्तेः_निष्कासय(ग्रन्थिः_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#यदि_अन्तः
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        विकल्प (syscall)
        {
        पक्ष SysSend:
            ret = handleInvocation(false, true, false, false, get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः));
            यदि (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                check_व्यवधानम्(/* was_interrupt_entry */ false);
            }

            भङ्ग;

        पक्ष SysNBSend:
            ret = handleInvocation(false, false, false, false, get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः));
            यदि (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                check_व्यवधानम्(/* was_interrupt_entry */ false);
            }
            भङ्ग;

        पक्ष SysCall:
            ret = handleInvocation(true, true, true, false, get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः));
            यदि (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                check_व्यवधानम्(/* was_interrupt_entry */ false);
            }
            भङ्ग;

        पक्ष SysRecv:
            handleRecv(true, true);
            भङ्ग;
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
        पक्ष Sys_प्रत्युत्तरम्:
            handle_प्रत्युत्तरम्();
            भङ्ग;

        पक्ष Sys_प्रत्युत्तरम्_Recv:
            handle_प्रत्युत्तरम्();
            handleRecv(true, true);
            भङ्ग;

#अन्यथा /* CONFIG_KERNEL_MCS */
        पक्ष SysWait:
            handleRecv(true, false);
            भङ्ग;

        पक्ष SysNBWait:
            handleRecv(false, false);
            भङ्ग;
        पक्ष Sys_प्रत्युत्तरम्_Recv: {
            cptr_t प्रत्युत्तरम् = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), प्रत्युत्तरम्_पञ्जिकाकोशः);
            ret = handleInvocation(false, false, true, true, प्रत्युत्तरम्);
            /* reply is not preemptible, but to ease verification we check explicitly */
            यदि (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                check_व्यवधानम्(/* was_interrupt_entry */ false);
                भङ्ग;
            }
            handleRecv(true, true);
            भङ्ग;
        }

        पक्ष SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            यदि (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                check_व्यवधानम्(/* was_interrupt_entry */ false);
                भङ्ग;
            }
            handleRecv(true, true);
            भङ्ग;
        }

        पक्ष SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), प्रत्युत्तरम्_पञ्जिकाकोशः));
            यदि (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                check_व्यवधानम्(/* was_interrupt_entry */ false);
                भङ्ग;
            }
            handleRecv(true, false);
            भङ्ग;
#यदि_अन्तः
        पक्ष SysNBRecv:
            handleRecv(false, true);
            भङ्ग;

        पक्ष SysYield:
            handleYield();
            भङ्ग;

        स्वभाव:
            fail("Invalid syscall");
        }

    })

    कालसारणी();
    activateThread();

    प्रत्यागम EXCEPTION_NONE;
}
