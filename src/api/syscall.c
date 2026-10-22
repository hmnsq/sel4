/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <benchmark/benchmark.h>
#ضمن <arch/benchmark.h>
#ضمن <benchmark/benchmark_track.h>
#ضمن <benchmark/benchmark_utilisation.h>
#ضمن <api/syscall.h>
#ضمن <api/failures.h>
#ضمن <api/faults.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/faulthandler.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/hardware.h>
#ضمن <object/interrupt.h>
#ضمن <model/statedata.h>
#ضمن <string.h>
#ضمن <kernel/traps.h>
#ضمن <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#ضمن <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#ضمن <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#ضمن <mode/machine/debug.h>
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
ساكن مضمن فراغ checkInterrupt(bool_t was_interrupt_entry)
#وإلا
ساكن مضمن فراغ checkInterrupt(فراغ)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    إذا (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } وإلا {
#ifdef CONFIG_IRQ_REPORTING
        إذا (was_interrupt_entry) {
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

exception_t handleInterruptEntry(فراغ)
{
#ifdef CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    أرجع EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    إذا (w == SysDebugPutChar) {
        kernel_putchar(getRegister(كلي_STATE(ksCurThread), capRegister));
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        أرجع EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    إذا (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = كلي_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    إذا (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = كلي_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(كلي_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(كلي_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(كلي_STATE(ksCurThread), capRegister, cap_type);
        أرجع EXCEPTION_NONE;
    }

    إذا (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ثابت حرف *name;
        word_t len;
        word_t cptr = getRegister(كلي_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(كلي_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ثابت حرف *)(lookupIPCBuffer(true, كلي_STATE(ksCurThread)) + 1);
        إذا (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_سكو_Max_Length * حجم(word_t));
        إذا (len == se_L4_سكو_Max_Length * حجم(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        أرجع EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    إذا (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(كلي_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(كلي_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(كلي_STATE(ksCurThread), capRegister, affinity);
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugSendIPI) {
        أرجع handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    إذا (w == SysDebugRun) {
        ((فراغ (*)(فراغ *))getRegister(كلي_STATE(ksCurThread), capRegister))((فراغ *)getRegister(كلي_STATE(ksCurThread),
                                                                                                  سكو_Info_Register));
        أرجع EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    إذا (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(كلي_STATE(ksCurThread), capRegister);
        إذا (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, عدم) | ((uint64_t)getSyscallArg(1, عدم) << 32);
        } وإلا {
            val = getSyscallArg(0, عدم);
        }
        x86_wrmsr(reg, val);
        أرجع EXCEPTION_NONE;
    } وإلا إذا (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(كلي_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        صحيح لمب = 1;
        إذا (CONFIG_WORD_SIZE == 32) {
            setMR(كلي_STATE(ksCurThread), عدم, 0, val & 0xffffffff);
            setMR(كلي_STATE(ksCurThread), عدم, 1, val >> 32);
            لمب++;
        } وإلا {
            setMR(كلي_STATE(ksCurThread), عدم, 0, val);
        }
        setRegister(كلي_STATE(ksCurThread), سكو_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, لمب)));
        أرجع EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    بدل (w) {
    حالة SysBenchmarkFlushCaches:
        أرجع handle_SysBenchmarkFlushCaches();
    حالة SysBenchmarkResetLog:
        أرجع handle_SysBenchmarkResetLog();
    حالة SysBenchmarkFinalizeLog:
        أرجع handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    حالة SysBenchmarkSetLogBuffer:
        أرجع handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    حالة SysBenchmarkGetThreadUtilisation:
        أرجع handle_SysBenchmarkGetThreadUtilisation();
    حالة SysBenchmarkResetThreadUtilisation:
        أرجع handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    حالة SysBenchmarkDumpAllThreadsUtilisation:
        أرجع handle_SysBenchmarkDumpAllThreadsUtilisation();
    حالة SysBenchmarkResetAllThreadsUtilisation:
        أرجع handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    حالة SysBenchmarkNullSyscall:
        أرجع EXCEPTION_NONE;
    افتراضي:
        اكسر; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        إذا (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(كلي_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            أرجع Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(كلي_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(كلي_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(كلي_STATE(ksCurThread), vm_faultType);
        إذا (status != EXCEPTION_NONE)
        {
            handleFault(كلي_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    أرجع EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#وإلا
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = كلي_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, سكو_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        إذا (isBlocking) {
            handleFault(thread);
        }

        أرجع EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    إذا (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        إذا (isBlocking) {
            handleFault(thread);
        }
        أرجع EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    إذا (unlikely(length > n_سكو_Registers && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_سكو_Registers);
        length = n_سكو_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#وإلا
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    إذا (unlikely(status == EXCEPTION_PREEMPTED)) {
        أرجع status;
    }

    إذا (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        إذا (isCall) {
            replyFromKernel_error(thread);
        }
        أرجع EXCEPTION_NONE;
    }

    إذا (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        إذا (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    أرجع EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ساكن مضمن lookupCap_ret_t lookupReply(فراغ)
{
    word_t replyCPtr = getRegister(كلي_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(كلي_STATE(ksCurThread), replyCPtr);
    إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        أرجع lu_ret;
    }

    إذا (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        أرجع lu_ret;
    }

    أرجع lu_ret;
}
#وإلا
ساكن فراغ handleReply(فراغ)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(كلي_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    بدل (cap_get_capType(callerCap)) {
    حالة cap_reply_cap: {
        tcb_t *caller;

        إذا (cap_reply_cap_get_capReplyMaster(callerCap)) {
            اكسر;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        أكد(caller != كلي_STATE(ksCurThread));
        doReplyTransfer(كلي_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        أرجع;
    }

    حالة cap_null_cap:
        /* Do nothing when no caller is pending */
        أرجع;

    افتراضي:
        اكسر;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
ساكن فراغ handleRecv(bool_t isBlocking, bool_t canReply)
#وإلا
ساكن فراغ handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(كلي_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(كلي_STATE(ksCurThread), epCPtr);

    إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(كلي_STATE(ksCurThread));
        أرجع;
    }

    بدل (cap_get_capType(lu_ret.cap)) {
    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(كلي_STATE(ksCurThread));
            اكسر;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        إذا (canReply) {
            lu_ret = lookupReply();
            إذا (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(كلي_STATE(ksCurThread));
                أرجع;
            } وإلا {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(كلي_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#وإلا
        deleteCallerCap(كلي_STATE(ksCurThread));
        receiveIPC(كلي_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        اكسر;

    حالة cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != كلي_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(كلي_STATE(ksCurThread));
            اكسر;
        }

        receiveSignal(كلي_STATE(ksCurThread), lu_ret.cap, isBlocking);
        اكسر;
    }
    افتراضي:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(كلي_STATE(ksCurThread));
        اكسر;
    }
}

#ifdef CONFIG_KERNEL_MCS
ساكن مضمن فراغ mcsPreemptionPoint(فراغ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    إذا (isSchedulable(كلي_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } وإلا إذا (sc_active(كلي_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(كلي_STATE(ksConsumed), false);
    } وإلا {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        كلي_STATE(ksConsumed) = 0;
    }
}
#وإلا
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

ساكن فراغ handleYield(فراغ)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = كلي_STATE(ksCurSC)->scConsumed + كلي_STATE(ksConsumed);
    refill_t head = *refill_head(كلي_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    كلي_STATE(ksCurSC)->scConsumed = consumed;
#وإلا
    tcbSchedDequeue(كلي_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        بدل (syscall)
        {
        حالة SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(كلي_STATE(ksCurThread), capRegister));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            اكسر;

        حالة SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(كلي_STATE(ksCurThread), capRegister));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(كلي_STATE(ksCurThread), capRegister));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysRecv:
            handleRecv(true, true);
            اكسر;
#ifndef CONFIG_KERNEL_MCS
        حالة SysReply:
            handleReply();
            اكسر;

        حالة SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            اكسر;

#وإلا /* CONFIG_KERNEL_MCS */
        حالة SysWait:
            handleRecv(true, false);
            اكسر;

        حالة SysNBWait:
            handleRecv(false, false);
            اكسر;
        حالة SysReplyRecv: {
            cptr_t reply = getRegister(كلي_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(كلي_STATE(ksCurThread), replyRegister));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, false);
            اكسر;
#endif
        حالة SysNBRecv:
            handleRecv(false, true);
            اكسر;

        حالة SysYield:
            handleYield();
            اكسر;

        افتراضي:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    أرجع EXCEPTION_NONE;
}
