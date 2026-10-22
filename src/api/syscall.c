/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <benchmark/benchmark.h>
#অন্তর্ভাব <arch/benchmark.h>
#অন্তর্ভাব <benchmark/benchmark_track.h>
#অন্তর্ভাব <benchmark/benchmark_utilisation.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/faults.h>
#অন্তর্ভাব <kernel/cspace.h>
#অন্তর্ভাব <kernel/faulthandler.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <plat/machine/hardware.h>
#অন্তর্ভাব <object/interrupt.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <kernel/traps.h>
#অন্তর্ভাব <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#অন্তর্ভাব <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#অন্তর্ভাব <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#অন্তর্ভাব <mode/machine/debug.h>
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
স্থির অন্তঃরেখ শূন্য checkInterrupt(bool_t was_interrupt_entry)
#অন্যথা
স্থির অন্তঃরেখ শূন্য checkInterrupt(শূন্য)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    যদি (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } অন্যথা {
#ifdef CONFIG_IRQ_REPORTING
        যদি (was_interrupt_entry) {
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

exception_t handleInterruptEntry(শূন্য)
{
#ifdef CONFIG_KERNEL_MCS
    যদি (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    যদি (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    ফেরত EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    যদি (w == SysDebugPutChar) {
        kernel_putchar(getRegister(গ্রন্থি_STATE(ksCurThread), capRegister));
        ফেরত EXCEPTION_NONE;
    }
    যদি (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        ফেরত EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    যদি (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = গ্রন্থি_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    যদি (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = গ্রন্থি_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        ফেরত EXCEPTION_NONE;
    }
    যদি (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(গ্রন্থি_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(গ্রন্থি_STATE(ksCurThread), capRegister, cap_type);
        ফেরত EXCEPTION_NONE;
    }

    যদি (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ধ্রুবক অক্ষর *name;
        word_t len;
        word_t cptr = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(গ্রন্থি_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        যদি (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ধ্রুবক অক্ষর *)(lookupIPCBuffer(true, গ্রন্থি_STATE(ksCurThread)) + 1);
        যদি (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_বার্তা_Max_দৈর্ঘ্য * আকার(word_t));
        যদি (len == se_L4_বার্তা_Max_দৈর্ঘ্য * আকার(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        ফেরত EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    যদি (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(গ্রন্থি_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        যদি (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(গ্রন্থি_STATE(ksCurThread), capRegister, affinity);
        ফেরত EXCEPTION_NONE;
    }
    যদি (w == SysDebugSendIPI) {
        ফেরত handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    যদি (w == SysDebugRun) {
        ((শূন্য (*)(শূন্য *))getRegister(গ্রন্থি_STATE(ksCurThread), capRegister))((শূন্য *)getRegister(গ্রন্থি_STATE(ksCurThread),
                                                                                                  বার্তা_Info_Register));
        ফেরত EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    যদি (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);
        যদি (CONFIG_WORD_আকার == 32) {
            val = (uint64_t)getSyscallArg(0, ফাঁকা) | ((uint64_t)getSyscallArg(1, ফাঁকা) << 32);
        } অন্যথা {
            val = getSyscallArg(0, ফাঁকা);
        }
        x86_wrmsr(reg, val);
        ফেরত EXCEPTION_NONE;
    } অন্যথা যদি (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        পূর্ণসংখ্যা অঙ্ক = 1;
        যদি (CONFIG_WORD_আকার == 32) {
            setMR(গ্রন্থি_STATE(ksCurThread), ফাঁকা, 0, val & 0xffffffff);
            setMR(গ্রন্থি_STATE(ksCurThread), ফাঁকা, 1, val >> 32);
            অঙ্ক++;
        } অন্যথা {
            setMR(গ্রন্থি_STATE(ksCurThread), ফাঁকা, 0, val);
        }
        setRegister(গ্রন্থি_STATE(ksCurThread), বার্তা_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, অঙ্ক)));
        ফেরত EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    নির্বাচন (w) {
    ক্ষেত্র SysBenchmarkFlushCaches:
        ফেরত handle_SysBenchmarkFlushCaches();
    ক্ষেত্র SysBenchmarkResetLog:
        ফেরত handle_SysBenchmarkResetLog();
    ক্ষেত্র SysBenchmarkFinalizeLog:
        ফেরত handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_বাফার
    ক্ষেত্র Sys_Benchmark_Set_Log_বাফার:
        ফেরত handle_Sys_Benchmark_Set_Log_বাফার();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    ক্ষেত্র SysBenchmarkGetThreadUtilisation:
        ফেরত handle_SysBenchmarkGetThreadUtilisation();
    ক্ষেত্র SysBenchmarkResetThreadUtilisation:
        ফেরত handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    ক্ষেত্র SysBenchmarkDumpAllThreadsUtilisation:
        ফেরত handle_SysBenchmarkDumpAllThreadsUtilisation();
    ক্ষেত্র SysBenchmarkResetAllThreadsUtilisation:
        ফেরত handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    ক্ষেত্র SysBenchmarkNullSyscall:
        ফেরত EXCEPTION_NONE;
    পূর্বনির্ধারিত:
        বিরতি; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        যদি (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            ফেরত Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(গ্রন্থি_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    ফেরত EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(গ্রন্থি_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    ফেরত EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(গ্রন্থি_STATE(ksCurThread), vm_faultType);
        যদি (status != EXCEPTION_NONE)
        {
            handleFault(গ্রন্থি_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    ফেরত EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
স্থির exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#অন্যথা
স্থির exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *বাফার;
    exception_t status;
    word_t দৈর্ঘ্য;
    tcb_t *thread;

    thread = গ্রন্থি_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, বার্তা_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        যদি (isBlocking) {
            handleFault(thread);
        }

        ফেরত EXCEPTION_NONE;
    }

    বাফার = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, বাফার, info);

    যদি (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        যদি (isBlocking) {
            handleFault(thread);
        }
        ফেরত EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    দৈর্ঘ্য = se_L4_Message_Info_get_দৈর্ঘ্য(info);
    যদি (unlikely(দৈর্ঘ্য > n_বার্তা_Registers && !বাফার)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_বার্তা_Registers);
        দৈর্ঘ্য = n_বার্তা_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), দৈর্ঘ্য,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, বাফার);
#অন্যথা
    status = decodeInvocation(seL4_MessageInfo_get_label(info), দৈর্ঘ্য,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, বাফার);
#endif

    যদি (unlikely(status == EXCEPTION_PREEMPTED)) {
        ফেরত status;
    }

    যদি (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        যদি (isCall) {
            replyFromKernel_error(thread);
        }
        ফেরত EXCEPTION_NONE;
    }

    যদি (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        যদি (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    ফেরত EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
স্থির অন্তঃরেখ lookupCap_ret_t lookupReply(শূন্য)
{
    word_t replyCPtr = getRegister(গ্রন্থি_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(গ্রন্থি_STATE(ksCurThread), replyCPtr);
    যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        ফেরত lu_ret;
    }

    যদি (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        ফেরত lu_ret;
    }

    ফেরত lu_ret;
}
#অন্যথা
স্থির শূন্য handleReply(শূন্য)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(গ্রন্থি_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    নির্বাচন (cap_get_capType(callerCap)) {
    ক্ষেত্র cap_reply_cap: {
        tcb_t *caller;

        যদি (cap_reply_cap_get_capReplyMaster(callerCap)) {
            বিরতি;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        প্রতিজ্ঞা(caller != গ্রন্থি_STATE(ksCurThread));
        doReplyTransfer(গ্রন্থি_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        ফেরত;
    }

    ক্ষেত্র cap_null_cap:
        /* Do nothing when no caller is pending */
        ফেরত;

    পূর্বনির্ধারিত:
        বিরতি;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
স্থির শূন্য handleRecv(bool_t isBlocking, bool_t canReply)
#অন্যথা
স্থির শূন্য handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(গ্রন্থি_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(গ্রন্থি_STATE(ksCurThread), epCPtr);

    যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(গ্রন্থি_STATE(ksCurThread));
        ফেরত;
    }

    নির্বাচন (cap_get_capType(lu_ret.cap)) {
    ক্ষেত্র cap_endpoint_cap:
        যদি (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(গ্রন্থি_STATE(ksCurThread));
            বিরতি;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        যদি (canReply) {
            lu_ret = lookupReply();
            যদি (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(গ্রন্থি_STATE(ksCurThread));
                ফেরত;
            } অন্যথা {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(গ্রন্থি_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#অন্যথা
        deleteCallerCap(গ্রন্থি_STATE(ksCurThread));
        receiveIPC(গ্রন্থি_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        বিরতি;

    ক্ষেত্র cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        যদি (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != গ্রন্থি_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(গ্রন্থি_STATE(ksCurThread));
            বিরতি;
        }

        receiveSignal(গ্রন্থি_STATE(ksCurThread), lu_ret.cap, isBlocking);
        বিরতি;
    }
    পূর্বনির্ধারিত:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(গ্রন্থি_STATE(ksCurThread));
        বিরতি;
    }
}

#ifdef CONFIG_KERNEL_MCS
স্থির অন্তঃরেখ শূন্য mcsPreemptionPoint(শূন্য)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    যদি (isSchedulable(গ্রন্থি_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } অন্যথা যদি (sc_active(গ্রন্থি_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(গ্রন্থি_STATE(ksConsumed), false);
    } অন্যথা {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        গ্রন্থি_STATE(ksConsumed) = 0;
    }
}
#অন্যথা
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

স্থির শূন্য handleYield(শূন্য)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = গ্রন্থি_STATE(ksCurSC)->scConsumed + গ্রন্থি_STATE(ksConsumed);
    refill_t শীর্ষ = *refill_শীর্ষ(গ্রন্থি_STATE(ksCurSC));
    chargeBudget(শীর্ষ.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    গ্রন্থি_STATE(ksCurSC)->scConsumed = consumed;
#অন্যথা
    tcbSchedDequeue(গ্রন্থি_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        নির্বাচন (syscall)
        {
        ক্ষেত্র SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(গ্রন্থি_STATE(ksCurThread), capRegister));
            যদি (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            বিরতি;

        ক্ষেত্র SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(গ্রন্থি_STATE(ksCurThread), capRegister));
            যদি (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            বিরতি;

        ক্ষেত্র SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(গ্রন্থি_STATE(ksCurThread), capRegister));
            যদি (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            বিরতি;

        ক্ষেত্র SysRecv:
            handleRecv(true, true);
            বিরতি;
#ifndef CONFIG_KERNEL_MCS
        ক্ষেত্র SysReply:
            handleReply();
            বিরতি;

        ক্ষেত্র SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            বিরতি;

#অন্যথা /* CONFIG_KERNEL_MCS */
        ক্ষেত্র SysWait:
            handleRecv(true, false);
            বিরতি;

        ক্ষেত্র SysNBWait:
            handleRecv(false, false);
            বিরতি;
        ক্ষেত্র SysReplyRecv: {
            cptr_t reply = getRegister(গ্রন্থি_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            যদি (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                বিরতি;
            }
            handleRecv(true, true);
            বিরতি;
        }

        ক্ষেত্র SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            যদি (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                বিরতি;
            }
            handleRecv(true, true);
            বিরতি;
        }

        ক্ষেত্র SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(গ্রন্থি_STATE(ksCurThread), replyRegister));
            যদি (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                বিরতি;
            }
            handleRecv(true, false);
            বিরতি;
#endif
        ক্ষেত্র SysNBRecv:
            handleRecv(false, true);
            বিরতি;

        ক্ষেত্র SysYield:
            handleYield();
            বিরতি;

        পূর্বনির্ধারিত:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    ফেরত EXCEPTION_NONE;
}
