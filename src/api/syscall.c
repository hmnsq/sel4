/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <benchmark/benchmark.h>
#અન્તર્ભાવ <arch/benchmark.h>
#અન્તર્ભાવ <benchmark/benchmark_track.h>
#અન્તર્ભાવ <benchmark/benchmark_utilisation.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/faults.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <kernel/faulthandler.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <plat/machine/hardware.h>
#અન્તર્ભાવ <object/interrupt.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <kernel/traps.h>
#અન્તર્ભાવ <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#અન્તર્ભાવ <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#અન્તર્ભાવ <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#અન્તર્ભાવ <mode/machine/debug.h>
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
સ્થિર અંતઃરેખ શૂન્ય checkInterrupt(bool_t was_interrupt_entry)
#નહીંતર
સ્થિર અંતઃરેખ શૂન્ય checkInterrupt(શૂન્ય)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    જો (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } નહીંતર {
#ifdef CONFIG_IRQ_REPORTING
        જો (was_interrupt_entry) {
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

exception_t handleInterruptEntry(શૂન્ય)
{
#ifdef CONFIG_KERNEL_MCS
    જો (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    જો (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    પાછા EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    જો (w == SysDebugPutChar) {
        kernel_putchar(getRegister(ગાંઠ_STATE(ksCurThread), capRegister));
        પાછા EXCEPTION_NONE;
    }
    જો (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        પાછા EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    જો (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = ગાંઠ_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    જો (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = ગાંઠ_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        પાછા EXCEPTION_NONE;
    }
    જો (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ગાંઠ_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(ગાંઠ_STATE(ksCurThread), capRegister, cap_type);
        પાછા EXCEPTION_NONE;
    }

    જો (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        અચલ અક્ષર *name;
        word_t len;
        word_t cptr = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ગાંઠ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        જો (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (અચલ અક્ષર *)(lookupIPCBuffer(true, ગાંઠ_STATE(ksCurThread)) + 1);
        જો (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_સંદેશ_Max_Length * કદ(word_t));
        જો (len == se_L4_સંદેશ_Max_Length * કદ(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        પાછા EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    જો (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ગાંઠ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        જો (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(ગાંઠ_STATE(ksCurThread), capRegister, affinity);
        પાછા EXCEPTION_NONE;
    }
    જો (w == SysDebugSendIPI) {
        પાછા handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    જો (w == SysDebugRun) {
        ((શૂન્ય (*)(શૂન્ય *))getRegister(ગાંઠ_STATE(ksCurThread), capRegister))((શૂન્ય *)getRegister(ગાંઠ_STATE(ksCurThread),
                                                                                                  સંદેશ_Info_Register));
        પાછા EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    જો (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);
        જો (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, ખાલી) | ((uint64_t)getSyscallArg(1, ખાલી) << 32);
        } નહીંતર {
            val = getSyscallArg(0, ખાલી);
        }
        x86_wrmsr(reg, val);
        પાછા EXCEPTION_NONE;
    } નહીંતર જો (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        પૂર્ણાંક અંક = 1;
        જો (CONFIG_WORD_SIZE == 32) {
            setMR(ગાંઠ_STATE(ksCurThread), ખાલી, 0, val & 0xffffffff);
            setMR(ગાંઠ_STATE(ksCurThread), ખાલી, 1, val >> 32);
            અંક++;
        } નહીંતર {
            setMR(ગાંઠ_STATE(ksCurThread), ખાલી, 0, val);
        }
        setRegister(ગાંઠ_STATE(ksCurThread), સંદેશ_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, અંક)));
        પાછા EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    પસંદગી (w) {
    સ્થિતિ SysBenchmarkFlushCaches:
        પાછા handle_SysBenchmarkFlushCaches();
    સ્થિતિ SysBenchmarkResetLog:
        પાછા handle_SysBenchmarkResetLog();
    સ્થિતિ SysBenchmarkFinalizeLog:
        પાછા handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    સ્થિતિ SysBenchmarkSetLogBuffer:
        પાછા handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    સ્થિતિ SysBenchmarkGetThreadUtilisation:
        પાછા handle_SysBenchmarkGetThreadUtilisation();
    સ્થિતિ SysBenchmarkResetThreadUtilisation:
        પાછા handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    સ્થિતિ SysBenchmarkDumpAllThreadsUtilisation:
        પાછા handle_SysBenchmarkDumpAllThreadsUtilisation();
    સ્થિતિ SysBenchmarkResetAllThreadsUtilisation:
        પાછા handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    સ્થિતિ SysBenchmarkNullSyscall:
        પાછા EXCEPTION_NONE;
    મૂળભૂત:
        તોડો; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        જો (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            પાછા Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(ગાંઠ_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    પાછા EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(ગાંઠ_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    પાછા EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(ગાંઠ_STATE(ksCurThread), vm_faultType);
        જો (status != EXCEPTION_NONE)
        {
            handleFault(ગાંઠ_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
સ્થિર exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#નહીંતર
સ્થિર exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = ગાંઠ_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, સંદેશ_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        જો (isBlocking) {
            handleFault(thread);
        }

        પાછા EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    જો (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        જો (isBlocking) {
            handleFault(thread);
        }
        પાછા EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    જો (unlikely(length > n_સંદેશ_Registers && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_સંદેશ_Registers);
        length = n_સંદેશ_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#નહીંતર
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    જો (unlikely(status == EXCEPTION_PREEMPTED)) {
        પાછા status;
    }

    જો (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        જો (isCall) {
            replyFromKernel_error(thread);
        }
        પાછા EXCEPTION_NONE;
    }

    જો (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        જો (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
સ્થિર અંતઃરેખ lookupCap_ret_t lookupReply(શૂન્ય)
{
    word_t replyCPtr = getRegister(ગાંઠ_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(ગાંઠ_STATE(ksCurThread), replyCPtr);
    જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        પાછા lu_ret;
    }

    જો (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        પાછા lu_ret;
    }

    પાછા lu_ret;
}
#નહીંતર
સ્થિર શૂન્ય handleReply(શૂન્ય)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(ગાંઠ_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    પસંદગી (cap_get_capType(callerCap)) {
    સ્થિતિ cap_reply_cap: {
        tcb_t *caller;

        જો (cap_reply_cap_get_capReplyMaster(callerCap)) {
            તોડો;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        પ્રતિજ્ઞા(caller != ગાંઠ_STATE(ksCurThread));
        doReplyTransfer(ગાંઠ_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        પાછા;
    }

    સ્થિતિ cap_null_cap:
        /* Do nothing when no caller is pending */
        પાછા;

    મૂળભૂત:
        તોડો;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
સ્થિર શૂન્ય handleRecv(bool_t isBlocking, bool_t canReply)
#નહીંતર
સ્થિર શૂન્ય handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(ગાંઠ_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(ગાંઠ_STATE(ksCurThread), epCPtr);

    જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ગાંઠ_STATE(ksCurThread));
        પાછા;
    }

    પસંદગી (cap_get_capType(lu_ret.cap)) {
    સ્થિતિ cap_endpoint_cap:
        જો (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ગાંઠ_STATE(ksCurThread));
            તોડો;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        જો (canReply) {
            lu_ret = lookupReply();
            જો (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(ગાંઠ_STATE(ksCurThread));
                પાછા;
            } નહીંતર {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(ગાંઠ_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#નહીંતર
        deleteCallerCap(ગાંઠ_STATE(ksCurThread));
        receiveIPC(ગાંઠ_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        તોડો;

    સ્થિતિ cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        જો (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != ગાંઠ_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ગાંઠ_STATE(ksCurThread));
            તોડો;
        }

        receiveSignal(ગાંઠ_STATE(ksCurThread), lu_ret.cap, isBlocking);
        તોડો;
    }
    મૂળભૂત:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ગાંઠ_STATE(ksCurThread));
        તોડો;
    }
}

#ifdef CONFIG_KERNEL_MCS
સ્થિર અંતઃરેખ શૂન્ય mcsPreemptionPoint(શૂન્ય)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    જો (isSchedulable(ગાંઠ_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } નહીંતર જો (sc_active(ગાંઠ_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(ગાંઠ_STATE(ksConsumed), false);
    } નહીંતર {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        ગાંઠ_STATE(ksConsumed) = 0;
    }
}
#નહીંતર
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

સ્થિર શૂન્ય handleYield(શૂન્ય)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = ગાંઠ_STATE(ksCurSC)->scConsumed + ગાંઠ_STATE(ksConsumed);
    refill_t head = *refill_head(ગાંઠ_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    ગાંઠ_STATE(ksCurSC)->scConsumed = consumed;
#નહીંતર
    tcbSchedDequeue(ગાંઠ_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        પસંદગી (syscall)
        {
        સ્થિતિ SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(ગાંઠ_STATE(ksCurThread), capRegister));
            જો (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            તોડો;

        સ્થિતિ SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(ગાંઠ_STATE(ksCurThread), capRegister));
            જો (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            તોડો;

        સ્થિતિ SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(ગાંઠ_STATE(ksCurThread), capRegister));
            જો (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            તોડો;

        સ્થિતિ SysRecv:
            handleRecv(true, true);
            તોડો;
#ifndef CONFIG_KERNEL_MCS
        સ્થિતિ SysReply:
            handleReply();
            તોડો;

        સ્થિતિ SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            તોડો;

#નહીંતર /* CONFIG_KERNEL_MCS */
        સ્થિતિ SysWait:
            handleRecv(true, false);
            તોડો;

        સ્થિતિ SysNBWait:
            handleRecv(false, false);
            તોડો;
        સ્થિતિ SysReplyRecv: {
            cptr_t reply = getRegister(ગાંઠ_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            જો (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                તોડો;
            }
            handleRecv(true, true);
            તોડો;
        }

        સ્થિતિ SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            જો (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                તોડો;
            }
            handleRecv(true, true);
            તોડો;
        }

        સ્થિતિ SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(ગાંઠ_STATE(ksCurThread), replyRegister));
            જો (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                તોડો;
            }
            handleRecv(true, false);
            તોડો;
#endif
        સ્થિતિ SysNBRecv:
            handleRecv(false, true);
            તોડો;

        સ્થિતિ SysYield:
            handleYield();
            તોડો;

        મૂળભૂત:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    પાછા EXCEPTION_NONE;
}
