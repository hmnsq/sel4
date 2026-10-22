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
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
#ضمن <smp/ipi.h>
#پایان_اگر
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
#ضمن <arch/machine/capdl.h>
#پایان_اگر
#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
#ضمن <mode/machine/debug.h>
#پایان_اگر

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#اگر_تعریف_شده CONFIG_IRQ_REPORTING
ساكن مضمن فراغ check_وقفه(bool_t was_وقفه_مدخل)
#وإلا
ساكن مضمن فراغ check_وقفه(فراغ)
#پایان_اگر
{
    irq_t irq;

    irq = get_فعال_IRQ();
    إذا (IRQT_مقصد_IRQ(irq) != IRQT_مقصد_IRQ(irqInvalid)) {
        handle_وقفه(irq);
    } وإلا {
#اگر_تعریف_شده CONFIG_IRQ_REPORTING
        إذا (was_وقفه_مدخل) {
            userError("Spurious interrupt!");
        }
#پایان_اگر
        handleSpuriousIRQ();
    }
}

#اگر_تعریف_نشده CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#تعریف_کن check_وقفه(was_وقفه_مدخل) check_وقفه()
#پایان_اگر

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_وقفه_مدخل(فراغ)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_درون_صف(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#پایان_اگر

    check_وقفه(/* was_interrupt_entry */ true);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_درون_صف(), 1)) {
#پایان_اگر
        زمانبندی();
        activateThread();
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    }
#پایان_اگر

    أرجع EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(کلمه_t w)
{
#اگر_تعریف_شده CONFIG_PRINTING
    إذا (w == SysDebugPutChar) {
        kernel_putchar(get_ثبات(گره_STATE(ksCurThread), cap_ثبات));
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugDumpScheduler) {
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#پایان_اگر
        أرجع EXCEPTION_NONE;
    }
#پایان_اگر
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
    إذا (w == SysDebugHalt) {
        tcb_t *استفادهنشده tptr = گره_STATE(ksCurThread);
        چاپ_کن("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    إذا (w == SysDebugSnapshot) {
        tcb_t *استفادهنشده tptr = گره_STATE(ksCurThread);
        چاپ_کن("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugCapIdentify) {
        کلمه_t cptr = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
        lookup_Cap_And_جایگاه_ret_t lu_ret = lookup_Cap_And_جایگاه(گره_STATE(ksCurThread), cptr);
        کلمه_t cap_type = cap_get_capType(lu_ret.cap);
        مجموعه_ثبات(گره_STATE(ksCurThread), cap_ثبات, cap_type);
        أرجع EXCEPTION_NONE;
    }

    إذا (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ثابت حرف *name;
        کلمه_t len;
        کلمه_t cptr = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
        lookup_Cap_And_جایگاه_ret_t lu_ret = lookup_Cap_And_جایگاه(گره_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        کلمه_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ثابت حرف *)(lookupIPCBuffer(true, گره_STATE(ksCurThread)) + 1);
        إذا (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_پیام_Max_درازی * حجم(کلمه_t));
        إذا (len == se_L4_پیام_Max_درازی * حجم(کلمه_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        مجموعه_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        أرجع EXCEPTION_NONE;
    }
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    إذا (w == SysDebugGetThreadAffinity) {
        کلمه_t cptr = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
        lookup_Cap_And_جایگاه_ret_t lu_ret = lookup_Cap_And_جایگاه(گره_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        کلمه_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        کلمه_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        مجموعه_ثبات(گره_STATE(ksCurThread), cap_ثبات, affinity);
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugSendIPI) {
        أرجع handle_SysDebugSendIPI();
    }
#پایان_اگر /* ENABLE_SMP_SUPPORT */
#پایان_اگر /* CONFIG_DEBUG_BUILD */

#اگر_تعریف_شده CONFIG_DANGEROUS_کد_INJECTION
    إذا (w == Sys_Debug_پیاپی) {
        ((فراغ (*)(فراغ *))get_ثبات(گره_STATE(ksCurThread), cap_ثبات))((فراغ *)get_ثبات(گره_STATE(ksCurThread),
                                                                                                  پیام_Info_ثبات));
        أرجع EXCEPTION_NONE;
    }
#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_X86_DANGEROUS_MSR
    إذا (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
        إذا (CONFIG_کلمه_اندازه == 32) {
            val = (uint64_t)getSyscallArg(0, عدم) | ((uint64_t)getSyscallArg(1, عدم) << 32);
        } وإلا {
            val = getSyscallArg(0, عدم);
        }
        x86_wrmsr(reg, val);
        أرجع EXCEPTION_NONE;
    } وإلا إذا (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
        val = x86_rdmsr(reg);
        صحيح رقم = 1;
        إذا (CONFIG_کلمه_اندازه == 32) {
            مجموعه_MR(گره_STATE(ksCurThread), عدم, 0, val & 0xffffffff);
            مجموعه_MR(گره_STATE(ksCurThread), عدم, 1, val >> 32);
            رقم++;
        } وإلا {
            مجموعه_MR(گره_STATE(ksCurThread), عدم, 0, val);
        }
        مجموعه_ثبات(گره_STATE(ksCurThread), پیام_Info_ثبات, کلمه_From_پیام_Info(se_L4_پیام_Info_new(0, 0, 0, رقم)));
        أرجع EXCEPTION_NONE;
    }
#پایان_اگر

#اگر_تعریف_شده CONFIG_ENABLE_BENCHMARKS
    بدل (w) {
    حالة SysBenchmarkFlushCaches:
        أرجع handle_SysBenchmarkFlushCaches();
    حالة SysBenchmarkResetLog:
        أرجع handle_SysBenchmarkResetLog();
    حالة SysBenchmarkFinalizeLog:
        أرجع handle_SysBenchmarkFinalizeLog();
#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
    حالة Sys_Benchmark_مجموعه_Log_میانگیر:
        أرجع handle_Sys_Benchmark_مجموعه_Log_میانگیر();
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */
#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_UTILISATION
    حالة SysBenchmarkGetThreadUtilisation:
        أرجع handle_SysBenchmarkGetThreadUtilisation();
    حالة SysBenchmarkResetThreadUtilisation:
        أرجع handle_SysBenchmarkResetThreadUtilisation();
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
    حالة Sys_Benchmark_Dump_All_نخها_Utilisation:
        أرجع handle_Sys_Benchmark_Dump_All_نخها_Utilisation();
    حالة Sys_Benchmark_Reset_All_نخها_Utilisation:
        أرجع handle_Sys_Benchmark_Reset_All_نخها_Utilisation();
#پایان_اگر /* CONFIG_DEBUG_BUILD */
#پایان_اگر /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    حالة SysBenchmarkNullSyscall:
        أرجع EXCEPTION_NONE;
    افتراضي:
        اكسر; /* syscall is not for benchmarking */
    } /* end switch(w) */
#پایان_اگر /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#اگر_تعریف_شده CONFIG_مجموعه_TLS_پایه_SELF
        إذا (w == Sys_مجموعه_TLSBase)
        {
            کلمه_t tls_پایه = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            أرجع Arch_مجموعه_TLSRegister(tls_پایه);
        }
#پایان_اگر
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(گره_STATE(ksCurThread));
    })

    زمانبندی();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleUserLevelFault(کلمه_t w_a, کلمه_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(گره_STATE(ksCurThread));
    })
    زمانبندی();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t وضعیت = handleVMFault(گره_STATE(ksCurThread), vm_faultType);
        إذا (وضعیت != EXCEPTION_NONE)
        {
            handleFault(گره_STATE(ksCurThread));
        }
    })

    زمانبندی();
    activateThread();

    أرجع EXCEPTION_NONE;
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t اولی_Phase, cptr_t cptr)
#وإلا
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#پایان_اگر
{
    se_L4_پیام_Info_t info;
    lookup_Cap_And_جایگاه_ret_t lu_ret;
    کلمه_t *میانگیر;
    exception_t وضعیت;
    کلمه_t درازی;
    tcb_t *thread;

    thread = گره_STATE(ksCurThread);

    info = پیام_Info_From_کلمه(get_ثبات(thread, پیام_Info_ثبات));
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    cptr_t cptr = get_ثبات(thread, cap_ثبات);
#پایان_اگر

    /* faulting section */
    lu_ret = lookup_Cap_And_جایگاه(thread, cptr);

    إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        إذا (isBlocking) {
            handleFault(thread);
        }

        أرجع EXCEPTION_NONE;
    }

    میانگیر = lookupIPCBuffer(false, thread);

    وضعیت = lookupExtraCaps(thread, میانگیر, info);

    إذا (unlikely(وضعیت != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        إذا (isBlocking) {
            handleFault(thread);
        }
        أرجع EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    درازی = se_L4_پیام_Info_get_درازی(info);
    إذا (unlikely(درازی > n_پیام_Registers && !میانگیر)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_پیام_Registers);
        درازی = n_پیام_Registers;
    }
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    وضعیت = decodeInvocation(se_L4_پیام_Info_get_label(info), درازی,
                              cptr, lu_ret.جایگاه, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, اولی_Phase, میانگیر);
#وإلا
    وضعیت = decodeInvocation(se_L4_پیام_Info_get_label(info), درازی,
                              cptr, lu_ret.جایگاه, lu_ret.cap,
                              isBlocking, isCall, میانگیر);
#پایان_اگر

    إذا (unlikely(وضعیت == EXCEPTION_PREEMPTED)) {
        أرجع وضعیت;
    }

    إذا (unlikely(وضعیت == EXCEPTION_SYSCALL_ERROR)) {
        إذا (isCall) {
            پاسخ_From_Kernel_error(thread);
        }
        أرجع EXCEPTION_NONE;
    }

    إذا (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        إذا (isCall) {
            پاسخ_From_Kernel_success_خالی(thread);
        }
        مجموعه_Thread_State(thread, ThreadState_Running);
    }

    أرجع EXCEPTION_NONE;
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
ساكن مضمن lookupCap_ret_t lookup_پاسخ(فراغ)
{
    کلمه_t پاسخ_CPtr = get_ثبات(گره_STATE(ksCurThread), پاسخ_ثبات);
    lookupCap_ret_t lu_ret = lookupCap(گره_STATE(ksCurThread), پاسخ_CPtr);
    إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(پاسخ_CPtr, true);
        أرجع lu_ret;
    }

    إذا (unlikely(cap_get_capType(lu_ret.cap) != cap_پاسخ_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(پاسخ_CPtr, true);
        lu_ret.وضعیت = EXCEPTION_FAULT;
        أرجع lu_ret;
    }

    أرجع lu_ret;
}
#وإلا
ساكن فراغ handle_پاسخ(فراغ)
{
    cte_t *caller_جایگاه;
    cap_t callerCap;

    caller_جایگاه = TCB_PTR_CTE_PTR(گره_STATE(ksCurThread), tcbCaller);
    callerCap = caller_جایگاه->cap;

    بدل (cap_get_capType(callerCap)) {
    حالة cap_پاسخ_cap: {
        tcb_t *caller;

        إذا (cap_پاسخ_cap_get_cap_پاسخ_Master(callerCap)) {
            اكسر;
        }
        caller = TCB_PTR(cap_پاسخ_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        أكد(caller != گره_STATE(ksCurThread));
        do_پاسخ_Transfer(گره_STATE(ksCurThread), caller, caller_جایگاه,
                        cap_پاسخ_cap_get_cap_پاسخ_Can_Grant(callerCap));
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
#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_MCS
ساكن فراغ handleRecv(bool_t isBlocking, bool_t can_پاسخ)
#وإلا
ساكن فراغ handleRecv(bool_t isBlocking)
#پایان_اگر
{
    کلمه_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_ثبات(گره_STATE(ksCurThread), cap_ثبات);

    lu_ret = lookupCap(گره_STATE(ksCurThread), epCPtr);

    إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(گره_STATE(ksCurThread));
        أرجع;
    }

    بدل (cap_get_capType(lu_ret.cap)) {
    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(گره_STATE(ksCurThread));
            اكسر;
        }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t پاسخ_cap = cap_null_cap_new();
        إذا (can_پاسخ) {
            lu_ret = lookup_پاسخ();
            إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(گره_STATE(ksCurThread));
                أرجع;
            } وإلا {
                پاسخ_cap = lu_ret.cap;
            }
        }
        receiveIPC(گره_STATE(ksCurThread), ep_cap, isBlocking, پاسخ_cap);
#وإلا
        deleteCallerCap(گره_STATE(ksCurThread));
        receiveIPC(گره_STATE(ksCurThread), lu_ret.cap, isBlocking);
#پایان_اگر
        اكسر;

    حالة cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != گره_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(گره_STATE(ksCurThread));
            اكسر;
        }

        receiveSignal(گره_STATE(ksCurThread), lu_ret.cap, isBlocking);
        اكسر;
    }
    افتراضي:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(گره_STATE(ksCurThread));
        اكسر;
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
ساكن مضمن فراغ mcs_Preemption_نقطه(فراغ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    إذا (isSchedulable(گره_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } وإلا إذا (sc_فعال(گره_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(گره_STATE(ksConsumed), false);
    } وإلا {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        گره_STATE(ksConsumed) = 0;
    }
}
#وإلا
#تعریف_کن handleRecv(isBlocking, can_پاسخ) handleRecv(isBlocking)
#تعریف_کن mcs_Preemption_نقطه()
#تعریف_کن handleInvocation(isCall, isBlocking, canDonate, اولی_Phase, cptr) handleInvocation(isCall, isBlocking)
#پایان_اگر

ساكن فراغ handleYield(فراغ)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = گره_STATE(ksCurSC)->scConsumed + گره_STATE(ksConsumed);
    refill_t سر = *refill_سر(گره_STATE(ksCurSC));
    chargeBudget(سر.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    گره_STATE(ksCurSC)->scConsumed = consumed;
#وإلا
    tcb_Sched_خارج_از_صف(گره_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#پایان_اگر
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        بدل (syscall)
        {
        حالة SysSend:
            ret = handleInvocation(false, true, false, false, get_ثبات(گره_STATE(ksCurThread), cap_ثبات));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطه();
                check_وقفه(/* was_interrupt_entry */ false);
            }

            اكسر;

        حالة SysNBSend:
            ret = handleInvocation(false, false, false, false, get_ثبات(گره_STATE(ksCurThread), cap_ثبات));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطه();
                check_وقفه(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysCall:
            ret = handleInvocation(true, true, true, false, get_ثبات(گره_STATE(ksCurThread), cap_ثبات));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطه();
                check_وقفه(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysRecv:
            handleRecv(true, true);
            اكسر;
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
        حالة Sys_پاسخ:
            handle_پاسخ();
            اكسر;

        حالة Sys_پاسخ_Recv:
            handle_پاسخ();
            handleRecv(true, true);
            اكسر;

#وإلا /* CONFIG_KERNEL_MCS */
        حالة SysWait:
            handleRecv(true, false);
            اكسر;

        حالة SysNBWait:
            handleRecv(false, false);
            اكسر;
        حالة Sys_پاسخ_Recv: {
            cptr_t پاسخ = get_ثبات(گره_STATE(ksCurThread), پاسخ_ثبات);
            ret = handleInvocation(false, false, true, true, پاسخ);
            /* reply is not preemptible, but to ease verification we check explicitly */
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطه();
                check_وقفه(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطه();
                check_وقفه(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_ثبات(گره_STATE(ksCurThread), پاسخ_ثبات));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطه();
                check_وقفه(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, false);
            اكسر;
#پایان_اگر
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

    زمانبندی();
    activateThread();

    أرجع EXCEPTION_NONE;
}
