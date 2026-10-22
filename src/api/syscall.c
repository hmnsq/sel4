/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <benchmark/benchmark.h>
#شامل <arch/benchmark.h>
#شامل <benchmark/benchmark_track.h>
#شامل <benchmark/benchmark_utilisation.h>
#شامل <api/syscall.h>
#شامل <api/failures.h>
#شامل <api/faults.h>
#شامل <kernel/cspace.h>
#شامل <kernel/faulthandler.h>
#شامل <kernel/thread.h>
#شامل <kernel/vspace.h>
#شامل <machine/io.h>
#شامل <plat/machine/hardware.h>
#شامل <object/interrupt.h>
#شامل <model/statedata.h>
#شامل <string.h>
#شامل <kernel/traps.h>
#شامل <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#شامل <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#شامل <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#شامل <mode/machine/debug.h>
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
ایستا درونخطی تهی checkInterrupt(bool_t was_interrupt_entry)
#وگرنه
ایستا درونخطی تهی checkInterrupt(تهی)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    اگر (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } وگرنه {
#ifdef CONFIG_IRQ_REPORTING
        اگر (was_interrupt_entry) {
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

exception_t handleInterruptEntry(تهی)
{
#ifdef CONFIG_KERNEL_MCS
    اگر (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    اگر (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    بازگشت EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    اگر (w == SysDebugPutChar) {
        kernel_putchar(getRegister(گره_STATE(ksCurThread), capRegister));
        بازگشت EXCEPTION_NONE;
    }
    اگر (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        بازگشت EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    اگر (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = گره_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    اگر (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = گره_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        بازگشت EXCEPTION_NONE;
    }
    اگر (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(گره_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(گره_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(گره_STATE(ksCurThread), capRegister, cap_type);
        بازگشت EXCEPTION_NONE;
    }

    اگر (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ثابت نویسه *name;
        word_t len;
        word_t cptr = getRegister(گره_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(گره_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        اگر (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ثابت نویسه *)(lookupIPCBuffer(true, گره_STATE(ksCurThread)) + 1);
        اگر (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_پیام_Max_Length * اندازه(word_t));
        اگر (len == se_L4_پیام_Max_Length * اندازه(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        بازگشت EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    اگر (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(گره_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(گره_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        اگر (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(گره_STATE(ksCurThread), capRegister, affinity);
        بازگشت EXCEPTION_NONE;
    }
    اگر (w == SysDebugSendIPI) {
        بازگشت handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    اگر (w == SysDebugRun) {
        ((تهی (*)(تهی *))getRegister(گره_STATE(ksCurThread), capRegister))((تهی *)getRegister(گره_STATE(ksCurThread),
                                                                                                  پیام_Info_Register));
        بازگشت EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    اگر (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(گره_STATE(ksCurThread), capRegister);
        اگر (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, پوچ) | ((uint64_t)getSyscallArg(1, پوچ) << 32);
        } وگرنه {
            val = getSyscallArg(0, پوچ);
        }
        x86_wrmsr(reg, val);
        بازگشت EXCEPTION_NONE;
    } وگرنه اگر (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(گره_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        صحیح رقم = 1;
        اگر (CONFIG_WORD_SIZE == 32) {
            setMR(گره_STATE(ksCurThread), پوچ, 0, val & 0xffffffff);
            setMR(گره_STATE(ksCurThread), پوچ, 1, val >> 32);
            رقم++;
        } وگرنه {
            setMR(گره_STATE(ksCurThread), پوچ, 0, val);
        }
        setRegister(گره_STATE(ksCurThread), پیام_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, رقم)));
        بازگشت EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    انتخاب (w) {
    حالت SysBenchmarkFlushCaches:
        بازگشت handle_SysBenchmarkFlushCaches();
    حالت SysBenchmarkResetLog:
        بازگشت handle_SysBenchmarkResetLog();
    حالت SysBenchmarkFinalizeLog:
        بازگشت handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    حالت SysBenchmarkSetLogBuffer:
        بازگشت handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    حالت SysBenchmarkGetThreadUtilisation:
        بازگشت handle_SysBenchmarkGetThreadUtilisation();
    حالت SysBenchmarkResetThreadUtilisation:
        بازگشت handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    حالت SysBenchmarkDumpAllThreadsUtilisation:
        بازگشت handle_SysBenchmarkDumpAllThreadsUtilisation();
    حالت SysBenchmarkResetAllThreadsUtilisation:
        بازگشت handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    حالت SysBenchmarkNullSyscall:
        بازگشت EXCEPTION_NONE;
    پیشفرض:
        بشکن; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        اگر (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(گره_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            بازگشت Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(گره_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    بازگشت EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(گره_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    بازگشت EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(گره_STATE(ksCurThread), vm_faultType);
        اگر (status != EXCEPTION_NONE)
        {
            handleFault(گره_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    بازگشت EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ایستا exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#وگرنه
ایستا exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = گره_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, پیام_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    اگر (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        اگر (isBlocking) {
            handleFault(thread);
        }

        بازگشت EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    اگر (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        اگر (isBlocking) {
            handleFault(thread);
        }
        بازگشت EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    اگر (unlikely(length > n_پیام_Registers && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_پیام_Registers);
        length = n_پیام_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#وگرنه
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    اگر (unlikely(status == EXCEPTION_PREEMPTED)) {
        بازگشت status;
    }

    اگر (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        اگر (isCall) {
            replyFromKernel_error(thread);
        }
        بازگشت EXCEPTION_NONE;
    }

    اگر (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        اگر (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    بازگشت EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ایستا درونخطی lookupCap_ret_t lookupReply(تهی)
{
    word_t replyCPtr = getRegister(گره_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(گره_STATE(ksCurThread), replyCPtr);
    اگر (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        بازگشت lu_ret;
    }

    اگر (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        بازگشت lu_ret;
    }

    بازگشت lu_ret;
}
#وگرنه
ایستا تهی handleReply(تهی)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(گره_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    انتخاب (cap_get_capType(callerCap)) {
    حالت cap_reply_cap: {
        tcb_t *caller;

        اگر (cap_reply_cap_get_capReplyMaster(callerCap)) {
            بشکن;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        أكد(caller != گره_STATE(ksCurThread));
        doReplyTransfer(گره_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        بازگشت;
    }

    حالت cap_null_cap:
        /* Do nothing when no caller is pending */
        بازگشت;

    پیشفرض:
        بشکن;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
ایستا تهی handleRecv(bool_t isBlocking, bool_t canReply)
#وگرنه
ایستا تهی handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(گره_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(گره_STATE(ksCurThread), epCPtr);

    اگر (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(گره_STATE(ksCurThread));
        بازگشت;
    }

    انتخاب (cap_get_capType(lu_ret.cap)) {
    حالت cap_endpoint_cap:
        اگر (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(گره_STATE(ksCurThread));
            بشکن;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        اگر (canReply) {
            lu_ret = lookupReply();
            اگر (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(گره_STATE(ksCurThread));
                بازگشت;
            } وگرنه {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(گره_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#وگرنه
        deleteCallerCap(گره_STATE(ksCurThread));
        receiveIPC(گره_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        بشکن;

    حالت cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        اگر (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != گره_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(گره_STATE(ksCurThread));
            بشکن;
        }

        receiveSignal(گره_STATE(ksCurThread), lu_ret.cap, isBlocking);
        بشکن;
    }
    پیشفرض:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(گره_STATE(ksCurThread));
        بشکن;
    }
}

#ifdef CONFIG_KERNEL_MCS
ایستا درونخطی تهی mcsPreemptionPoint(تهی)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    اگر (isSchedulable(گره_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } وگرنه اگر (sc_active(گره_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(گره_STATE(ksConsumed), false);
    } وگرنه {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        گره_STATE(ksConsumed) = 0;
    }
}
#وگرنه
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

ایستا تهی handleYield(تهی)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = گره_STATE(ksCurSC)->scConsumed + گره_STATE(ksConsumed);
    refill_t head = *refill_head(گره_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    گره_STATE(ksCurSC)->scConsumed = consumed;
#وگرنه
    tcbSchedDequeue(گره_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        انتخاب (syscall)
        {
        حالت SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(گره_STATE(ksCurThread), capRegister));
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            بشکن;

        حالت SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(گره_STATE(ksCurThread), capRegister));
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            بشکن;

        حالت SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(گره_STATE(ksCurThread), capRegister));
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            بشکن;

        حالت SysRecv:
            handleRecv(true, true);
            بشکن;
#ifndef CONFIG_KERNEL_MCS
        حالت SysReply:
            handleReply();
            بشکن;

        حالت SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            بشکن;

#وگرنه /* CONFIG_KERNEL_MCS */
        حالت SysWait:
            handleRecv(true, false);
            بشکن;

        حالت SysNBWait:
            handleRecv(false, false);
            بشکن;
        حالت SysReplyRecv: {
            cptr_t reply = getRegister(گره_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                بشکن;
            }
            handleRecv(true, true);
            بشکن;
        }

        حالت SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                بشکن;
            }
            handleRecv(true, true);
            بشکن;
        }

        حالت SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(گره_STATE(ksCurThread), replyRegister));
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                بشکن;
            }
            handleRecv(true, false);
            بشکن;
#endif
        حالت SysNBRecv:
            handleRecv(false, true);
            بشکن;

        حالت SysYield:
            handleYield();
            بشکن;

        پیشفرض:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    بازگشت EXCEPTION_NONE;
}
