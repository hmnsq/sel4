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
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
#ضمن <smp/ipi.h>
#جے_انت
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
#ضمن <arch/machine/capdl.h>
#جے_انت
#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
#ضمن <mode/machine/debug.h>
#جے_انت

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#جے_تعریف_شدہ CONFIG_IRQ_REPORTING
ساكن مضمن فراغ check_رکاوٹ(bool_t was_رکاوٹ_اندراج)
#وإلا
ساكن مضمن فراغ check_رکاوٹ(فراغ)
#جے_انت
{
    irq_t irq;

    irq = get_سرگرم_IRQ();
    إذا (IRQT_منزل_IRQ(irq) != IRQT_منزل_IRQ(irqInvalid)) {
        handle_رکاوٹ(irq);
    } وإلا {
#جے_تعریف_شدہ CONFIG_IRQ_REPORTING
        إذا (was_رکاوٹ_اندراج) {
            userError("Spurious interrupt!");
        }
#جے_انت
        handleSpuriousIRQ();
    }
}

#جے_بنا_تعریف CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#تعریف_کرو check_رکاوٹ(was_رکاوٹ_اندراج) check_رکاوٹ()
#جے_انت

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_رکاوٹ_اندراج(فراغ)
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_وچ_قطار(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#جے_انت

    check_رکاوٹ(/* was_interrupt_entry */ true);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_وچ_قطار(), 1)) {
#جے_انت
        ویلا_سارنی();
        activateThread();
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    }
#جے_انت

    أرجع EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(لفظ_t w)
{
#جے_تعریف_شدہ CONFIG_PRINTING
    إذا (w == SysDebugPutChar) {
        kernel_putchar(get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر));
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugDumpScheduler) {
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#جے_انت
        أرجع EXCEPTION_NONE;
    }
#جے_انت
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    إذا (w == SysDebugHalt) {
        tcb_t *بن_ورتیا tptr = گرہ_STATE(ksCurThread);
        لکھو("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    إذا (w == SysDebugSnapshot) {
        tcb_t *بن_ورتیا tptr = گرہ_STATE(ksCurThread);
        لکھو("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugCapIdentify) {
        لفظ_t cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        lookup_Cap_And_تھاں_ret_t lu_ret = lookup_Cap_And_تھاں(گرہ_STATE(ksCurThread), cptr);
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, cap_type);
        أرجع EXCEPTION_NONE;
    }

    إذا (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ثابت حرف *name;
        لفظ_t len;
        لفظ_t cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        lookup_Cap_And_تھاں_ret_t lu_ret = lookup_Cap_And_تھاں(گرہ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ثابت حرف *)(lookupIPCBuffer(true, گرہ_STATE(ksCurThread)) + 1);
        إذا (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_پیغام_Max_لمائی * حجم(لفظ_t));
        إذا (len == se_L4_پیغام_Max_لمائی * حجم(لفظ_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        سیٹ_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        أرجع EXCEPTION_NONE;
    }
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    إذا (w == SysDebugGetThreadAffinity) {
        لفظ_t cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        lookup_Cap_And_تھاں_ret_t lu_ret = lookup_Cap_And_تھاں(گرہ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        لفظ_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, affinity);
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugSendIPI) {
        أرجع handle_SysDebugSendIPI();
    }
#جے_انت /* ENABLE_SMP_SUPPORT */
#جے_انت /* CONFIG_DEBUG_BUILD */

#جے_تعریف_شدہ CONFIG_DANGEROUS_کوڈ_INJECTION
    إذا (w == Sys_Debug_لگاتار) {
        ((فراغ (*)(فراغ *))get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر))((فراغ *)get_رجسٹر(گرہ_STATE(ksCurThread),
                                                                                                  پیغام_Info_رجسٹر));
        أرجع EXCEPTION_NONE;
    }
#جے_انت

#جے_تعریف_شدہ CONFIG_KERNEL_X86_DANGEROUS_MSR
    إذا (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        إذا (CONFIG_لفظ_ناپ == 32) {
            val = (uint64_t)getSyscallArg(0, عدم) | ((uint64_t)getSyscallArg(1, عدم) << 32);
        } وإلا {
            val = getSyscallArg(0, عدم);
        }
        x86_wrmsr(reg, val);
        أرجع EXCEPTION_NONE;
    } وإلا إذا (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        val = x86_rdmsr(reg);
        صحيح عدد = 1;
        إذا (CONFIG_لفظ_ناپ == 32) {
            سیٹ_MR(گرہ_STATE(ksCurThread), عدم, 0, val & 0xffffffff);
            سیٹ_MR(گرہ_STATE(ksCurThread), عدم, 1, val >> 32);
            عدد++;
        } وإلا {
            سیٹ_MR(گرہ_STATE(ksCurThread), عدم, 0, val);
        }
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(se_L4_سنیہا_Info_new(0, 0, 0, عدد)));
        أرجع EXCEPTION_NONE;
    }
#جے_انت

#جے_تعریف_شدہ CONFIG_ENABLE_BENCHMARKS
    بدل (w) {
    حالة SysBenchmarkFlushCaches:
        أرجع handle_SysBenchmarkFlushCaches();
    حالة SysBenchmarkResetLog:
        أرجع handle_SysBenchmarkResetLog();
    حالة SysBenchmarkFinalizeLog:
        أرجع handle_SysBenchmarkFinalizeLog();
#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    حالة Sys_Benchmark_سیٹ_Log_بفر:
        أرجع handle_Sys_Benchmark_سیٹ_Log_بفر();
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */
#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    حالة SysBenchmarkGetThreadUtilisation:
        أرجع handle_SysBenchmarkGetThreadUtilisation();
    حالة SysBenchmarkResetThreadUtilisation:
        أرجع handle_SysBenchmarkResetThreadUtilisation();
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    حالة Sys_Benchmark_Dump_All_دھاگے_Utilisation:
        أرجع handle_Sys_Benchmark_Dump_All_دھاگے_Utilisation();
    حالة Sys_Benchmark_Reset_All_دھاگے_Utilisation:
        أرجع handle_Sys_Benchmark_Reset_All_دھاگے_Utilisation();
#جے_انت /* CONFIG_DEBUG_BUILD */
#جے_انت /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    حالة SysBenchmarkNullSyscall:
        أرجع EXCEPTION_NONE;
    افتراضي:
        اكسر; /* syscall is not for benchmarking */
    } /* end switch(w) */
#جے_انت /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#جے_تعریف_شدہ CONFIG_سیٹ_TLS_ادھار_SELF
        إذا (w == Sys_سیٹ_TLSBase)
        {
            لفظ_t tls_ادھار = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            أرجع Arch_سیٹ_TLSRegister(tls_ادھار);
        }
#جے_انت
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(گرہ_STATE(ksCurThread));
    })

    ویلا_سارنی();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleUserLevelFault(لفظ_t w_a, لفظ_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(گرہ_STATE(ksCurThread));
    })
    ویلا_سارنی();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t حالت = handleVMFault(گرہ_STATE(ksCurThread), vm_faultType);
        إذا (حالت != EXCEPTION_NONE)
        {
            handleFault(گرہ_STATE(ksCurThread));
        }
    })

    ویلا_سارنی();
    activateThread();

    أرجع EXCEPTION_NONE;
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t پہلا_Phase, cptr_t cptr)
#وإلا
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#جے_انت
{
    se_L4_سنیہا_Info_t info;
    lookup_Cap_And_تھاں_ret_t lu_ret;
    لفظ_t *بفر;
    exception_t حالت;
    لفظ_t لمائی;
    tcb_t *thread;

    thread = گرہ_STATE(ksCurThread);

    info = سنیہا_Info_From_لفظ(get_رجسٹر(thread, پیغام_Info_رجسٹر));
#جے_بنا_تعریف CONFIG_KERNEL_MCS
    cptr_t cptr = get_رجسٹر(thread, cap_رجسٹر);
#جے_انت

    /* faulting section */
    lu_ret = lookup_Cap_And_تھاں(thread, cptr);

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
    لمائی = se_L4_سنیہا_Info_get_لمائی(info);
    إذا (unlikely(لمائی > n_پیغام_Registers && !بفر)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_پیغام_Registers);
        لمائی = n_پیغام_Registers;
    }
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت = decodeInvocation(se_L4_سنیہا_Info_get_label(info), لمائی,
                              cptr, lu_ret.تھاں, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, پہلا_Phase, بفر);
#وإلا
    حالت = decodeInvocation(se_L4_سنیہا_Info_get_label(info), لمائی,
                              cptr, lu_ret.تھاں, lu_ret.cap,
                              isBlocking, isCall, بفر);
#جے_انت

    إذا (unlikely(حالت == EXCEPTION_PREEMPTED)) {
        أرجع حالت;
    }

    إذا (unlikely(حالت == EXCEPTION_SYSCALL_ERROR)) {
        إذا (isCall) {
            جواب_From_Kernel_error(thread);
        }
        أرجع EXCEPTION_NONE;
    }

    إذا (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        إذا (isCall) {
            جواب_From_Kernel_success_خالی_تھاواں(thread);
        }
        سیٹ_Thread_State(thread, ThreadState_Running);
    }

    أرجع EXCEPTION_NONE;
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
ساكن مضمن lookupCap_ret_t lookup_جواب(فراغ)
{
    لفظ_t جواب_CPtr = get_رجسٹر(گرہ_STATE(ksCurThread), جواب_رجسٹر);
    lookupCap_ret_t lu_ret = lookupCap(گرہ_STATE(ksCurThread), جواب_CPtr);
    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(جواب_CPtr, true);
        أرجع lu_ret;
    }

    إذا (unlikely(cap_get_capType(lu_ret.cap) != cap_جواب_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(جواب_CPtr, true);
        lu_ret.حالت = EXCEPTION_FAULT;
        أرجع lu_ret;
    }

    أرجع lu_ret;
}
#وإلا
ساكن فراغ handle_جواب(فراغ)
{
    cte_t *caller_تھاں;
    cap_t callerCap;

    caller_تھاں = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCaller);
    callerCap = caller_تھاں->cap;

    بدل (cap_get_capType(callerCap)) {
    حالة cap_جواب_cap: {
        tcb_t *caller;

        إذا (cap_جواب_cap_get_cap_جواب_Master(callerCap)) {
            اكسر;
        }
        caller = TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        أكد(caller != گرہ_STATE(ksCurThread));
        do_جواب_Transfer(گرہ_STATE(ksCurThread), caller, caller_تھاں,
                        cap_جواب_cap_get_cap_جواب_Can_Grant(callerCap));
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
#جے_انت

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
ساكن فراغ handleRecv(bool_t isBlocking, bool_t can_جواب)
#وإلا
ساكن فراغ handleRecv(bool_t isBlocking)
#جے_انت
{
    لفظ_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);

    lu_ret = lookupCap(گرہ_STATE(ksCurThread), epCPtr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(گرہ_STATE(ksCurThread));
        أرجع;
    }

    بدل (cap_get_capType(lu_ret.cap)) {
    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(گرہ_STATE(ksCurThread));
            اكسر;
        }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t جواب_cap = cap_null_cap_new();
        إذا (can_جواب) {
            lu_ret = lookup_جواب();
            إذا (lu_ret.حالت != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(گرہ_STATE(ksCurThread));
                أرجع;
            } وإلا {
                جواب_cap = lu_ret.cap;
            }
        }
        receiveIPC(گرہ_STATE(ksCurThread), ep_cap, isBlocking, جواب_cap);
#وإلا
        deleteCallerCap(گرہ_STATE(ksCurThread));
        receiveIPC(گرہ_STATE(ksCurThread), lu_ret.cap, isBlocking);
#جے_انت
        اكسر;

    حالة cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != گرہ_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(گرہ_STATE(ksCurThread));
            اكسر;
        }

        receiveSignal(گرہ_STATE(ksCurThread), lu_ret.cap, isBlocking);
        اكسر;
    }
    افتراضي:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(گرہ_STATE(ksCurThread));
        اكسر;
    }
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
ساكن مضمن فراغ mcs_Preemption_نقطہ(فراغ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    إذا (isSchedulable(گرہ_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } وإلا إذا (sc_سرگرم(گرہ_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(گرہ_STATE(ksConsumed), false);
    } وإلا {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        گرہ_STATE(ksConsumed) = 0;
    }
}
#وإلا
#تعریف_کرو handleRecv(isBlocking, can_جواب) handleRecv(isBlocking)
#تعریف_کرو mcs_Preemption_نقطہ()
#تعریف_کرو handleInvocation(isCall, isBlocking, canDonate, پہلا_Phase, cptr) handleInvocation(isCall, isBlocking)
#جے_انت

ساكن فراغ handleYield(فراغ)
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = گرہ_STATE(ksCurSC)->scConsumed + گرہ_STATE(ksConsumed);
    refill_t سر = *refill_سر(گرہ_STATE(ksCurSC));
    chargeBudget(سر.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    گرہ_STATE(ksCurSC)->scConsumed = consumed;
#وإلا
    tcb_Sched_قطار_توں_کڈھو(گرہ_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#جے_انت
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        بدل (syscall)
        {
        حالة SysSend:
            ret = handleInvocation(false, true, false, false, get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_رکاوٹ(/* was_interrupt_entry */ false);
            }

            اكسر;

        حالة SysNBSend:
            ret = handleInvocation(false, false, false, false, get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_رکاوٹ(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysCall:
            ret = handleInvocation(true, true, true, false, get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_رکاوٹ(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysRecv:
            handleRecv(true, true);
            اكسر;
#جے_بنا_تعریف CONFIG_KERNEL_MCS
        حالة Sys_جواب:
            handle_جواب();
            اكسر;

        حالة Sys_جواب_Recv:
            handle_جواب();
            handleRecv(true, true);
            اكسر;

#وإلا /* CONFIG_KERNEL_MCS */
        حالة SysWait:
            handleRecv(true, false);
            اكسر;

        حالة SysNBWait:
            handleRecv(false, false);
            اكسر;
        حالة Sys_جواب_Recv: {
            cptr_t جواب = get_رجسٹر(گرہ_STATE(ksCurThread), جواب_رجسٹر);
            ret = handleInvocation(false, false, true, true, جواب);
            /* reply is not preemptible, but to ease verification we check explicitly */
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_رکاوٹ(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_رکاوٹ(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_رجسٹر(گرہ_STATE(ksCurThread), جواب_رجسٹر));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_رکاوٹ(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, false);
            اكسر;
#جے_انت
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

    ویلا_سارنی();
    activateThread();

    أرجع EXCEPTION_NONE;
}
