/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <benchmark/benchmark.h>
#include <arch/benchmark.h>
#include <benchmark/benchmark_track.h>
#include <benchmark/benchmark_utilisation.h>
#include <api/syscall.h>
#include <api/failures.h>
#include <api/faults.h>
#include <kernel/cspace.h>
#include <kernel/faulthandler.h>
#include <kernel/thread.h>
#include <kernel/vspace.h>
#include <machine/io.h>
#include <plat/machine/hardware.h>
#include <object/interrupt.h>
#include <model/statedata.h>
#include <string.h>
#include <kernel/traps.h>
#include <arch/machine.h>
#ifdef ENABLE_SMP_SUPPORT
#include <smp/ipi.h>
#endif
#ifdef CONFIG_DEBUG_BUILD
#include <arch/machine/capdl.h>
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
#include <mode/machine/debug.h>
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
static anta_hstha void checkInterrupt(bool_t was_interrupt_entry)
#ᱵᱟᱝ
static anta_hstha void checkInterrupt(void)
#endif
{
    irq_t irq;

    irq = getActiveIRQ();
    ᱡᱩᱫᱤ (IRQT_TO_IRQ(irq) != IRQT_TO_IRQ(irqInvalid)) {
        handleInterrupt(irq);
    } ᱵᱟᱝ {
#ifdef CONFIG_IRQ_REPORTING
        ᱡᱩᱫᱤ (was_interrupt_entry) {
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

exception_t handleInterruptEntry(void)
{
#ifdef CONFIG_KERNEL_MCS
    ᱡᱩᱫᱤ (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#endif

    checkInterrupt(/* was_interrupt_entry */ true);

#ifdef CONFIG_KERNEL_MCS
    ᱡᱩᱫᱤ (SMP_TERNARY(clh_is_self_in_queue(), 1)) {
#endif
        schedule();
        activateThread();
#ifdef CONFIG_KERNEL_MCS
    }
#endif

    ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(word_t w)
{
#ifdef CONFIG_PRINTING
    ᱡᱩᱫᱤ (w == SysDebugPutChar) {
        kernel_putchar(getRegister(NODE_STATE(ksCurThread), capRegister));
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }
    ᱡᱩᱫᱤ (w == SysDebugDumpScheduler) {
#ifdef CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#endif
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }
#endif
#ifdef CONFIG_DEBUG_BUILD
    ᱡᱩᱫᱤ (w == SysDebugHalt) {
        tcb_t *UNUSED tptr = NODE_STATE(ksCurThread);
        printf("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    ᱡᱩᱫᱤ (w == SysDebugSnapshot) {
        tcb_t *UNUSED tptr = NODE_STATE(ksCurThread);
        printf("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }
    ᱡᱩᱫᱤ (w == SysDebugCapIdentify) {
        word_t cptr = getRegister(NODE_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(NODE_STATE(ksCurThread), cptr);
        word_t cap_type = cap_get_capType(lu_ret.cap);
        setRegister(NODE_STATE(ksCurThread), capRegister, cap_type);
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }

    ᱡᱩᱫᱤ (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        const var_na *name;
        word_t len;
        word_t cptr = getRegister(NODE_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(NODE_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        ᱡᱩᱫᱤ (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (const var_na *)(lookupIPCBuffer(true, NODE_STATE(ksCurThread)) + 1);
        ᱡᱩᱫᱤ (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, seL4_MsgMaxLength * parimaa_na(word_t));
        ᱡᱩᱫᱤ (len == seL4_MsgMaxLength * parimaa_na(word_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        setThreadName(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }
#ifdef ENABLE_SMP_SUPPORT
    ᱡᱩᱫᱤ (w == SysDebugGetThreadAffinity) {
        word_t cptr = getRegister(NODE_STATE(ksCurThread), capRegister);
        lookupCapAndSlot_ret_t lu_ret = lookupCapAndSlot(NODE_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        word_t cap_type = cap_get_capType(lu_ret.cap);
        ᱡᱩᱫᱤ (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        word_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        setRegister(NODE_STATE(ksCurThread), capRegister, affinity);
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }
    ᱡᱩᱫᱤ (w == SysDebugSendIPI) {
        ᱨᱩᱣᱟᱹᱲ handle_SysDebugSendIPI();
    }
#endif /* ENABLE_SMP_SUPPORT */
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DANGEROUS_CODE_INJECTION
    ᱡᱩᱫᱤ (w == SysDebugRun) {
        ((void (*)(void *))getRegister(NODE_STATE(ksCurThread), capRegister))((void *)getRegister(NODE_STATE(ksCurThread),
                                                                                                  msgInfoRegister));
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_KERNEL_X86_DANGEROUS_MSR
    ᱡᱩᱫᱤ (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = getRegister(NODE_STATE(ksCurThread), capRegister);
        ᱡᱩᱫᱤ (CONFIG_WORD_SIZE == 32) {
            val = (uint64_t)getSyscallArg(0, NULL) | ((uint64_t)getSyscallArg(1, NULL) << 32);
        } ᱵᱟᱝ {
            val = getSyscallArg(0, NULL);
        }
        x86_wrmsr(reg, val);
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    } ᱵᱟᱝ ᱡᱩᱫᱤ (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = getRegister(NODE_STATE(ksCurThread), capRegister);
        val = x86_rdmsr(reg);
        puur_naa_nka num = 1;
        ᱡᱩᱫᱤ (CONFIG_WORD_SIZE == 32) {
            setMR(NODE_STATE(ksCurThread), NULL, 0, val & 0xffffffff);
            setMR(NODE_STATE(ksCurThread), NULL, 1, val >> 32);
            num++;
        } ᱵᱟᱝ {
            setMR(NODE_STATE(ksCurThread), NULL, 0, val);
        }
        setRegister(NODE_STATE(ksCurThread), msgInfoRegister, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, num)));
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }
#endif

#ifdef CONFIG_ENABLE_BENCHMARKS
    switch (w) {
    pak_sa SysBenchmarkFlushCaches:
        ᱨᱩᱣᱟᱹᱲ handle_SysBenchmarkFlushCaches();
    pak_sa SysBenchmarkResetLog:
        ᱨᱩᱣᱟᱹᱲ handle_SysBenchmarkResetLog();
    pak_sa SysBenchmarkFinalizeLog:
        ᱨᱩᱣᱟᱹᱲ handle_SysBenchmarkFinalizeLog();
#ifdef CONFIG_KERNEL_LOG_BUFFER
    pak_sa SysBenchmarkSetLogBuffer:
        ᱨᱩᱣᱟᱹᱲ handle_SysBenchmarkSetLogBuffer();
#endif /* CONFIG_KERNEL_LOG_BUFFER */
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    pak_sa SysBenchmarkGetThreadUtilisation:
        ᱨᱩᱣᱟᱹᱲ handle_SysBenchmarkGetThreadUtilisation();
    pak_sa SysBenchmarkResetThreadUtilisation:
        ᱨᱩᱣᱟᱹᱲ handle_SysBenchmarkResetThreadUtilisation();
#ifdef CONFIG_DEBUG_BUILD
    pak_sa SysBenchmarkDumpAllThreadsUtilisation:
        ᱨᱩᱣᱟᱹᱲ handle_SysBenchmarkDumpAllThreadsUtilisation();
    pak_sa SysBenchmarkResetAllThreadsUtilisation:
        ᱨᱩᱣᱟᱹᱲ handle_SysBenchmarkResetAllThreadsUtilisation();
#endif /* CONFIG_DEBUG_BUILD */
#endif /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    pak_sa SysBenchmarkNullSyscall:
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    default:
        bha_nga; /* syscall is not for benchmarking */
    } /* end switch(w) */
#endif /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ifdef CONFIG_SET_TLS_BASE_SELF
        ᱡᱩᱫᱤ (w == SysSetTLSBase)
        {
            word_t tls_base = getRegister(NODE_STATE(ksCurThread), capRegister);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            ᱨᱩᱣᱟᱹᱲ Arch_setTLSRegister(tls_base);
        }
#endif
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(NODE_STATE(ksCurThread));
    })

    schedule();
    activateThread();

    ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
}

exception_t handleUserLevelFault(word_t w_a, word_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(NODE_STATE(ksCurThread));
    })
    schedule();
    activateThread();

    ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t status = handleVMFault(NODE_STATE(ksCurThread), vm_faultType);
        ᱡᱩᱫᱤ (status != EXCEPTION_NONE)
        {
            handleFault(NODE_STATE(ksCurThread));
        }
    })

    schedule();
    activateThread();

    ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
static exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t firstPhase, cptr_t cptr)
#ᱵᱟᱝ
static exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#endif
{
    seL4_MessageInfo_t info;
    lookupCapAndSlot_ret_t lu_ret;
    word_t *buffer;
    exception_t status;
    word_t length;
    tcb_t *thread;

    thread = NODE_STATE(ksCurThread);

    info = messageInfoFromWord(getRegister(thread, msgInfoRegister));
#ifndef CONFIG_KERNEL_MCS
    cptr_t cptr = getRegister(thread, capRegister);
#endif

    /* faulting section */
    lu_ret = lookupCapAndSlot(thread, cptr);

    ᱡᱩᱫᱤ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        ᱡᱩᱫᱤ (isBlocking) {
            handleFault(thread);
        }

        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }

    buffer = lookupIPCBuffer(false, thread);

    status = lookupExtraCaps(thread, buffer, info);

    ᱡᱩᱫᱤ (unlikely(status != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        ᱡᱩᱫᱤ (isBlocking) {
            handleFault(thread);
        }
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    length = seL4_MessageInfo_get_length(info);
    ᱡᱩᱫᱤ (unlikely(length > n_msgRegisters && !buffer)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_msgRegisters);
        length = n_msgRegisters;
    }
#ifdef CONFIG_KERNEL_MCS
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, firstPhase, buffer);
#ᱵᱟᱝ
    status = decodeInvocation(seL4_MessageInfo_get_label(info), length,
                              cptr, lu_ret.slot, lu_ret.cap,
                              isBlocking, isCall, buffer);
#endif

    ᱡᱩᱫᱤ (unlikely(status == EXCEPTION_PREEMPTED)) {
        ᱨᱩᱣᱟᱹᱲ status;
    }

    ᱡᱩᱫᱤ (unlikely(status == EXCEPTION_SYSCALL_ERROR)) {
        ᱡᱩᱫᱤ (isCall) {
            replyFromKernel_error(thread);
        }
        ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
    }

    ᱡᱩᱫᱤ (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        ᱡᱩᱫᱤ (isCall) {
            replyFromKernel_success_empty(thread);
        }
        setThreadState(thread, ThreadState_Running);
    }

    ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
static anta_hstha lookupCap_ret_t lookupReply(void)
{
    word_t replyCPtr = getRegister(NODE_STATE(ksCurThread), replyRegister);
    lookupCap_ret_t lu_ret = lookupCap(NODE_STATE(ksCurThread), replyCPtr);
    ᱡᱩᱫᱤ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        ᱨᱩᱣᱟᱹᱲ lu_ret;
    }

    ᱡᱩᱫᱤ (unlikely(cap_get_capType(lu_ret.cap) != cap_reply_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(replyCPtr, true);
        lu_ret.status = EXCEPTION_FAULT;
        ᱨᱩᱣᱟᱹᱲ lu_ret;
    }

    ᱨᱩᱣᱟᱹᱲ lu_ret;
}
#ᱵᱟᱝ
static void handleReply(void)
{
    cte_t *callerSlot;
    cap_t callerCap;

    callerSlot = TCB_PTR_CTE_PTR(NODE_STATE(ksCurThread), tcbCaller);
    callerCap = callerSlot->cap;

    switch (cap_get_capType(callerCap)) {
    pak_sa cap_reply_cap: {
        tcb_t *caller;

        ᱡᱩᱫᱤ (cap_reply_cap_get_capReplyMaster(callerCap)) {
            bha_nga;
        }
        caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        assert(caller != NODE_STATE(ksCurThread));
        doReplyTransfer(NODE_STATE(ksCurThread), caller, callerSlot,
                        cap_reply_cap_get_capReplyCanGrant(callerCap));
        ᱨᱩᱣᱟᱹᱲ;
    }

    pak_sa cap_null_cap:
        /* Do nothing when no caller is pending */
        ᱨᱩᱣᱟᱹᱲ;

    default:
        bha_nga;
    }

    fail("handleReply: invalid caller cap");
}
#endif

#ifdef CONFIG_KERNEL_MCS
static void handleRecv(bool_t isBlocking, bool_t canReply)
#ᱵᱟᱝ
static void handleRecv(bool_t isBlocking)
#endif
{
    word_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = getRegister(NODE_STATE(ksCurThread), capRegister);

    lu_ret = lookupCap(NODE_STATE(ksCurThread), epCPtr);

    ᱡᱩᱫᱤ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(NODE_STATE(ksCurThread));
        ᱨᱩᱣᱟᱹᱲ;
    }

    switch (cap_get_capType(lu_ret.cap)) {
    pak_sa cap_endpoint_cap:
        ᱡᱩᱫᱤ (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(NODE_STATE(ksCurThread));
            bha_nga;
        }

#ifdef CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t reply_cap = cap_null_cap_new();
        ᱡᱩᱫᱤ (canReply) {
            lu_ret = lookupReply();
            ᱡᱩᱫᱤ (lu_ret.status != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(NODE_STATE(ksCurThread));
                ᱨᱩᱣᱟᱹᱲ;
            } ᱵᱟᱝ {
                reply_cap = lu_ret.cap;
            }
        }
        receiveIPC(NODE_STATE(ksCurThread), ep_cap, isBlocking, reply_cap);
#ᱵᱟᱝ
        deleteCallerCap(NODE_STATE(ksCurThread));
        receiveIPC(NODE_STATE(ksCurThread), lu_ret.cap, isBlocking);
#endif
        bha_nga;

    pak_sa cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        ᱡᱩᱫᱤ (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != NODE_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(NODE_STATE(ksCurThread));
            bha_nga;
        }

        receiveSignal(NODE_STATE(ksCurThread), lu_ret.cap, isBlocking);
        bha_nga;
    }
    default:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(NODE_STATE(ksCurThread));
        bha_nga;
    }
}

#ifdef CONFIG_KERNEL_MCS
static anta_hstha void mcsPreemptionPoint(void)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    ᱡᱩᱫᱤ (isSchedulable(NODE_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } ᱵᱟᱝ ᱡᱩᱫᱤ (sc_active(NODE_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(NODE_STATE(ksConsumed), false);
    } ᱵᱟᱝ {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        NODE_STATE(ksConsumed) = 0;
    }
}
#ᱵᱟᱝ
#define handleRecv(isBlocking, canReply) handleRecv(isBlocking)
#define mcsPreemptionPoint()
#define handleInvocation(isCall, isBlocking, canDonate, firstPhase, cptr) handleInvocation(isCall, isBlocking)
#endif

static void handleYield(void)
{
#ifdef CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = NODE_STATE(ksCurSC)->scConsumed + NODE_STATE(ksConsumed);
    refill_t head = *refill_head(NODE_STATE(ksCurSC));
    chargeBudget(head.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    NODE_STATE(ksCurSC)->scConsumed = consumed;
#ᱵᱟᱝ
    tcbSchedDequeue(NODE_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#endif
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        switch (syscall)
        {
        pak_sa SysSend:
            ret = handleInvocation(false, true, false, false, getRegister(NODE_STATE(ksCurThread), capRegister));
            ᱡᱩᱫᱤ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }

            bha_nga;

        pak_sa SysNBSend:
            ret = handleInvocation(false, false, false, false, getRegister(NODE_STATE(ksCurThread), capRegister));
            ᱡᱩᱫᱤ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            bha_nga;

        pak_sa SysCall:
            ret = handleInvocation(true, true, true, false, getRegister(NODE_STATE(ksCurThread), capRegister));
            ᱡᱩᱫᱤ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
            }
            bha_nga;

        pak_sa SysRecv:
            handleRecv(true, true);
            bha_nga;
#ifndef CONFIG_KERNEL_MCS
        pak_sa SysReply:
            handleReply();
            bha_nga;

        pak_sa SysReplyRecv:
            handleReply();
            handleRecv(true, true);
            bha_nga;

#ᱵᱟᱝ /* CONFIG_KERNEL_MCS */
        pak_sa SysWait:
            handleRecv(true, false);
            bha_nga;

        pak_sa SysNBWait:
            handleRecv(false, false);
            bha_nga;
        pak_sa SysReplyRecv: {
            cptr_t reply = getRegister(NODE_STATE(ksCurThread), replyRegister);
            ret = handleInvocation(false, false, true, true, reply);
            /* reply is not preemptible, but to ease verification we check explicitly */
            ᱡᱩᱫᱤ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                bha_nga;
            }
            handleRecv(true, true);
            bha_nga;
        }

        pak_sa SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            ᱡᱩᱫᱤ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                bha_nga;
            }
            handleRecv(true, true);
            bha_nga;
        }

        pak_sa SysNBSendWait:
            ret = handleInvocation(false, false, true, true, getRegister(NODE_STATE(ksCurThread), replyRegister));
            ᱡᱩᱫᱤ (unlikely(ret != EXCEPTION_NONE)) {
                mcsPreemptionPoint();
                checkInterrupt(/* was_interrupt_entry */ false);
                bha_nga;
            }
            handleRecv(true, false);
            bha_nga;
#endif
        pak_sa SysNBRecv:
            handleRecv(false, true);
            bha_nga;

        pak_sa SysYield:
            handleYield();
            bha_nga;

        default:
            fail("Invalid syscall");
        }

    })

    schedule();
    activateThread();

    ᱨᱩᱣᱟᱹᱲ EXCEPTION_NONE;
}
