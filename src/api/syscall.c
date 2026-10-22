/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark.h>
#ଅନ୍ତର୍ଭାଵ <arch/benchmark.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_track.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_utilisation.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <api/faults.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ଅନ୍ତର୍ଭାଵ <kernel/faulthandler.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/hardware.h>
#ଅନ୍ତର୍ଭାଵ <object/interrupt.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <kernel/traps.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#ଅନ୍ତର୍ଭାଵ <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#ଅନ୍ତର୍ଭାଵ <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#ଅନ୍ତର୍ଭାଵ <mode/machine/debug.h>
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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ checkInterrupt(bool_t was_interrupt_entry)
#ଅନ୍ୟଥା
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ checkInterrupt(ଶୂନ୍ୟ)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    ଯଦି (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } ଅନ୍ୟଥା {
#ifdef CONFIG_IRQ_REPORTING
        ଯଦି (was_interrupt_entry) {
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

exception_t handleInterruptEntry(ଶୂନ୍ୟ)
{
#ifdef CONFIG_KERNEL_MCS
    ଯଦି (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    ଯଦି (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    ଯଦି (w == SysDebugPutChar) {
        kernel_putchar(getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister));
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }
    ଯଦି (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    ଯଦି (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = ଗଣ୍ଠି_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    ଯଦି (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = ଗଣ୍ଠି_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }
    ଯଦି (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ଗଣ୍ଠି_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister, cap_type);
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    ଯଦି (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ଅଚଳ ଅକ୍ଷର *name;
        word_t len;
        word_t cptr = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ଗଣ୍ଠି_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        ଯଦି (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ଅଚଳ ଅକ୍ଷର *)(lookupIPCBuffer(true, ଗଣ୍ଠି_STATE(ksCurThread)) + 1);
        ଯଦି (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_ସନ୍ଦେଶ_Max_Length * ଆକାର(word_t));
        ଯଦି (len == se_L4_ସନ୍ଦେଶ_Max_Length * ଆକାର(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    ଯଦି (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(ଗଣ୍ଠି_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        ଯଦି (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister, affinity);
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }
    ଯଦି (w == SysDebugSendIPI) {
        ଫେରନ୍ତୁ handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    ଯଦି (w == SysDebugRun) {
        ((ଶୂନ୍ୟ (*)(ଶୂନ୍ୟ *))getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister))((ଶୂନ୍ୟ *)getRegister(ଗଣ୍ଠି_STATE(ksCurThread),
                                                                                                  ସନ୍ଦେଶ_Info_Register));
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    ଯଦି (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
        ଯଦି (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, ଖାଲି) | ((uint64_t)getSyscallArg(1, ଖାଲି) << 32);
        } ଅନ୍ୟଥା {
            val = getSyscallArg(0, ଖାଲି);
        }
        x86_wrmsr(reg, val);
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    } ଅନ୍ୟଥା ଯଦି (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ = 1;
        ଯଦି (CONFIG_WORD_SIZE == 32) {
            setMR(ଗଣ୍ଠି_STATE(ksCurThread), ଖାଲି, 0, val & 0xffffffff);
            setMR(ଗଣ୍ଠି_STATE(ksCurThread), ଖାଲି, 1, val >> 32);
            ଅଙ୍କ++;
        } ଅନ୍ୟଥା {
            setMR(ଗଣ୍ଠି_STATE(ksCurThread), ଖାଲି, 0, val);
        }
        setRegister(ଗଣ୍ଠି_STATE(ksCurThread), ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, ଅଙ୍କ)));
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    ବାଛ (w) {
    ସ୍ଥିତି SysBenchmarkFlushCaches:
        ଫେରନ୍ତୁ handle_SysBenchmarkFlushCaches();
    ସ୍ଥିତି SysBenchmarkResetLog:
        ଫେରନ୍ତୁ handle_SysBenchmarkResetLog();
    ସ୍ଥିତି SysBenchmarkFinalizeLog:
        ଫେରନ୍ତୁ handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    ସ୍ଥିତି SysBenchmarkSetLogBuffer:
        ଫେରନ୍ତୁ handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    ସ୍ଥିତି SysBenchmarkGetThreadUtilisation:
        ଫେରନ୍ତୁ handle_SysBenchmarkGetThreadUtilisation();
    ସ୍ଥିତି SysBenchmarkResetThreadUtilisation:
        ଫେରନ୍ତୁ handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    ସ୍ଥିତି SysBenchmarkDumpAllThreadsUtilisation:
        ଫେରନ୍ତୁ handle_SysBenchmarkDumpAllThreadsUtilisation();
    ସ୍ଥିତି SysBenchmarkResetAllThreadsUtilisation:
        ଫେରନ୍ତୁ handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    ସ୍ଥିତି SysBenchmarkNullSyscall:
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        ଯଦି (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            ଫେରନ୍ତୁ Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(ଗଣ୍ଠି_STATE(ksCurThread), vm_faultType);
        ଯଦି (status != EXCEPTION_NONE)
        {
            handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ସ୍ଥିର exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#ଅନ୍ୟଥା
ସ୍ଥିର exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = ଗଣ୍ଠି_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, ସନ୍ଦେଶ_Info_Register));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    ଯଦି (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        ଯଦି (isBlocking) {
            handleFault(thread);
        }

        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    ଯଦି (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        ଯଦି (isBlocking) {
            handleFault(thread);
        }
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    ଯଦି (unlikely(length > n_ସନ୍ଦେଶ_Registers && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_ସନ୍ଦେଶ_Registers);
        length = n_ସନ୍ଦେଶ_Registers;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#ଅନ୍ୟଥା
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    ଯଦି (unlikely(status == EXCEPTION_PREEMPTED)) {
        ଫେରନ୍ତୁ status;
    }

    ଯଦି (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        ଯଦି (isCall) {
            replyFromKernel_error(thread);
        }
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    ଯଦି (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        ଯଦି (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ସ୍ଥିର ଧାଡ଼ିଭିତରେ lookupCap_ret_t lookupReply(ଶୂନ୍ୟ)
{
    word_t replyCPtr = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(ଗଣ୍ଠି_STATE(ksCurThread), replyCPtr);
    ଯଦି (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        ଫେରନ୍ତୁ lu_ret;
    }

    ଯଦି (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        ଫେରନ୍ତୁ lu_ret;
    }

    ଫେରନ୍ତୁ lu_ret;
}
#ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ handleReply(ଶୂନ୍ୟ)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(ଗଣ୍ଠି_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    ବାଛ (cap_get_capType(callerCap)) {
    ସ୍ଥିତି cap_reply_cap: {
        tcb_t *caller;

        ଯଦି (cap_reply_cap_get_capReplyMaster(callerCap)) {
            ଭାଙ୍ଗ;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        ପ୍ରତିଜ୍ଞା(caller != ଗଣ୍ଠି_STATE(ksCurThread));
        doReplyTransfer(ଗଣ୍ଠି_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        ଫେରନ୍ତୁ;
    }

    ସ୍ଥିତି cap_null_cap:
        /* Do nothing when no caller is pending */
        ଫେରନ୍ତୁ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
ସ୍ଥିର ଶୂନ୍ୟ handleRecv(bool_t isBlocking, bool_t canReply)
#ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(ଗଣ୍ଠି_STATE(ksCurThread), epCPtr);

    ଯଦି (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
        ଫେରନ୍ତୁ;
    }

    ବାଛ (cap_get_capType(lu_ret.cap)) {
    ସ୍ଥିତି cap_endpoint_cap:
        ଯଦି (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
            ଭାଙ୍ଗ;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        ଯଦି (canReply) {
            lu_ret = lookupReply();
            ଯଦି (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
                ଫେରନ୍ତୁ;
            } ଅନ୍ୟଥା {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(ଗଣ୍ଠି_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#ଅନ୍ୟଥା
        deleteCallerCap(ଗଣ୍ଠି_STATE(ksCurThread));
        receiveIPC(ଗଣ୍ଠି_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        ଯଦି (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != ଗଣ୍ଠି_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
            ଭାଙ୍ଗ;
        }

        receiveSignal(ଗଣ୍ଠି_STATE(ksCurThread), lu_ret.cap, isBlocking);
        ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
        ଭାଙ୍ଗ;
    }
}

#ifdef CONFIG_KERNEL_MCS
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ mcsPreemptionPoint(ଶୂନ୍ୟ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    ଯଦି (isSchedulable(ଗଣ୍ଠି_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } ଅନ୍ୟଥା ଯଦି (sc_active(ଗଣ୍ଠି_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(ଗଣ୍ଠି_STATE(ksConsumed), false);
    } ଅନ୍ୟଥା {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        ଗଣ୍ଠି_STATE(ksConsumed) = 0;
    }
}
#ଅନ୍ୟଥା
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

ସ୍ଥିର ଶୂନ୍ୟ handleYield(ଶୂନ୍ୟ)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = ଗଣ୍ଠି_STATE(ksCurSC)->scConsumed + ଗଣ୍ଠି_STATE(ksConsumed);
    refill_t head = *refill_head(ଗଣ୍ଠି_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    ଗଣ୍ଠି_STATE(ksCurSC)->scConsumed = consumed;
#ଅନ୍ୟଥା
    tcbSchedDequeue(ଗଣ୍ଠି_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        ବାଛ (syscall)
        {
        ସ୍ଥିତି SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister));
            ଯଦି (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            ଭାଙ୍ଗ;

        ସ୍ଥିତି SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister));
            ଯଦି (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            ଭାଙ୍ଗ;

        ସ୍ଥିତି SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(ଗଣ୍ଠି_STATE(ksCurThread), capRegister));
            ଯଦି (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            ଭାଙ୍ଗ;

        ସ୍ଥିତି SysRecv:
            handleRecv(true, true);
            ଭାଙ୍ଗ;
#ifndef CONFIG_KERNEL_MCS
        ସ୍ଥିତି SysReply:
            handleReply();
            ଭାଙ୍ଗ;

        ସ୍ଥିତି SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            ଭାଙ୍ଗ;

#ଅନ୍ୟଥା /* CONFIG_KERNEL_MCS */
        ସ୍ଥିତି SysWait:
            handleRecv(true, false);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି SysNBWait:
            handleRecv(false, false);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି SysReplyRecv: {
            cptr_t reply = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            ଯଦି (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                ଭାଙ୍ଗ;
            }
            handleRecv(true, true);
            ଭାଙ୍ଗ;
        }

        ସ୍ଥିତି SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            ଯଦି (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                ଭାଙ୍ଗ;
            }
            handleRecv(true, true);
            ଭାଙ୍ଗ;
        }

        ସ୍ଥିତି SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(ଗଣ୍ଠି_STATE(ksCurThread), replyRegister));
            ଯଦି (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                ଭାଙ୍ଗ;
            }
            handleRecv(true, false);
            ଭାଙ୍ଗ;
#endif
        ସ୍ଥିତି SysNBRecv:
            handleRecv(false, true);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି SysYield:
            handleYield();
            ଭାଙ୍ଗ;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
