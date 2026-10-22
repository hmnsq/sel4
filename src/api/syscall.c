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
#ifdef ENABLE_SMP_SUPPORT
#अन्तर्भाव <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#अन्तर्भाव <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#अन्तर्भाव <mode/machine/debug.h>
#endif

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#ifdef CONFIG_IRQ_REPORTING
स्थिर अन्तःस्थ शून्य checkInterrupt(bool_t was_interrupt_entry)
#अञ्ञथा
स्थिर अन्तःस्थ शून्य checkInterrupt(शून्य)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    सचे (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } अञ्ञथा {
#ifdef CONFIG_IRQ_REPORTING
        सचे (was_interrupt_entry) {
            userError("Spurious interrupt!");
        }
#endif
        handleSpuriousIRQ();
    }
}

#ifndef CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#define checkInterrupt(was_interrupt_entry) checkInterrupt()
#endif

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handleInterruptEntry(शून्य)
{
#ifdef CONFIG_KERNEL_MCS
    सचे (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    सचे (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    पटिनिवत्त EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    सचे (w == SysDebugPutChar) {
        kernel_putchar(getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister));
        पटिनिवत्त EXCEPTION_NONE;
    }
    सचे (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        पटिनिवत्त EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    सचे (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = ग्रन्थिः_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    सचे (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = ग्रन्थिः_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        पटिनिवत्त EXCEPTION_NONE;
    }
    सचे (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ग्रन्थिः_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(ग्रन्थिः_STATE(ksCurThread), capRegister, cap_type);
        पटिनिवत्त EXCEPTION_NONE;
    }

    सचे (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ध्रुव वर्ण *name;
        word_t len;
        word_t cptr = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ग्रन्थिः_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        सचे (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ध्रुव वर्ण *)(lookupIPCBuffer(true, ग्रन्थिः_STATE(ksCurThread)) + 1);
        सचे (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_सन्देशः_Max_Length * परिमाण(word_t));
        सचे (len == se_L4_सन्देशः_Max_Length * परिमाण(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        पटिनिवत्त EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    सचे (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ग्रन्थिः_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        सचे (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(ग्रन्थिः_STATE(ksCurThread), capRegister, affinity);
        पटिनिवत्त EXCEPTION_NONE;
    }
    सचे (w == SysDebugSendIPI) {
        पटिनिवत्त handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    सचे (w == SysDebugRun) {
        ((शून्य (*)(शून्य *))getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister))((शून्य *)getRegister(ग्रन्थिः_STATE(ksCurThread),
                                                                                                  सन्देशः_Info_Register));
        पटिनिवत्त EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    सचे (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);
        सचे (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, रिक्त) | ((uint64_t)getSyscallArg(1, रिक्त) << 32);
        } अञ्ञथा {
            val = getSyscallArg(0, रिक्त);
        }
        x86_wrmsr(reg, val);
        पटिनिवत्त EXCEPTION_NONE;
    } अञ्ञथा सचे (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        पूर्णाङ्क अङ्कः = 1;
        सचे (CONFIG_WORD_SIZE == 32) {
            setMR(ग्रन्थिः_STATE(ksCurThread), रिक्त, 0, val & 0xffffffff);
            setMR(ग्रन्थिः_STATE(ksCurThread), रिक्त, 1, val >> 32);
            अङ्कः++;
        } अञ्ञथा {
            setMR(ग्रन्थिः_STATE(ksCurThread), रिक्त, 0, val);
        }
        setRegister(ग्रन्थिः_STATE(ksCurThread), सन्देशः_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, अङ्कः)));
        पटिनिवत्त EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    विकल्प (w) {
    पक्ष SysBenchmarkFlushCaches:
        पटिनिवत्त handle_SysBenchmarkFlushCaches();
    पक्ष SysBenchmarkResetLog:
        पटिनिवत्त handle_SysBenchmarkResetLog();
    पक्ष SysBenchmarkFinalizeLog:
        पटिनिवत्त handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    पक्ष SysBenchmarkSetLogBuffer:
        पटिनिवत्त handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    पक्ष SysBenchmarkGetThreadUtilisation:
        पटिनिवत्त handle_SysBenchmarkGetThreadUtilisation();
    पक्ष SysBenchmarkResetThreadUtilisation:
        पटिनिवत्त handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    पक्ष SysBenchmarkDumpAllThreadsUtilisation:
        पटिनिवत्त handle_SysBenchmarkDumpAllThreadsUtilisation();
    पक्ष SysBenchmarkResetAllThreadsUtilisation:
        पटिनिवत्त handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    पक्ष SysBenchmarkNullSyscall:
        पटिनिवत्त EXCEPTION_NONE;
    स्वभाव:
        भङ्ग; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        सचे (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            पटिनिवत्त Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    पटिनिवत्त EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    पटिनिवत्त EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(ग्रन्थिः_STATE(ksCurThread), vm_faultType);
        सचे (status != EXCEPTION_NONE)
        {
            handleFault(ग्रन्थिः_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    पटिनिवत्त EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
स्थिर exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#अञ्ञथा
स्थिर exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = ग्रन्थिः_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, सन्देशः_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    सचे (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        सचे (isBlocking) {
            handleFault(thread);
        }

        पटिनिवत्त EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    सचे (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        सचे (isBlocking) {
            handleFault(thread);
        }
        पटिनिवत्त EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    सचे (unlikely(length > n_सन्देशः_Registers && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_सन्देशः_Registers);
        length = n_सन्देशः_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#अञ्ञथा
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    सचे (unlikely(status == EXCEPTION_PREEMPTED)) {
        पटिनिवत्त status;
    }

    सचे (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        सचे (isCall) {
            replyFromKernel_error(thread);
        }
        पटिनिवत्त EXCEPTION_NONE;
    }

    सचे (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        सचे (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    पटिनिवत्त EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ lookupCap_ret_t lookupReply(शून्य)
{
    word_t replyCPtr = getRegister(ग्रन्थिः_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(ग्रन्थिः_STATE(ksCurThread), replyCPtr);
    सचे (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        पटिनिवत्त lu_ret;
    }

    सचे (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        पटिनिवत्त lu_ret;
    }

    पटिनिवत्त lu_ret;
}
#अञ्ञथा
स्थिर शून्य handleReply(शून्य)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    विकल्प (cap_get_capType(callerCap)) {
    पक्ष cap_reply_cap: {
        tcb_t *caller;

        सचे (cap_reply_cap_get_capReplyMaster(callerCap)) {
            भङ्ग;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        प्रतिज्ञा(caller != ग्रन्थिः_STATE(ksCurThread));
        doReplyTransfer(ग्रन्थिः_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        पटिनिवत्त;
    }

    पक्ष cap_null_cap:
        /* Do nothing when no caller is pending */
        पटिनिवत्त;

    स्वभाव:
        भङ्ग;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
स्थिर शून्य handleRecv(bool_t isBlocking, bool_t canReply)
#अञ्ञथा
स्थिर शून्य handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(ग्रन्थिः_STATE(ksCurThread), epCPtr);

    सचे (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
        पटिनिवत्त;
    }

    विकल्प (cap_get_capType(lu_ret.cap)) {
    पक्ष cap_endpoint_cap:
        सचे (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ग्रन्थिः_STATE(ksCurThread));
            भङ्ग;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        सचे (canReply) {
            lu_ret = lookupReply();
            सचे (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(ग्रन्थिः_STATE(ksCurThread));
                पटिनिवत्त;
            } अञ्ञथा {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(ग्रन्थिः_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#अञ्ञथा
        deleteCallerCap(ग्रन्थिः_STATE(ksCurThread));
        receiveIPC(ग्रन्थिः_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        भङ्ग;

    पक्ष cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        सचे (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
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

#ifdef CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य mcsPreemptionPoint(शून्य)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    सचे (isSchedulable(ग्रन्थिः_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } अञ्ञथा सचे (sc_active(ग्रन्थिः_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(ग्रन्थिः_STATE(ksConsumed), false);
    } अञ्ञथा {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        ग्रन्थिः_STATE(ksConsumed) = 0;
    }
}
#अञ्ञथा
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

स्थिर शून्य handleYield(शून्य)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = ग्रन्थिः_STATE(ksCurSC)->scConsumed + ग्रन्थिः_STATE(ksConsumed);
    refill_t head = *refill_head(ग्रन्थिः_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    ग्रन्थिः_STATE(ksCurSC)->scConsumed = consumed;
#अञ्ञथा
    tcbSchedDequeue(ग्रन्थिः_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        विकल्प (syscall)
        {
        पक्ष SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister));
            सचे (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            भङ्ग;

        पक्ष SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister));
            सचे (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            भङ्ग;

        पक्ष SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(ग्रन्थिः_STATE(ksCurThread), capRegister));
            सचे (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            भङ्ग;

        पक्ष SysRecv:
            handleRecv(true, true);
            भङ्ग;
#ifndef CONFIG_KERNEL_MCS
        पक्ष SysReply:
            handleReply();
            भङ्ग;

        पक्ष SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            भङ्ग;

#अञ्ञथा /* CONFIG_KERNEL_MCS */
        पक्ष SysWait:
            handleRecv(true, false);
            भङ्ग;

        पक्ष SysNBWait:
            handleRecv(false, false);
            भङ्ग;
        पक्ष SysReplyRecv: {
            cptr_t reply = getRegister(ग्रन्थिः_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            सचे (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                भङ्ग;
            }
            handleRecv(true, true);
            भङ्ग;
        }

        पक्ष SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            सचे (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                भङ्ग;
            }
            handleRecv(true, true);
            भङ्ग;
        }

        पक्ष SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(ग्रन्थिः_STATE(ksCurThread), replyRegister));
            सचे (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                भङ्ग;
            }
            handleRecv(true, false);
            भङ्ग;
#endif
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

    schedule();
    activateThread();

    पटिनिवत्त EXCEPTION_NONE;
}
