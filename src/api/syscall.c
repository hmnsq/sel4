/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <arch/benchmark.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/faults.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/faulthandler.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#ಅನ್ತರ್ಭಾವ <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#ಅನ್ತರ್ಭಾವ <mode/machine/debug.h>
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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ checkInterrupt(bool_t was_interrupt_entry)
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ checkInterrupt(ಶೂನ್ಯ)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    ಯದಿ (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } ಅನ್ಯಥಾ {
#ifdef CONFIG_IRQ_REPORTING
        ಯದಿ (was_interrupt_entry) {
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

exception_t handleInterruptEntry(ಶೂನ್ಯ)
{
#ifdef CONFIG_KERNEL_MCS
    ಯದಿ (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    ಯದಿ (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    ಯದಿ (w == SysDebugPutChar) {
        kernel_putchar(getRegister(ಗಂಟು_STATE(ksCurThread), capRegister));
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }
    ಯದಿ (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    ಯದಿ (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = ಗಂಟು_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    ಯದಿ (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = ಗಂಟು_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }
    ಯದಿ (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ಗಂಟು_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(ಗಂಟು_STATE(ksCurThread), capRegister, cap_type);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    ಯದಿ (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ಧ್ರುವ ವರ್ಣ *name;
        word_t len;
        word_t cptr = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ಗಂಟು_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        ಯದಿ (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ಧ್ರುವ ವರ್ಣ *)(lookupIPCBuffer(true, ಗಂಟು_STATE(ksCurThread)) + 1);
        ಯದಿ (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_ಸಂದೇಶ_Max_Length * ಪರಿಮಾಣ(word_t));
        ಯದಿ (len == se_L4_ಸಂದೇಶ_Max_Length * ಪರಿಮಾಣ(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    ಯದಿ (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ಗಂಟು_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        ಯದಿ (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(ಗಂಟು_STATE(ksCurThread), capRegister, affinity);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }
    ಯದಿ (w == SysDebugSendIPI) {
        ಪ್ರತ್ಯಾಗಮ handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    ಯದಿ (w == SysDebugRun) {
        ((ಶೂನ್ಯ (*)(ಶೂನ್ಯ *))getRegister(ಗಂಟು_STATE(ksCurThread), capRegister))((ಶೂನ್ಯ *)getRegister(ಗಂಟು_STATE(ksCurThread),
                                                                                                  ಸಂದೇಶ_Info_Register));
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    ಯದಿ (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
        ಯದಿ (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, ರಿಕ್ತ) | ((uint64_t)getSyscallArg(1, ರಿಕ್ತ) << 32);
        } ಅನ್ಯಥಾ {
            val = getSyscallArg(0, ರಿಕ್ತ);
        }
        x86_wrmsr(reg, val);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    } ಅನ್ಯಥಾ ಯದಿ (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ = 1;
        ಯದಿ (CONFIG_WORD_SIZE == 32) {
            setMR(ಗಂಟು_STATE(ksCurThread), ರಿಕ್ತ, 0, val & 0xffffffff);
            setMR(ಗಂಟು_STATE(ksCurThread), ರಿಕ್ತ, 1, val >> 32);
            ಅಂಕೆ++;
        } ಅನ್ಯಥಾ {
            setMR(ಗಂಟು_STATE(ksCurThread), ರಿಕ್ತ, 0, val);
        }
        setRegister(ಗಂಟು_STATE(ksCurThread), ಸಂದೇಶ_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, ಅಂಕೆ)));
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    ವಿಕಲ್ಪ (w) {
    ಪಕ್ಷ SysBenchmarkFlushCaches:
        ಪ್ರತ್ಯಾಗಮ handle_SysBenchmarkFlushCaches();
    ಪಕ್ಷ SysBenchmarkResetLog:
        ಪ್ರತ್ಯಾಗಮ handle_SysBenchmarkResetLog();
    ಪಕ್ಷ SysBenchmarkFinalizeLog:
        ಪ್ರತ್ಯಾಗಮ handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ಪಕ್ಷ SysBenchmarkSetLogBuffer:
        ಪ್ರತ್ಯಾಗಮ handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    ಪಕ್ಷ SysBenchmarkGetThreadUtilisation:
        ಪ್ರತ್ಯಾಗಮ handle_SysBenchmarkGetThreadUtilisation();
    ಪಕ್ಷ SysBenchmarkResetThreadUtilisation:
        ಪ್ರತ್ಯಾಗಮ handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    ಪಕ್ಷ SysBenchmarkDumpAllThreadsUtilisation:
        ಪ್ರತ್ಯಾಗಮ handle_SysBenchmarkDumpAllThreadsUtilisation();
    ಪಕ್ಷ SysBenchmarkResetAllThreadsUtilisation:
        ಪ್ರತ್ಯಾಗಮ handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    ಪಕ್ಷ SysBenchmarkNullSyscall:
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    ಸ್ವಭಾವ:
        ಭಙ್ಗ; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        ಯದಿ (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            ಪ್ರತ್ಯಾಗಮ Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(ಗಂಟು_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(ಗಂಟು_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(ಗಂಟು_STATE(ksCurThread), vm_faultType);
        ಯದಿ (status != EXCEPTION_NONE)
        {
            handleFault(ಗಂಟು_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ಸ್ಥಿರ exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#ಅನ್ಯಥಾ
ಸ್ಥಿರ exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = ಗಂಟು_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, ಸಂದೇಶ_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        ಯದಿ (isBlocking) {
            handleFault(thread);
        }

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    ಯದಿ (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        ಯದಿ (isBlocking) {
            handleFault(thread);
        }
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    ಯದಿ (unlikely(length > n_ಸಂದೇಶ_Registers && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_ಸಂದೇಶ_Registers);
        length = n_ಸಂದೇಶ_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#ಅನ್ಯಥಾ
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    ಯದಿ (unlikely(status == EXCEPTION_PREEMPTED)) {
        ಪ್ರತ್ಯಾಗಮ status;
    }

    ಯದಿ (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        ಯದಿ (isCall) {
            replyFromKernel_error(thread);
        }
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    ಯದಿ (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        ಯದಿ (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ lookupCap_ret_t lookupReply(ಶೂನ್ಯ)
{
    word_t replyCPtr = getRegister(ಗಂಟು_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(ಗಂಟು_STATE(ksCurThread), replyCPtr);
    ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        ಪ್ರತ್ಯಾಗಮ lu_ret;
    }

    ಯದಿ (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        ಪ್ರತ್ಯಾಗಮ lu_ret;
    }

    ಪ್ರತ್ಯಾಗಮ lu_ret;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಶೂನ್ಯ handleReply(ಶೂನ್ಯ)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    ವಿಕಲ್ಪ (cap_get_capType(callerCap)) {
    ಪಕ್ಷ cap_reply_cap: {
        tcb_t *caller;

        ಯದಿ (cap_reply_cap_get_capReplyMaster(callerCap)) {
            ಭಙ್ಗ;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        ಪ್ರತಿಜ್ಞಾ(caller != ಗಂಟು_STATE(ksCurThread));
        doReplyTransfer(ಗಂಟು_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಪಕ್ಷ cap_null_cap:
        /* Do nothing when no caller is pending */
        ಪ್ರತ್ಯಾಗಮ;

    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಶೂನ್ಯ handleRecv(bool_t isBlocking, bool_t canReply)
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಶೂನ್ಯ handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(ಗಂಟು_STATE(ksCurThread), epCPtr);

    ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ಗಂಟು_STATE(ksCurThread));
        ಪ್ರತ್ಯಾಗಮ;
    }

    ವಿಕಲ್ಪ (cap_get_capType(lu_ret.cap)) {
    ಪಕ್ಷ cap_endpoint_cap:
        ಯದಿ (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ಗಂಟು_STATE(ksCurThread));
            ಭಙ್ಗ;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        ಯದಿ (canReply) {
            lu_ret = lookupReply();
            ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(ಗಂಟು_STATE(ksCurThread));
                ಪ್ರತ್ಯಾಗಮ;
            } ಅನ್ಯಥಾ {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(ಗಂಟು_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#ಅನ್ಯಥಾ
        deleteCallerCap(ಗಂಟು_STATE(ksCurThread));
        receiveIPC(ಗಂಟು_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        ಯದಿ (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != ಗಂಟು_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ಗಂಟು_STATE(ksCurThread));
            ಭಙ್ಗ;
        }

        receiveSignal(ಗಂಟು_STATE(ksCurThread), lu_ret.cap, isBlocking);
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ಗಂಟು_STATE(ksCurThread));
        ಭಙ್ಗ;
    }
}

#ifdef CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ mcsPreemptionPoint(ಶೂನ್ಯ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    ಯದಿ (isSchedulable(ಗಂಟು_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } ಅನ್ಯಥಾ ಯದಿ (sc_active(ಗಂಟು_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(ಗಂಟು_STATE(ksConsumed), false);
    } ಅನ್ಯಥಾ {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        ಗಂಟು_STATE(ksConsumed) = 0;
    }
}
#ಅನ್ಯಥಾ
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

ಸ್ಥಿರ ಶೂನ್ಯ handleYield(ಶೂನ್ಯ)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = ಗಂಟು_STATE(ksCurSC)->scConsumed + ಗಂಟು_STATE(ksConsumed);
    refill_t head = *refill_head(ಗಂಟು_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    ಗಂಟು_STATE(ksCurSC)->scConsumed = consumed;
#ಅನ್ಯಥಾ
    tcbSchedDequeue(ಗಂಟು_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        ವಿಕಲ್ಪ (syscall)
        {
        ಪಕ್ಷ SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(ಗಂಟು_STATE(ksCurThread), capRegister));
            ಯದಿ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            ಭಙ್ಗ;

        ಪಕ್ಷ SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(ಗಂಟು_STATE(ksCurThread), capRegister));
            ಯದಿ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            ಭಙ್ಗ;

        ಪಕ್ಷ SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(ಗಂಟು_STATE(ksCurThread), capRegister));
            ಯದಿ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            ಭಙ್ಗ;

        ಪಕ್ಷ SysRecv:
            handleRecv(true, true);
            ಭಙ್ಗ;
#ifndef CONFIG_KERNEL_MCS
        ಪಕ್ಷ SysReply:
            handleReply();
            ಭಙ್ಗ;

        ಪಕ್ಷ SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            ಭಙ್ಗ;

#ಅನ್ಯಥಾ /* CONFIG_KERNEL_MCS */
        ಪಕ್ಷ SysWait:
            handleRecv(true, false);
            ಭಙ್ಗ;

        ಪಕ್ಷ SysNBWait:
            handleRecv(false, false);
            ಭಙ್ಗ;
        ಪಕ್ಷ SysReplyRecv: {
            cptr_t reply = getRegister(ಗಂಟು_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            ಯದಿ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                ಭಙ್ಗ;
            }
            handleRecv(true, true);
            ಭಙ್ಗ;
        }

        ಪಕ್ಷ SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            ಯದಿ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                ಭಙ್ಗ;
            }
            handleRecv(true, true);
            ಭಙ್ಗ;
        }

        ಪಕ್ಷ SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(ಗಂಟು_STATE(ksCurThread), replyRegister));
            ಯದಿ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                ಭಙ್ಗ;
            }
            handleRecv(true, false);
            ಭಙ್ಗ;
#endif
        ಪಕ್ಷ SysNBRecv:
            handleRecv(false, true);
            ಭಙ್ಗ;

        ಪಕ್ಷ SysYield:
            handleYield();
            ಭಙ್ಗ;

        ಸ್ವಭಾವ:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
