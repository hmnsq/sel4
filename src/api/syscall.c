/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <benchmark/benchmark.h>
#समावेश <arch/benchmark.h>
#समावेश <benchmark/benchmark_track.h>
#समावेश <benchmark/benchmark_utilisation.h>
#समावेश <api/syscall.h>
#समावेश <api/failures.h>
#समावेश <api/faults.h>
#समावेश <kernel/cspace.h>
#समावेश <kernel/faulthandler.h>
#समावेश <kernel/thread.h>
#समावेश <kernel/vspace.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/hardware.h>
#समावेश <object/interrupt.h>
#समावेश <model/statedata.h>
#समावेश <string.h>
#समावेश <kernel/traps.h>
#समावेश <arch/machine.h>
#अगर_घोषित ENABLE_SMP_SUPPORT
#समावेश <smp/ipi.h>
#पूर्ण_अगर
#अगर_घोषित CONFIG_DEBUG_BUILD
#समावेश <arch/machine/capdl.h>
#पूर्ण_अगर
#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
#समावेश <mode/machine/debug.h>
#पूर्ण_अगर

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#अगर_घोषित CONFIG_IRQ_REPORTING
अटल अंतरभूत व्योम checkInterrupt(bool_t was_interrupt_entry)
#अन्यथा
अटल अंतरभूत व्योम checkInterrupt(व्योम)
#पूर्ण_अगर
{
    irq_t irq;

    irq = getActiveIRQ();
    अगर (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } अन्यथा {
#अगर_घोषित CONFIG_IRQ_REPORTING
        अगर (was_interrupt_entry) {
            userError("Spurious interrupt!");
        }
#पूर्ण_अगर
        handleSpuriousIRQ();
    }
}

#अगर_अघोषित CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#घोषणा checkInterrupt(was_interrupt_entry) checkInterrupt()
#पूर्ण_अगर

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handleInterruptEntry(व्योम)
{
#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#पूर्ण_अगर

    checkInterrupt(/* was_interrupt_entry */ true);

#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#पूर्ण_अगर
        schedule();
        activateThread();
#अगर_घोषित CONFIG_KERNEL_MCS
    }
#पूर्ण_अगर

    वापस EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#अगर_घोषित CONFIG_PRINTING
    अगर (w == SysDebugPutChar) {
        kernel_putchar(getRegister(गाँठ_STATE(ksCurThread), capRegister));
        वापस EXCEPTION_NONE;
    }
    अगर (w == SysDebugDumpScheduler) {
#अगर_घोषित CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#पूर्ण_अगर
        वापस EXCEPTION_NONE;
    }
#पूर्ण_अगर
#अगर_घोषित CONFIG_DEBUG_BUILD
    अगर (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = गाँठ_STATE(ksCurThread);
        म_लिखो("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    अगर (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = गाँठ_STATE(ksCurThread);
        म_लिखो("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        वापस EXCEPTION_NONE;
    }
    अगर (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(गाँठ_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(गाँठ_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(गाँठ_STATE(ksCurThread), capRegister, cap_type);
        वापस EXCEPTION_NONE;
    }

    अगर (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        स्थिर अक्षर *name;
        word_t len;
        word_t cptr = getRegister(गाँठ_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(गाँठ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        अगर (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (स्थिर अक्षर *)(lookupIPCBuffer(true, गाँठ_STATE(ksCurThread)) + 1);
        अगर (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_संदेश_Max_लंबाई * माप(word_t));
        अगर (len == se_L4_संदेश_Max_लंबाई * माप(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        वापस EXCEPTION_NONE;
    }
#अगर_घोषित ENABLE_SMP_SUPPORT
    अगर (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(गाँठ_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(गाँठ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        अगर (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(गाँठ_STATE(ksCurThread), capRegister, affinity);
        वापस EXCEPTION_NONE;
    }
    अगर (w == SysDebugSendIPI) {
        वापस handle_SysDebugSendIPI();
    }
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */

#अगर_घोषित CONFIG_DANGEROUS_CODE_INJECTION
    अगर (w == SysDebugRun) {
        ((व्योम (*)(व्योम *))getRegister(गाँठ_STATE(ksCurThread), capRegister))((व्योम *)getRegister(गाँठ_STATE(ksCurThread),
                                                                                                  संदेश_Info_Register));
        वापस EXCEPTION_NONE;
    }
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_X86_DANGEROUS_MSR
    अगर (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(गाँठ_STATE(ksCurThread), capRegister);
        अगर (CONFIG_WORD_आकार == 32) {
            val = (uint64_t)getSyscallArg(0, शून्य) | ((uint64_t)getSyscallArg(1, शून्य) << 32);
        } अन्यथा {
            val = getSyscallArg(0, शून्य);
        }
        x86_wrmsr(reg, val);
        वापस EXCEPTION_NONE;
    } अन्यथा अगर (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(गाँठ_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        पूर्णांक अंक = 1;
        अगर (CONFIG_WORD_आकार == 32) {
            setMR(गाँठ_STATE(ksCurThread), शून्य, 0, val & 0xffffffff);
            setMR(गाँठ_STATE(ksCurThread), शून्य, 1, val >> 32);
            अंक++;
        } अन्यथा {
            setMR(गाँठ_STATE(ksCurThread), शून्य, 0, val);
        }
        setRegister(गाँठ_STATE(ksCurThread), संदेश_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, अंक)));
        वापस EXCEPTION_NONE;
    }
#पूर्ण_अगर

#अगर_घोषित CONFIG_ENABLE_BENCHMARKS
    चयन (w) {
    हाल SysBenchmarkFlushCaches:
        वापस handle_SysBenchmarkFlushCaches();
    हाल SysBenchmarkResetLog:
        वापस handle_SysBenchmarkResetLog();
    हाल SysBenchmarkFinalizeLog:
        वापस handle_SysBenchmarkFinalizeLog();
#अगर_घोषित CONFIG_KERNEL_LOG_बफर
    हाल Sys_Benchmark_Set_Log_बफर:
        वापस handle_Sys_Benchmark_Set_Log_बफर();
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */
#अगर_घोषित CONFIG_BENCHMARK_TRACK_UTILISATION
    हाल SysBenchmarkGetThreadUtilisation:
        वापस handle_SysBenchmarkGetThreadUtilisation();
    हाल SysBenchmarkResetThreadUtilisation:
        वापस handle_SysBenchmarkResetThreadUtilisation();
#अगर_घोषित CONFIG_DEBUG_BUILD
    हाल SysBenchmarkDumpAllThreadsUtilisation:
        वापस handle_SysBenchmarkDumpAllThreadsUtilisation();
    हाल SysBenchmarkResetAllThreadsUtilisation:
        वापस handle_SysBenchmarkResetAllThreadsUtilisation();
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
#पूर्ण_अगर /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    हाल SysBenchmarkNullSyscall:
        वापस EXCEPTION_NONE;
    शेष:
        अवरोध; /* syscall is not for benchmarking */
    } /* end switch(w) */
#पूर्ण_अगर /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#अगर_घोषित CONFIG_SET_TLS_BASE_SELF
        अगर (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(गाँठ_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            वापस Arch_setTLSRegister(tls_base);
        }
#पूर्ण_अगर
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(गाँठ_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    वापस EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(गाँठ_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    वापस EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(गाँठ_STATE(ksCurThread), vm_faultType);
        अगर (status != EXCEPTION_NONE)
        {
            handleFault(गाँठ_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#अन्यथा
अटल exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#पूर्ण_अगर
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *बफर;
    exception_t status;
    word_t लंबाई;
    tcb_t *thread;

    thread = गाँठ_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, संदेश_Info_Register));
#अगर_अघोषित CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#पूर्ण_अगर

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        अगर (isBlocking) {
            handleFault(thread);
        }

        वापस EXCEPTION_NONE;
    }

    बफर = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, बफर, info);

    अगर (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        अगर (isBlocking) {
            handleFault(thread);
        }
        वापस EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    लंबाई = se_L4_Message_Info_get_लंबाई(info);
    अगर (unlikely(लंबाई > n_संदेश_Registers && !बफर)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_संदेश_Registers);
        लंबाई = n_संदेश_Registers;
    }
#अगर_घोषित CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), लंबाई,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, बफर);
#अन्यथा
    status = decodeInvocation(seL4_MessageInfo_get_label(info), लंबाई,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, बफर);
#पूर्ण_अगर

    अगर (unlikely(status == EXCEPTION_PREEMPTED)) {
        वापस status;
    }

    अगर (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        अगर (isCall) {
            replyFromKernel_error(thread);
        }
        वापस EXCEPTION_NONE;
    }

    अगर (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        अगर (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल अंतरभूत lookupCap_ret_t lookupReply(व्योम)
{
    word_t replyCPtr = getRegister(गाँठ_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(गाँठ_STATE(ksCurThread), replyCPtr);
    अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        वापस lu_ret;
    }

    अगर (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        वापस lu_ret;
    }

    वापस lu_ret;
}
#अन्यथा
अटल व्योम handleReply(व्योम)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    चयन (cap_get_capType(callerCap)) {
    हाल cap_reply_cap: {
        tcb_t *caller;

        अगर (cap_reply_cap_get_capReplyMaster(callerCap)) {
            अवरोध;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        निश्चित(caller != गाँठ_STATE(ksCurThread));
        doReplyTransfer(गाँठ_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        वापस;
    }

    हाल cap_null_cap:
        /* Do nothing when no caller is pending */
        वापस;

    शेष:
        अवरोध;
    }

    fail("handleReply: invalid caller cap");
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_MCS
अटल व्योम handleRecv(bool_t isBlocking, bool_t canReply)
#अन्यथा
अटल व्योम handleRecv(bool_t isBlocking)
#पूर्ण_अगर
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(गाँठ_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(गाँठ_STATE(ksCurThread), epCPtr);

    अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(गाँठ_STATE(ksCurThread));
        वापस;
    }

    चयन (cap_get_capType(lu_ret.cap)) {
    हाल cap_endpoint_cap:
        अगर (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(गाँठ_STATE(ksCurThread));
            अवरोध;
        }

#अगर_घोषित CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        अगर (canReply) {
            lu_ret = lookupReply();
            अगर (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(गाँठ_STATE(ksCurThread));
                वापस;
            } अन्यथा {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(गाँठ_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#अन्यथा
        deleteCallerCap(गाँठ_STATE(ksCurThread));
        receiveIPC(गाँठ_STATE(ksCurThread), lu_ret.cap, isBlocking);
#पूर्ण_अगर
        अवरोध;

    हाल cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        अगर (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != गाँठ_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(गाँठ_STATE(ksCurThread));
            अवरोध;
        }

        receiveSignal(गाँठ_STATE(ksCurThread), lu_ret.cap, isBlocking);
        अवरोध;
    }
    शेष:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(गाँठ_STATE(ksCurThread));
        अवरोध;
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल अंतरभूत व्योम mcsPreemptionPoint(व्योम)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    अगर (isSchedulable(गाँठ_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } अन्यथा अगर (sc_active(गाँठ_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(गाँठ_STATE(ksConsumed), false);
    } अन्यथा {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        गाँठ_STATE(ksConsumed) = 0;
    }
}
#अन्यथा
#घोषणा handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#घोषणा mcsPreemptionPoint()
#घोषणा handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#पूर्ण_अगर

अटल व्योम handleYield(व्योम)
{
#अगर_घोषित CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = गाँठ_STATE(ksCurSC)->scConsumed + गाँठ_STATE(ksConsumed);
    refill_t शीर्ष = *refill_शीर्ष(गाँठ_STATE(ksCurSC));
    chargeBudget(शीर्ष.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    गाँठ_STATE(ksCurSC)->scConsumed = consumed;
#अन्यथा
    tcbSchedDequeue(गाँठ_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#पूर्ण_अगर
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        चयन (syscall)
        {
        हाल SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(गाँठ_STATE(ksCurThread), capRegister));
            अगर (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            अवरोध;

        हाल SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(गाँठ_STATE(ksCurThread), capRegister));
            अगर (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            अवरोध;

        हाल SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(गाँठ_STATE(ksCurThread), capRegister));
            अगर (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            अवरोध;

        हाल SysRecv:
            handleRecv(true, true);
            अवरोध;
#अगर_अघोषित CONFIG_KERNEL_MCS
        हाल SysReply:
            handleReply();
            अवरोध;

        हाल SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            अवरोध;

#अन्यथा /* CONFIG_KERNEL_MCS */
        हाल SysWait:
            handleRecv(true, false);
            अवरोध;

        हाल SysNBWait:
            handleRecv(false, false);
            अवरोध;
        हाल SysReplyRecv: {
            cptr_t reply = getRegister(गाँठ_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            अगर (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                अवरोध;
            }
            handleRecv(true, true);
            अवरोध;
        }

        हाल SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            अगर (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                अवरोध;
            }
            handleRecv(true, true);
            अवरोध;
        }

        हाल SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(गाँठ_STATE(ksCurThread), replyRegister));
            अगर (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                अवरोध;
            }
            handleRecv(true, false);
            अवरोध;
#पूर्ण_अगर
        हाल SysNBRecv:
            handleRecv(false, true);
            अवरोध;

        हाल SysYield:
            handleYield();
            अवरोध;

        शेष:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    वापस EXCEPTION_NONE;
}
