/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <benchmark/benchmark.h>
#അന്തര്ഭാവ <arch/benchmark.h>
#അന്തര്ഭാവ <benchmark/benchmark_track.h>
#അന്തര്ഭാവ <benchmark/benchmark_utilisation.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <api/faults.h>
#അന്തര്ഭാവ <kernel/cspace.h>
#അന്തര്ഭാവ <kernel/faulthandler.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <plat/machine/hardware.h>
#അന്തര്ഭാവ <object/interrupt.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <kernel/traps.h>
#അന്തര്ഭാവ <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#അന്തര്ഭാവ <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#അന്തര്ഭാവ <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#അന്തര്ഭാവ <mode/machine/debug.h>
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
സ്ഥിതം വരിയിൽ ശൂന്യം checkInterrupt(bool_t was_interrupt_entry)
#അല്ലെങ്കിൽ
സ്ഥിതം വരിയിൽ ശൂന്യം checkInterrupt(ശൂന്യം)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    എങ്കിൽ (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } അല്ലെങ്കിൽ {
#ifdef CONFIG_IRQ_REPORTING
        എങ്കിൽ (was_interrupt_entry) {
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

exception_t handleInterruptEntry(ശൂന്യം)
{
#ifdef CONFIG_KERNEL_MCS
    എങ്കിൽ (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    എങ്കിൽ (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    തിരികെ EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    എങ്കിൽ (w == SysDebugPutChar) {
        kernel_putchar(getRegister(കണ്ണി_STATE(ksCurThread), capRegister));
        തിരികെ EXCEPTION_NONE;
    }
    എങ്കിൽ (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        തിരികെ EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    എങ്കിൽ (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = കണ്ണി_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    എങ്കിൽ (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = കണ്ണി_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        തിരികെ EXCEPTION_NONE;
    }
    എങ്കിൽ (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(കണ്ണി_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(കണ്ണി_STATE(ksCurThread), capRegister, cap_type);
        തിരികെ EXCEPTION_NONE;
    }

    എങ്കിൽ (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        സ്ഥിരം അക്ഷരം *name;
        word_t len;
        word_t cptr = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(കണ്ണി_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        എങ്കിൽ (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (സ്ഥിരം അക്ഷരം *)(lookupIPCBuffer(true, കണ്ണി_STATE(ksCurThread)) + 1);
        എങ്കിൽ (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_സന്ദേശം_Max_Length * വലുപ്പം(word_t));
        എങ്കിൽ (len == se_L4_സന്ദേശം_Max_Length * വലുപ്പം(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        തിരികെ EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    എങ്കിൽ (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(കണ്ണി_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        എങ്കിൽ (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(കണ്ണി_STATE(ksCurThread), capRegister, affinity);
        തിരികെ EXCEPTION_NONE;
    }
    എങ്കിൽ (w == SysDebugSendIPI) {
        തിരികെ handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    എങ്കിൽ (w == SysDebugRun) {
        ((ശൂന്യം (*)(ശൂന്യം *))getRegister(കണ്ണി_STATE(ksCurThread), capRegister))((ശൂന്യം *)getRegister(കണ്ണി_STATE(ksCurThread),
                                                                                                  സന്ദേശം_Info_Register));
        തിരികെ EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    എങ്കിൽ (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
        എങ്കിൽ (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, ശൂന്യ) | ((uint64_t)getSyscallArg(1, ശൂന്യ) << 32);
        } അല്ലെങ്കിൽ {
            val = getSyscallArg(0, ശൂന്യ);
        }
        x86_wrmsr(reg, val);
        തിരികെ EXCEPTION_NONE;
    } അല്ലെങ്കിൽ എങ്കിൽ (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        പൂർണ്ണസംഖ്യ അക്കം = 1;
        എങ്കിൽ (CONFIG_WORD_SIZE == 32) {
            setMR(കണ്ണി_STATE(ksCurThread), ശൂന്യ, 0, val & 0xffffffff);
            setMR(കണ്ണി_STATE(ksCurThread), ശൂന്യ, 1, val >> 32);
            അക്കം++;
        } അല്ലെങ്കിൽ {
            setMR(കണ്ണി_STATE(ksCurThread), ശൂന്യ, 0, val);
        }
        setRegister(കണ്ണി_STATE(ksCurThread), സന്ദേശം_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, അക്കം)));
        തിരികെ EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    തിരഞ്ഞെടുപ്പ് (w) {
    അവസ്ഥ SysBenchmarkFlushCaches:
        തിരികെ handle_SysBenchmarkFlushCaches();
    അവസ്ഥ SysBenchmarkResetLog:
        തിരികെ handle_SysBenchmarkResetLog();
    അവസ്ഥ SysBenchmarkFinalizeLog:
        തിരികെ handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    അവസ്ഥ SysBenchmarkSetLogBuffer:
        തിരികെ handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    അവസ്ഥ SysBenchmarkGetThreadUtilisation:
        തിരികെ handle_SysBenchmarkGetThreadUtilisation();
    അവസ്ഥ SysBenchmarkResetThreadUtilisation:
        തിരികെ handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    അവസ്ഥ SysBenchmarkDumpAllThreadsUtilisation:
        തിരികെ handle_SysBenchmarkDumpAllThreadsUtilisation();
    അവസ്ഥ SysBenchmarkResetAllThreadsUtilisation:
        തിരികെ handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    അവസ്ഥ SysBenchmarkNullSyscall:
        തിരികെ EXCEPTION_NONE;
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        എങ്കിൽ (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            തിരികെ Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(കണ്ണി_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    തിരികെ EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(കണ്ണി_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    തിരികെ EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(കണ്ണി_STATE(ksCurThread), vm_faultType);
        എങ്കിൽ (status != EXCEPTION_NONE)
        {
            handleFault(കണ്ണി_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    തിരികെ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
സ്ഥിതം exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#അല്ലെങ്കിൽ
സ്ഥിതം exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = കണ്ണി_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, സന്ദേശം_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    എങ്കിൽ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        എങ്കിൽ (isBlocking) {
            handleFault(thread);
        }

        തിരികെ EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    എങ്കിൽ (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        എങ്കിൽ (isBlocking) {
            handleFault(thread);
        }
        തിരികെ EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    എങ്കിൽ (unlikely(length > n_സന്ദേശം_Registers && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_സന്ദേശം_Registers);
        length = n_സന്ദേശം_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#അല്ലെങ്കിൽ
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    എങ്കിൽ (unlikely(status == EXCEPTION_PREEMPTED)) {
        തിരികെ status;
    }

    എങ്കിൽ (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        എങ്കിൽ (isCall) {
            replyFromKernel_error(thread);
        }
        തിരികെ EXCEPTION_NONE;
    }

    എങ്കിൽ (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        എങ്കിൽ (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    തിരികെ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
സ്ഥിതം വരിയിൽ lookupCap_ret_t lookupReply(ശൂന്യം)
{
    word_t replyCPtr = getRegister(കണ്ണി_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(കണ്ണി_STATE(ksCurThread), replyCPtr);
    എങ്കിൽ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        തിരികെ lu_ret;
    }

    എങ്കിൽ (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        തിരികെ lu_ret;
    }

    തിരികെ lu_ret;
}
#അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം handleReply(ശൂന്യം)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(കണ്ണി_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(callerCap)) {
    അവസ്ഥ cap_reply_cap: {
        tcb_t *caller;

        എങ്കിൽ (cap_reply_cap_get_capReplyMaster(callerCap)) {
            നിർത്തുക;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        പ്രതിജ്ഞാ(caller != കണ്ണി_STATE(ksCurThread));
        doReplyTransfer(കണ്ണി_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        തിരികെ;
    }

    അവസ്ഥ cap_null_cap:
        /* Do nothing when no caller is pending */
        തിരികെ;

    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
സ്ഥിതം ശൂന്യം handleRecv(bool_t isBlocking, bool_t canReply)
#അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(കണ്ണി_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(കണ്ണി_STATE(ksCurThread), epCPtr);

    എങ്കിൽ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(കണ്ണി_STATE(ksCurThread));
        തിരികെ;
    }

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(lu_ret.cap)) {
    അവസ്ഥ cap_endpoint_cap:
        എങ്കിൽ (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(കണ്ണി_STATE(ksCurThread));
            നിർത്തുക;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        എങ്കിൽ (canReply) {
            lu_ret = lookupReply();
            എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(കണ്ണി_STATE(ksCurThread));
                തിരികെ;
            } അല്ലെങ്കിൽ {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(കണ്ണി_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#അല്ലെങ്കിൽ
        deleteCallerCap(കണ്ണി_STATE(ksCurThread));
        receiveIPC(കണ്ണി_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        നിർത്തുക;

    അവസ്ഥ cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        എങ്കിൽ (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != കണ്ണി_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(കണ്ണി_STATE(ksCurThread));
            നിർത്തുക;
        }

        receiveSignal(കണ്ണി_STATE(ksCurThread), lu_ret.cap, isBlocking);
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(കണ്ണി_STATE(ksCurThread));
        നിർത്തുക;
    }
}

#ifdef CONFIG_KERNEL_MCS
സ്ഥിതം വരിയിൽ ശൂന്യം mcsPreemptionPoint(ശൂന്യം)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    എങ്കിൽ (isSchedulable(കണ്ണി_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } അല്ലെങ്കിൽ എങ്കിൽ (sc_active(കണ്ണി_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(കണ്ണി_STATE(ksConsumed), false);
    } അല്ലെങ്കിൽ {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        കണ്ണി_STATE(ksConsumed) = 0;
    }
}
#അല്ലെങ്കിൽ
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

സ്ഥിതം ശൂന്യം handleYield(ശൂന്യം)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = കണ്ണി_STATE(ksCurSC)->scConsumed + കണ്ണി_STATE(ksConsumed);
    refill_t head = *refill_head(കണ്ണി_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    കണ്ണി_STATE(ksCurSC)->scConsumed = consumed;
#അല്ലെങ്കിൽ
    tcbSchedDequeue(കണ്ണി_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        തിരഞ്ഞെടുപ്പ് (syscall)
        {
        അവസ്ഥ SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(കണ്ണി_STATE(ksCurThread), capRegister));
            എങ്കിൽ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            നിർത്തുക;

        അവസ്ഥ SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(കണ്ണി_STATE(ksCurThread), capRegister));
            എങ്കിൽ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            നിർത്തുക;

        അവസ്ഥ SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(കണ്ണി_STATE(ksCurThread), capRegister));
            എങ്കിൽ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            നിർത്തുക;

        അവസ്ഥ SysRecv:
            handleRecv(true, true);
            നിർത്തുക;
#ifndef CONFIG_KERNEL_MCS
        അവസ്ഥ SysReply:
            handleReply();
            നിർത്തുക;

        അവസ്ഥ SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            നിർത്തുക;

#അല്ലെങ്കിൽ /* CONFIG_KERNEL_MCS */
        അവസ്ഥ SysWait:
            handleRecv(true, false);
            നിർത്തുക;

        അവസ്ഥ SysNBWait:
            handleRecv(false, false);
            നിർത്തുക;
        അവസ്ഥ SysReplyRecv: {
            cptr_t reply = getRegister(കണ്ണി_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            എങ്കിൽ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                നിർത്തുക;
            }
            handleRecv(true, true);
            നിർത്തുക;
        }

        അവസ്ഥ SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            എങ്കിൽ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                നിർത്തുക;
            }
            handleRecv(true, true);
            നിർത്തുക;
        }

        അവസ്ഥ SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(കണ്ണി_STATE(ksCurThread), replyRegister));
            എങ്കിൽ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                നിർത്തുക;
            }
            handleRecv(true, false);
            നിർത്തുക;
#endif
        അവസ്ഥ SysNBRecv:
            handleRecv(false, true);
            നിർത്തുക;

        അവസ്ഥ SysYield:
            handleYield();
            നിർത്തുക;

        സ്ഥിരസ്ഥിതി:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    തിരികെ EXCEPTION_NONE;
}
