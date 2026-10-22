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
#که_تعریف_شوی ENABLE_SMP_SUPPORT
#ضمن <smp/ipi.h>
#د_که_پای
#که_تعریف_شوی CONFIG_DEBUG_BUILD
#ضمن <arch/machine/capdl.h>
#د_که_پای
#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
#ضمن <mode/machine/debug.h>
#د_که_پای

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#که_تعریف_شوی CONFIG_IRQ_REPORTING
ساكن مضمن فراغ check_مداخله(bool_t was_مداخله_ثبت)
#وإلا
ساكن مضمن فراغ check_مداخله(فراغ)
#د_که_پای
{
    irq_t irq;

    irq = get_فعال_IRQ();
    إذا (IRQT_منزل_IRQ(irq) != IRQT_منزل_IRQ(irqInvalid)) {
        handle_مداخله(irq);
    } وإلا {
#که_تعریف_شوی CONFIG_IRQ_REPORTING
        إذا (was_مداخله_ثبت) {
            userError("Spurious interrupt!");
        }
#د_که_پای
        handleSpuriousIRQ();
    }
}

#که_نه_تعریف_شوی CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#تعریف_کړه check_مداخله(was_مداخله_ثبت) check_مداخله()
#د_که_پای

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_مداخله_ثبت(فراغ)
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_دننه_کتار(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#د_که_پای

    check_مداخله(/* was_interrupt_entry */ true);

#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_دننه_کتار(), 1)) {
#د_که_پای
        مهالوېش();
        activateThread();
#که_تعریف_شوی CONFIG_KERNEL_MCS
    }
#د_که_پای

    أرجع EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(کلمه_t w)
{
#که_تعریف_شوی CONFIG_PRINTING
    إذا (w == SysDebugPutChar) {
        kernel_putchar(get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی));
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugDumpScheduler) {
#که_تعریف_شوی CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#د_که_پای
        أرجع EXCEPTION_NONE;
    }
#د_که_پای
#که_تعریف_شوی CONFIG_DEBUG_BUILD
    إذا (w == SysDebugHalt) {
        tcb_t *ناکارول_شوی tptr = غوټه_STATE(ksCurThread);
        چاپ_کړه("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    إذا (w == SysDebugSnapshot) {
        tcb_t *ناکارول_شوی tptr = غوټه_STATE(ksCurThread);
        چاپ_کړه("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugCapIdentify) {
        کلمه_t cptr = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
        lookup_Cap_And_خونه_ret_t lu_ret = lookup_Cap_And_خونه(غوټه_STATE(ksCurThread), cptr);
        کلمه_t cap_type = cap_get_capType(lu_ret.cap);
        سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی, cap_type);
        أرجع EXCEPTION_NONE;
    }

    إذا (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ثابت حرف *name;
        کلمه_t len;
        کلمه_t cptr = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
        lookup_Cap_And_خونه_ret_t lu_ret = lookup_Cap_And_خونه(غوټه_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        کلمه_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ثابت حرف *)(lookupIPCBuffer(true, غوټه_STATE(ksCurThread)) + 1);
        إذا (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_پیغام_Max_اوږدتیا * حجم(کلمه_t));
        إذا (len == se_L4_پیغام_Max_اوږدتیا * حجم(کلمه_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        سېټ_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        أرجع EXCEPTION_NONE;
    }
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    إذا (w == SysDebugGetThreadAffinity) {
        کلمه_t cptr = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
        lookup_Cap_And_خونه_ret_t lu_ret = lookup_Cap_And_خونه(غوټه_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        کلمه_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        کلمه_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی, affinity);
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugSendIPI) {
        أرجع handle_SysDebugSendIPI();
    }
#د_که_پای /* ENABLE_SMP_SUPPORT */
#د_که_پای /* CONFIG_DEBUG_BUILD */

#که_تعریف_شوی CONFIG_DANGEROUS_کوډ_INJECTION
    إذا (w == Sys_Debug_پرلپسې) {
        ((فراغ (*)(فراغ *))get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی))((فراغ *)get_ثبتوونکی(غوټه_STATE(ksCurThread),
                                                                                                  پیغام_Info_ثبتوونکی));
        أرجع EXCEPTION_NONE;
    }
#د_که_پای

#که_تعریف_شوی CONFIG_KERNEL_X86_DANGEROUS_MSR
    إذا (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
        إذا (CONFIG_کلمه_اندازه == 32) {
            val = (uint64_t)getSyscallArg(0, عدم) | ((uint64_t)getSyscallArg(1, عدم) << 32);
        } وإلا {
            val = getSyscallArg(0, عدم);
        }
        x86_wrmsr(reg, val);
        أرجع EXCEPTION_NONE;
    } وإلا إذا (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
        val = x86_rdmsr(reg);
        صحيح عدد = 1;
        إذا (CONFIG_کلمه_اندازه == 32) {
            سېټ_MR(غوټه_STATE(ksCurThread), عدم, 0, val & 0xffffffff);
            سېټ_MR(غوټه_STATE(ksCurThread), عدم, 1, val >> 32);
            عدد++;
        } وإلا {
            سېټ_MR(غوټه_STATE(ksCurThread), عدم, 0, val);
        }
        سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(se_L4_پیغام_Info_new(0, 0, 0, عدد)));
        أرجع EXCEPTION_NONE;
    }
#د_که_پای

#که_تعریف_شوی CONFIG_ENABLE_BENCHMARKS
    بدل (w) {
    حالة SysBenchmarkFlushCaches:
        أرجع handle_SysBenchmarkFlushCaches();
    حالة SysBenchmarkResetLog:
        أرجع handle_SysBenchmarkResetLog();
    حالة SysBenchmarkFinalizeLog:
        أرجع handle_SysBenchmarkFinalizeLog();
#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
    حالة Sys_Benchmark_سېټ_Log_بفر:
        أرجع handle_Sys_Benchmark_سېټ_Log_بفر();
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */
#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_UTILISATION
    حالة SysBenchmarkGetThreadUtilisation:
        أرجع handle_SysBenchmarkGetThreadUtilisation();
    حالة SysBenchmarkResetThreadUtilisation:
        أرجع handle_SysBenchmarkResetThreadUtilisation();
#که_تعریف_شوی CONFIG_DEBUG_BUILD
    حالة Sys_Benchmark_Dump_All_تارونه_Utilisation:
        أرجع handle_Sys_Benchmark_Dump_All_تارونه_Utilisation();
    حالة Sys_Benchmark_Reset_All_تارونه_Utilisation:
        أرجع handle_Sys_Benchmark_Reset_All_تارونه_Utilisation();
#د_که_پای /* CONFIG_DEBUG_BUILD */
#د_که_پای /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    حالة SysBenchmarkNullSyscall:
        أرجع EXCEPTION_NONE;
    افتراضي:
        اكسر; /* syscall is not for benchmarking */
    } /* end switch(w) */
#د_که_پای /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#که_تعریف_شوی CONFIG_سېټ_TLS_بنسټ_SELF
        إذا (w == Sys_سېټ_TLSBase)
        {
            کلمه_t tls_بنسټ = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            أرجع Arch_سېټ_TLSRegister(tls_بنسټ);
        }
#د_که_پای
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(غوټه_STATE(ksCurThread));
    })

    مهالوېش();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleUserLevelFault(کلمه_t w_a, کلمه_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(غوټه_STATE(ksCurThread));
    })
    مهالوېش();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t حالت = handleVMFault(غوټه_STATE(ksCurThread), vm_faultType);
        إذا (حالت != EXCEPTION_NONE)
        {
            handleFault(غوټه_STATE(ksCurThread));
        }
    })

    مهالوېش();
    activateThread();

    أرجع EXCEPTION_NONE;
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t لومړی_Phase, cptr_t cptr)
#وإلا
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#د_که_پای
{
    se_L4_پیغام_Info_t info;
    lookup_Cap_And_خونه_ret_t lu_ret;
    کلمه_t *بفر;
    exception_t حالت;
    کلمه_t اوږدتیا;
    tcb_t *thread;

    thread = غوټه_STATE(ksCurThread);

    info = پیغام_Info_From_کلمه(get_ثبتوونکی(thread, پیغام_Info_ثبتوونکی));
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    cptr_t cptr = get_ثبتوونکی(thread, cap_ثبتوونکی);
#د_که_پای

    /* faulting section */
    lu_ret = lookup_Cap_And_خونه(thread, cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        إذا (isBlocking) {
            handleFault(thread);
        }

        أرجع EXCEPTION_NONE;
    }

    بفر = lookupIPCBuffer(false, thread);

    حالت = lookupExtraCaps(thread, بفر, info);

    إذا (unlikely(حالت != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        إذا (isBlocking) {
            handleFault(thread);
        }
        أرجع EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    اوږدتیا = se_L4_پیغام_Info_get_اوږدتیا(info);
    إذا (unlikely(اوږدتیا > n_پیغام_Registers && !بفر)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_پیغام_Registers);
        اوږدتیا = n_پیغام_Registers;
    }
#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالت = decodeInvocation(se_L4_پیغام_Info_get_label(info), اوږدتیا,
                              cptr, lu_ret.خونه, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, لومړی_Phase, بفر);
#وإلا
    حالت = decodeInvocation(se_L4_پیغام_Info_get_label(info), اوږدتیا,
                              cptr, lu_ret.خونه, lu_ret.cap,
                              isBlocking, isCall, بفر);
#د_که_پای

    إذا (unlikely(حالت == EXCEPTION_PREEMPTED)) {
        أرجع حالت;
    }

    إذا (unlikely(حالت == EXCEPTION_SYSCALL_ERROR)) {
        إذا (isCall) {
            ځواب_From_Kernel_error(thread);
        }
        أرجع EXCEPTION_NONE;
    }

    إذا (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        إذا (isCall) {
            ځواب_From_Kernel_success_تشه(thread);
        }
        سېټ_Thread_State(thread, ThreadState_Running);
    }

    أرجع EXCEPTION_NONE;
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن مضمن lookupCap_ret_t lookup_ځواب(فراغ)
{
    کلمه_t ځواب_CPtr = get_ثبتوونکی(غوټه_STATE(ksCurThread), ځواب_ثبتوونکی);
    lookupCap_ret_t lu_ret = lookupCap(غوټه_STATE(ksCurThread), ځواب_CPtr);
    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(ځواب_CPtr, true);
        أرجع lu_ret;
    }

    إذا (unlikely(cap_get_capType(lu_ret.cap) != cap_ځواب_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(ځواب_CPtr, true);
        lu_ret.حالت = EXCEPTION_FAULT;
        أرجع lu_ret;
    }

    أرجع lu_ret;
}
#وإلا
ساكن فراغ handle_ځواب(فراغ)
{
    cte_t *caller_خونه;
    cap_t callerCap;

    caller_خونه = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbCaller);
    callerCap = caller_خونه->cap;

    بدل (cap_get_capType(callerCap)) {
    حالة cap_ځواب_cap: {
        tcb_t *caller;

        إذا (cap_ځواب_cap_get_cap_ځواب_Master(callerCap)) {
            اكسر;
        }
        caller = TCB_PTR(cap_ځواب_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        أكد(caller != غوټه_STATE(ksCurThread));
        do_ځواب_Transfer(غوټه_STATE(ksCurThread), caller, caller_خونه,
                        cap_ځواب_cap_get_cap_ځواب_Can_Grant(callerCap));
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
#د_که_پای

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن فراغ handleRecv(bool_t isBlocking, bool_t can_ځواب)
#وإلا
ساكن فراغ handleRecv(bool_t isBlocking)
#د_که_پای
{
    کلمه_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);

    lu_ret = lookupCap(غوټه_STATE(ksCurThread), epCPtr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(غوټه_STATE(ksCurThread));
        أرجع;
    }

    بدل (cap_get_capType(lu_ret.cap)) {
    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(غوټه_STATE(ksCurThread));
            اكسر;
        }

#که_تعریف_شوی CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t ځواب_cap = cap_null_cap_new();
        إذا (can_ځواب) {
            lu_ret = lookup_ځواب();
            إذا (lu_ret.حالت != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(غوټه_STATE(ksCurThread));
                أرجع;
            } وإلا {
                ځواب_cap = lu_ret.cap;
            }
        }
        receiveIPC(غوټه_STATE(ksCurThread), ep_cap, isBlocking, ځواب_cap);
#وإلا
        deleteCallerCap(غوټه_STATE(ksCurThread));
        receiveIPC(غوټه_STATE(ksCurThread), lu_ret.cap, isBlocking);
#د_که_پای
        اكسر;

    حالة cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != غوټه_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(غوټه_STATE(ksCurThread));
            اكسر;
        }

        receiveSignal(غوټه_STATE(ksCurThread), lu_ret.cap, isBlocking);
        اكسر;
    }
    افتراضي:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(غوټه_STATE(ksCurThread));
        اكسر;
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن مضمن فراغ mcs_Preemption_ټکی(فراغ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    إذا (isSchedulable(غوټه_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } وإلا إذا (sc_فعال(غوټه_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(غوټه_STATE(ksConsumed), false);
    } وإلا {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        غوټه_STATE(ksConsumed) = 0;
    }
}
#وإلا
#تعریف_کړه handleRecv(isBlocking, can_ځواب) handleRecv(isBlocking)
#تعریف_کړه mcs_Preemption_ټکی()
#تعریف_کړه handleInvocation(isCall, isBlocking, canDonate, لومړی_Phase, cptr) handleInvocation(isCall, isBlocking)
#د_که_پای

ساكن فراغ handleYield(فراغ)
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = غوټه_STATE(ksCurSC)->scConsumed + غوټه_STATE(ksConsumed);
    refill_t سر = *refill_سر(غوټه_STATE(ksCurSC));
    chargeBudget(سر.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    غوټه_STATE(ksCurSC)->scConsumed = consumed;
#وإلا
    tcb_Sched_له_کتار_ایستل(غوټه_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#د_که_پای
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        بدل (syscall)
        {
        حالة SysSend:
            ret = handleInvocation(false, true, false, false, get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ټکی();
                check_مداخله(/* was_interrupt_entry */ false);
            }

            اكسر;

        حالة SysNBSend:
            ret = handleInvocation(false, false, false, false, get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ټکی();
                check_مداخله(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysCall:
            ret = handleInvocation(true, true, true, false, get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ټکی();
                check_مداخله(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysRecv:
            handleRecv(true, true);
            اكسر;
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
        حالة Sys_ځواب:
            handle_ځواب();
            اكسر;

        حالة Sys_ځواب_Recv:
            handle_ځواب();
            handleRecv(true, true);
            اكسر;

#وإلا /* CONFIG_KERNEL_MCS */
        حالة SysWait:
            handleRecv(true, false);
            اكسر;

        حالة SysNBWait:
            handleRecv(false, false);
            اكسر;
        حالة Sys_ځواب_Recv: {
            cptr_t ځواب = get_ثبتوونکی(غوټه_STATE(ksCurThread), ځواب_ثبتوونکی);
            ret = handleInvocation(false, false, true, true, ځواب);
            /* reply is not preemptible, but to ease verification we check explicitly */
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ټکی();
                check_مداخله(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ټکی();
                check_مداخله(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_ثبتوونکی(غوټه_STATE(ksCurThread), ځواب_ثبتوونکی));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ټکی();
                check_مداخله(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, false);
            اكسر;
#د_که_پای
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

    مهالوېش();
    activateThread();

    أرجع EXCEPTION_NONE;
}
