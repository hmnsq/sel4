/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <benchmark/benchmark.h>
#அந்தர்பாவ <arch/benchmark.h>
#அந்தர்பாவ <benchmark/benchmark_track.h>
#அந்தர்பாவ <benchmark/benchmark_utilisation.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <api/faults.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <kernel/faulthandler.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <plat/machine/hardware.h>
#அந்தர்பாவ <object/interrupt.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <kernel/traps.h>
#அந்தர்பாவ <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#அந்தர்பாவ <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#அந்தர்பாவ <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#அந்தர்பாவ <mode/machine/debug.h>
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
நிலையான உள்வரி வெறுமை checkInterrupt(bool_t was_interrupt_entry)
#இல்லையெனில்
நிலையான உள்வரி வெறுமை checkInterrupt(வெறுமை)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    எனில் (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } இல்லையெனில் {
#ifdef CONFIG_IRQ_REPORTING
        எனில் (was_interrupt_entry) {
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

exception_t handleInterruptEntry(வெறுமை)
{
#ifdef CONFIG_KERNEL_MCS
    எனில் (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    எனில் (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    திரும்பு EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    எனில் (w == SysDebugPutChar) {
        kernel_putchar(getRegister(முனை_STATE(ksCurThread), capRegister));
        திரும்பு EXCEPTION_NONE;
    }
    எனில் (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        திரும்பு EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    எனில் (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = முனை_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    எனில் (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = முனை_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        திரும்பு EXCEPTION_NONE;
    }
    எனில் (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(முனை_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(முனை_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(முனை_STATE(ksCurThread), capRegister, cap_type);
        திரும்பு EXCEPTION_NONE;
    }

    எனில் (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        மாறிலி எழுத்து *name;
        word_t len;
        word_t cptr = getRegister(முனை_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(முனை_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        எனில் (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (மாறிலி எழுத்து *)(lookupIPCBuffer(true, முனை_STATE(ksCurThread)) + 1);
        எனில் (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_செய்தி_Max_Length * அளவு(word_t));
        எனில் (len == se_L4_செய்தி_Max_Length * அளவு(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        திரும்பு EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    எனில் (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(முனை_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(முனை_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        எனில் (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(முனை_STATE(ksCurThread), capRegister, affinity);
        திரும்பு EXCEPTION_NONE;
    }
    எனில் (w == SysDebugSendIPI) {
        திரும்பு handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    எனில் (w == SysDebugRun) {
        ((வெறுமை (*)(வெறுமை *))getRegister(முனை_STATE(ksCurThread), capRegister))((வெறுமை *)getRegister(முனை_STATE(ksCurThread),
                                                                                                  செய்தி_Info_Register));
        திரும்பு EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    எனில் (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(முனை_STATE(ksCurThread), capRegister);
        எனில் (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, வெற்று) | ((uint64_t)getSyscallArg(1, வெற்று) << 32);
        } இல்லையெனில் {
            val = getSyscallArg(0, வெற்று);
        }
        x86_wrmsr(reg, val);
        திரும்பு EXCEPTION_NONE;
    } இல்லையெனில் எனில் (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(முனை_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        முழு இலக்கம் = 1;
        எனில் (CONFIG_WORD_SIZE == 32) {
            setMR(முனை_STATE(ksCurThread), வெற்று, 0, val & 0xffffffff);
            setMR(முனை_STATE(ksCurThread), வெற்று, 1, val >> 32);
            இலக்கம்++;
        } இல்லையெனில் {
            setMR(முனை_STATE(ksCurThread), வெற்று, 0, val);
        }
        setRegister(முனை_STATE(ksCurThread), செய்தி_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, இலக்கம்)));
        திரும்பு EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    தேர்வு (w) {
    நிலை SysBenchmarkFlushCaches:
        திரும்பு handle_SysBenchmarkFlushCaches();
    நிலை SysBenchmarkResetLog:
        திரும்பு handle_SysBenchmarkResetLog();
    நிலை SysBenchmarkFinalizeLog:
        திரும்பு handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    நிலை SysBenchmarkSetLogBuffer:
        திரும்பு handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    நிலை SysBenchmarkGetThreadUtilisation:
        திரும்பு handle_SysBenchmarkGetThreadUtilisation();
    நிலை SysBenchmarkResetThreadUtilisation:
        திரும்பு handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    நிலை SysBenchmarkDumpAllThreadsUtilisation:
        திரும்பு handle_SysBenchmarkDumpAllThreadsUtilisation();
    நிலை SysBenchmarkResetAllThreadsUtilisation:
        திரும்பு handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    நிலை SysBenchmarkNullSyscall:
        திரும்பு EXCEPTION_NONE;
    இயல்பு:
        நிறுத்து; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        எனில் (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(முனை_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            திரும்பு Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(முனை_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    திரும்பு EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(முனை_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    திரும்பு EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(முனை_STATE(ksCurThread), vm_faultType);
        எனில் (status != EXCEPTION_NONE)
        {
            handleFault(முனை_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    திரும்பு EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
நிலையான exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#இல்லையெனில்
நிலையான exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = முனை_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, செய்தி_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    எனில் (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        எனில் (isBlocking) {
            handleFault(thread);
        }

        திரும்பு EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    எனில் (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        எனில் (isBlocking) {
            handleFault(thread);
        }
        திரும்பு EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    எனில் (unlikely(length > n_செய்தி_Registers && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_செய்தி_Registers);
        length = n_செய்தி_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#இல்லையெனில்
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    எனில் (unlikely(status == EXCEPTION_PREEMPTED)) {
        திரும்பு status;
    }

    எனில் (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        எனில் (isCall) {
            replyFromKernel_error(thread);
        }
        திரும்பு EXCEPTION_NONE;
    }

    எனில் (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        எனில் (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    திரும்பு EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
நிலையான உள்வரி lookupCap_ret_t lookupReply(வெறுமை)
{
    word_t replyCPtr = getRegister(முனை_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(முனை_STATE(ksCurThread), replyCPtr);
    எனில் (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        திரும்பு lu_ret;
    }

    எனில் (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        திரும்பு lu_ret;
    }

    திரும்பு lu_ret;
}
#இல்லையெனில்
நிலையான வெறுமை handleReply(வெறுமை)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    தேர்வு (cap_get_capType(callerCap)) {
    நிலை cap_reply_cap: {
        tcb_t *caller;

        எனில் (cap_reply_cap_get_capReplyMaster(callerCap)) {
            நிறுத்து;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        ப்ரதிஜ்ஞா(caller != முனை_STATE(ksCurThread));
        doReplyTransfer(முனை_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        திரும்பு;
    }

    நிலை cap_null_cap:
        /* Do nothing when no caller is pending */
        திரும்பு;

    இயல்பு:
        நிறுத்து;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
நிலையான வெறுமை handleRecv(bool_t isBlocking, bool_t canReply)
#இல்லையெனில்
நிலையான வெறுமை handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(முனை_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(முனை_STATE(ksCurThread), epCPtr);

    எனில் (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(முனை_STATE(ksCurThread));
        திரும்பு;
    }

    தேர்வு (cap_get_capType(lu_ret.cap)) {
    நிலை cap_endpoint_cap:
        எனில் (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(முனை_STATE(ksCurThread));
            நிறுத்து;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        எனில் (canReply) {
            lu_ret = lookupReply();
            எனில் (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(முனை_STATE(ksCurThread));
                திரும்பு;
            } இல்லையெனில் {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(முனை_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#இல்லையெனில்
        deleteCallerCap(முனை_STATE(ksCurThread));
        receiveIPC(முனை_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        நிறுத்து;

    நிலை cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        எனில் (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != முனை_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(முனை_STATE(ksCurThread));
            நிறுத்து;
        }

        receiveSignal(முனை_STATE(ksCurThread), lu_ret.cap, isBlocking);
        நிறுத்து;
    }
    இயல்பு:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(முனை_STATE(ksCurThread));
        நிறுத்து;
    }
}

#ifdef CONFIG_KERNEL_MCS
நிலையான உள்வரி வெறுமை mcsPreemptionPoint(வெறுமை)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    எனில் (isSchedulable(முனை_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } இல்லையெனில் எனில் (sc_active(முனை_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(முனை_STATE(ksConsumed), false);
    } இல்லையெனில் {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        முனை_STATE(ksConsumed) = 0;
    }
}
#இல்லையெனில்
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

நிலையான வெறுமை handleYield(வெறுமை)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = முனை_STATE(ksCurSC)->scConsumed + முனை_STATE(ksConsumed);
    refill_t head = *refill_head(முனை_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    முனை_STATE(ksCurSC)->scConsumed = consumed;
#இல்லையெனில்
    tcbSchedDequeue(முனை_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        தேர்வு (syscall)
        {
        நிலை SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(முனை_STATE(ksCurThread), capRegister));
            எனில் (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            நிறுத்து;

        நிலை SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(முனை_STATE(ksCurThread), capRegister));
            எனில் (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            நிறுத்து;

        நிலை SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(முனை_STATE(ksCurThread), capRegister));
            எனில் (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            நிறுத்து;

        நிலை SysRecv:
            handleRecv(true, true);
            நிறுத்து;
#ifndef CONFIG_KERNEL_MCS
        நிலை SysReply:
            handleReply();
            நிறுத்து;

        நிலை SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            நிறுத்து;

#இல்லையெனில் /* CONFIG_KERNEL_MCS */
        நிலை SysWait:
            handleRecv(true, false);
            நிறுத்து;

        நிலை SysNBWait:
            handleRecv(false, false);
            நிறுத்து;
        நிலை SysReplyRecv: {
            cptr_t reply = getRegister(முனை_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            எனில் (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                நிறுத்து;
            }
            handleRecv(true, true);
            நிறுத்து;
        }

        நிலை SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            எனில் (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                நிறுத்து;
            }
            handleRecv(true, true);
            நிறுத்து;
        }

        நிலை SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(முனை_STATE(ksCurThread), replyRegister));
            எனில் (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                நிறுத்து;
            }
            handleRecv(true, false);
            நிறுத்து;
#endif
        நிலை SysNBRecv:
            handleRecv(false, true);
            நிறுத்து;

        நிலை SysYield:
            handleYield();
            நிறுத்து;

        இயல்பு:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    திரும்பு EXCEPTION_NONE;
}
