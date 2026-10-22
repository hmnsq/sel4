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
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
#شامل <smp/ipi.h>
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
#شامل <arch/machine/capdl.h>
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
#شامل <mode/machine/debug.h>
#اگر_ختم

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#اگر_تعریف_شدہ CONFIG_IRQ_REPORTING
ساکن اندرونی خالی check_مداخلت(bool_t was_مداخلت_اندراج)
#ورنہ
ساکن اندرونی خالی check_مداخلت(خالی)
#اگر_ختم
{
    irq_t irq;

    irq = get_فعال_IRQ();
    اگر (IRQT_مقصد_IRQ(irq) != IRQT_مقصد_IRQ(irqInvalid)) {
        handle_مداخلت(irq);
    } ورنہ {
#اگر_تعریف_شدہ CONFIG_IRQ_REPORTING
        اگر (was_مداخلت_اندراج) {
            userError("Spurious interrupt!");
        }
#اگر_ختم
        handleSpuriousIRQ();
    }
}

#اگر_غیر_تعریف_شدہ CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#تعریف_کرو check_مداخلت(was_مداخلت_اندراج) check_مداخلت()
#اگر_ختم

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_مداخلت_اندراج(خالی)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (SMP_TERNARY(clh_is_self_اندر_قطار(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#اگر_ختم

    check_مداخلت(/* was_interrupt_entry */ true);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (SMP_TERNARY(clh_is_self_اندر_قطار(), 1)) {
#اگر_ختم
        جدول_وقت();
        activateThread();
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    }
#اگر_ختم

    واپس EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(لفظ_t w)
{
#اگر_تعریف_شدہ CONFIG_PRINTING
    اگر (w == SysDebugPutChar) {
        kernel_putchar(get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر));
        واپس EXCEPTION_NONE;
    }
    اگر (w == SysDebugDumpScheduler) {
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#اگر_ختم
        واپس EXCEPTION_NONE;
    }
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    اگر (w == SysDebugHalt) {
        tcb_t *غیر_مستعمل tptr = گرہ_STATE(ksCurThread);
        لکھو("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    اگر (w == SysDebugSnapshot) {
        tcb_t *غیر_مستعمل tptr = گرہ_STATE(ksCurThread);
        لکھو("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        واپس EXCEPTION_NONE;
    }
    اگر (w == SysDebugCapIdentify) {
        لفظ_t cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        lookup_Cap_And_خانہ_ret_t lu_ret = lookup_Cap_And_خانہ(گرہ_STATE(ksCurThread), cptr);
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, cap_type);
        واپس EXCEPTION_NONE;
    }

    اگر (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ثابت حرف *name;
        لفظ_t len;
        لفظ_t cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        lookup_Cap_And_خانہ_ret_t lu_ret = lookup_Cap_And_خانہ(گرہ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        اگر (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ثابت حرف *)(lookupIPCBuffer(true, گرہ_STATE(ksCurThread)) + 1);
        اگر (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_پیغام_Max_لمبائی * حجم(لفظ_t));
        اگر (len == se_L4_پیغام_Max_لمبائی * حجم(لفظ_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        سیٹ_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        واپس EXCEPTION_NONE;
    }
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    اگر (w == SysDebugGetThreadAffinity) {
        لفظ_t cptr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        lookup_Cap_And_خانہ_ret_t lu_ret = lookup_Cap_And_خانہ(گرہ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        اگر (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        لفظ_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر, affinity);
        واپس EXCEPTION_NONE;
    }
    اگر (w == SysDebugSendIPI) {
        واپس handle_SysDebugSendIPI();
    }
#اگر_ختم /* ENABLE_SMP_SUPPORT */
#اگر_ختم /* CONFIG_DEBUG_BUILD */

#اگر_تعریف_شدہ CONFIG_DANGEROUS_رمز_INJECTION
    اگر (w == Sys_Debug_تسلسل) {
        ((خالی (*)(خالی *))get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر))((خالی *)get_رجسٹر(گرہ_STATE(ksCurThread),
                                                                                                  پیغام_Info_رجسٹر));
        واپس EXCEPTION_NONE;
    }
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_X86_DANGEROUS_MSR
    اگر (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        اگر (CONFIG_لفظ_سائز == 32) {
            val = (uint64_t)getSyscallArg(0, تہی) | ((uint64_t)getSyscallArg(1, تہی) << 32);
        } ورنہ {
            val = getSyscallArg(0, تہی);
        }
        x86_wrmsr(reg, val);
        واپس EXCEPTION_NONE;
    } ورنہ اگر (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
        val = x86_rdmsr(reg);
        صحیح عدد = 1;
        اگر (CONFIG_لفظ_سائز == 32) {
            سیٹ_MR(گرہ_STATE(ksCurThread), تہی, 0, val & 0xffffffff);
            سیٹ_MR(گرہ_STATE(ksCurThread), تہی, 1, val >> 32);
            عدد++;
        } ورنہ {
            سیٹ_MR(گرہ_STATE(ksCurThread), تہی, 0, val);
        }
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(se_L4_پیغام_Info_new(0, 0, 0, عدد)));
        واپس EXCEPTION_NONE;
    }
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ENABLE_BENCHMARKS
    انتخاب (w) {
    حالت SysBenchmarkFlushCaches:
        واپس handle_SysBenchmarkFlushCaches();
    حالت SysBenchmarkResetLog:
        واپس handle_SysBenchmarkResetLog();
    حالت SysBenchmarkFinalizeLog:
        واپس handle_SysBenchmarkFinalizeLog();
#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    حالت Sys_Benchmark_سیٹ_Log_بفر:
        واپس handle_Sys_Benchmark_سیٹ_Log_بفر();
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */
#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    حالت SysBenchmarkGetThreadUtilisation:
        واپس handle_SysBenchmarkGetThreadUtilisation();
    حالت SysBenchmarkResetThreadUtilisation:
        واپس handle_SysBenchmarkResetThreadUtilisation();
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    حالت Sys_Benchmark_Dump_All_دھاگے_Utilisation:
        واپس handle_Sys_Benchmark_Dump_All_دھاگے_Utilisation();
    حالت Sys_Benchmark_Reset_All_دھاگے_Utilisation:
        واپس handle_Sys_Benchmark_Reset_All_دھاگے_Utilisation();
#اگر_ختم /* CONFIG_DEBUG_BUILD */
#اگر_ختم /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    حالت SysBenchmarkNullSyscall:
        واپس EXCEPTION_NONE;
    طےشدہ:
        توڑو; /* syscall is not for benchmarking */
    } /* end switch(w) */
#اگر_ختم /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#اگر_تعریف_شدہ CONFIG_سیٹ_TLS_بنیاد_SELF
        اگر (w == Sys_سیٹ_TLSBase)
        {
            لفظ_t tls_بنیاد = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            واپس Arch_سیٹ_TLSRegister(tls_بنیاد);
        }
#اگر_ختم
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(گرہ_STATE(ksCurThread));
    })

    جدول_وقت();
    activateThread();

    واپس EXCEPTION_NONE;
}

exception_t handleUserLevelFault(لفظ_t w_a, لفظ_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(گرہ_STATE(ksCurThread));
    })
    جدول_وقت();
    activateThread();

    واپس EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t حالت_2 = handleVMFault(گرہ_STATE(ksCurThread), vm_faultType);
        اگر (حالت_2 != EXCEPTION_NONE)
        {
            handleFault(گرہ_STATE(ksCurThread));
        }
    })

    جدول_وقت();
    activateThread();

    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t پہلا_Phase, cptr_t cptr)
#ورنہ
ساکن exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#اگر_ختم
{
    se_L4_پیغام_Info_t info;
    lookup_Cap_And_خانہ_ret_t lu_ret;
    لفظ_t *بفر;
    exception_t حالت_2;
    لفظ_t لمبائی;
    tcb_t *thread;

    thread = گرہ_STATE(ksCurThread);

    info = پیغام_Info_From_لفظ(get_رجسٹر(thread, پیغام_Info_رجسٹر));
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    cptr_t cptr = get_رجسٹر(thread, cap_رجسٹر);
#اگر_ختم

    /* faulting section */
    lu_ret = lookup_Cap_And_خانہ(thread, cptr);

    اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        اگر (isBlocking) {
            handleFault(thread);
        }

        واپس EXCEPTION_NONE;
    }

    بفر = lookupIPCBuffer(false, thread);

    حالت_2 = lookupExtraCaps(thread, بفر, info);

    اگر (unlikely(حالت_2 != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        اگر (isBlocking) {
            handleFault(thread);
        }
        واپس EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    لمبائی = se_L4_پیغام_Info_get_لمبائی(info);
    اگر (unlikely(لمبائی > n_پیغام_Registers && !بفر)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_پیغام_Registers);
        لمبائی = n_پیغام_Registers;
    }
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت_2 = decodeInvocation(se_L4_پیغام_Info_get_label(info), لمبائی,
                              cptr, lu_ret.خانہ, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, پہلا_Phase, بفر);
#ورنہ
    حالت_2 = decodeInvocation(se_L4_پیغام_Info_get_label(info), لمبائی,
                              cptr, lu_ret.خانہ, lu_ret.cap,
                              isBlocking, isCall, بفر);
#اگر_ختم

    اگر (unlikely(حالت_2 == EXCEPTION_PREEMPTED)) {
        واپس حالت_2;
    }

    اگر (unlikely(حالت_2 == EXCEPTION_SYSCALL_ERROR)) {
        اگر (isCall) {
            جواب_From_Kernel_error(thread);
        }
        واپس EXCEPTION_NONE;
    }

    اگر (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        اگر (isCall) {
            جواب_From_Kernel_success_خالی_خانے(thread);
        }
        سیٹ_Thread_State(thread, ThreadState_Running);
    }

    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن اندرونی lookupCap_ret_t lookup_جواب(خالی)
{
    لفظ_t جواب_CPtr = get_رجسٹر(گرہ_STATE(ksCurThread), جواب_رجسٹر);
    lookupCap_ret_t lu_ret = lookupCap(گرہ_STATE(ksCurThread), جواب_CPtr);
    اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(جواب_CPtr, true);
        واپس lu_ret;
    }

    اگر (unlikely(cap_get_capType(lu_ret.cap) != cap_جواب_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(جواب_CPtr, true);
        lu_ret.حالت_2 = EXCEPTION_FAULT;
        واپس lu_ret;
    }

    واپس lu_ret;
}
#ورنہ
ساکن خالی handle_جواب(خالی)
{
    cte_t *caller_خانہ;
    cap_t callerCap;

    caller_خانہ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCaller);
    callerCap = caller_خانہ->cap;

    انتخاب (cap_get_capType(callerCap)) {
    حالت cap_جواب_cap: {
        tcb_t *caller;

        اگر (cap_جواب_cap_get_cap_جواب_Master(callerCap)) {
            توڑو;
        }
        caller = TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        أكد(caller != گرہ_STATE(ksCurThread));
        do_جواب_Transfer(گرہ_STATE(ksCurThread), caller, caller_خانہ,
                        cap_جواب_cap_get_cap_جواب_Can_Grant(callerCap));
        واپس;
    }

    حالت cap_null_cap:
        /* Do nothing when no caller is pending */
        واپس;

    طےشدہ:
        توڑو;
    }

    fail("handleReply: invalid caller cap");
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن خالی handleRecv(bool_t isBlocking, bool_t can_جواب)
#ورنہ
ساکن خالی handleRecv(bool_t isBlocking)
#اگر_ختم
{
    لفظ_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);

    lu_ret = lookupCap(گرہ_STATE(ksCurThread), epCPtr);

    اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(گرہ_STATE(ksCurThread));
        واپس;
    }

    انتخاب (cap_get_capType(lu_ret.cap)) {
    حالت cap_endpoint_cap:
        اگر (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(گرہ_STATE(ksCurThread));
            توڑو;
        }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t جواب_cap = cap_null_cap_new();
        اگر (can_جواب) {
            lu_ret = lookup_جواب();
            اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(گرہ_STATE(ksCurThread));
                واپس;
            } ورنہ {
                جواب_cap = lu_ret.cap;
            }
        }
        receiveIPC(گرہ_STATE(ksCurThread), ep_cap, isBlocking, جواب_cap);
#ورنہ
        deleteCallerCap(گرہ_STATE(ksCurThread));
        receiveIPC(گرہ_STATE(ksCurThread), lu_ret.cap, isBlocking);
#اگر_ختم
        توڑو;

    حالت cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        اگر (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != گرہ_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(گرہ_STATE(ksCurThread));
            توڑو;
        }

        receiveSignal(گرہ_STATE(ksCurThread), lu_ret.cap, isBlocking);
        توڑو;
    }
    طےشدہ:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(گرہ_STATE(ksCurThread));
        توڑو;
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن اندرونی خالی mcs_Preemption_نقطہ(خالی)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    اگر (isSchedulable(گرہ_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } ورنہ اگر (sc_فعال(گرہ_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(گرہ_STATE(ksConsumed), false);
    } ورنہ {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        گرہ_STATE(ksConsumed) = 0;
    }
}
#ورنہ
#تعریف_کرو handleRecv(isBlocking, can_جواب) handleRecv(isBlocking)
#تعریف_کرو mcs_Preemption_نقطہ()
#تعریف_کرو handleInvocation(isCall, isBlocking, canDonate, پہلا_Phase, cptr) handleInvocation(isCall, isBlocking)
#اگر_ختم

ساکن خالی handleYield(خالی)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = گرہ_STATE(ksCurSC)->scConsumed + گرہ_STATE(ksConsumed);
    refill_t سر = *refill_سر(گرہ_STATE(ksCurSC));
    chargeBudget(سر.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    گرہ_STATE(ksCurSC)->scConsumed = consumed;
#ورنہ
    tcb_Sched_قطار_سے_نکالو(گرہ_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#اگر_ختم
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        انتخاب (syscall)
        {
        حالت SysSend:
            ret = handleInvocation(false, true, false, false, get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر));
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_مداخلت(/* was_interrupt_entry */ false);
            }

            توڑو;

        حالت SysNBSend:
            ret = handleInvocation(false, false, false, false, get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر));
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_مداخلت(/* was_interrupt_entry */ false);
            }
            توڑو;

        حالت SysCall:
            ret = handleInvocation(true, true, true, false, get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر));
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_مداخلت(/* was_interrupt_entry */ false);
            }
            توڑو;

        حالت SysRecv:
            handleRecv(true, true);
            توڑو;
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
        حالت Sys_جواب:
            handle_جواب();
            توڑو;

        حالت Sys_جواب_Recv:
            handle_جواب();
            handleRecv(true, true);
            توڑو;

#ورنہ /* CONFIG_KERNEL_MCS */
        حالت SysWait:
            handleRecv(true, false);
            توڑو;

        حالت SysNBWait:
            handleRecv(false, false);
            توڑو;
        حالت Sys_جواب_Recv: {
            cptr_t جواب = get_رجسٹر(گرہ_STATE(ksCurThread), جواب_رجسٹر);
            ret = handleInvocation(false, false, true, true, جواب);
            /* reply is not preemptible, but to ease verification we check explicitly */
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_مداخلت(/* was_interrupt_entry */ false);
                توڑو;
            }
            handleRecv(true, true);
            توڑو;
        }

        حالت SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_مداخلت(/* was_interrupt_entry */ false);
                توڑو;
            }
            handleRecv(true, true);
            توڑو;
        }

        حالت SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_رجسٹر(گرہ_STATE(ksCurThread), جواب_رجسٹر));
            اگر (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطہ();
                check_مداخلت(/* was_interrupt_entry */ false);
                توڑو;
            }
            handleRecv(true, false);
            توڑو;
#اگر_ختم
        حالت SysNBRecv:
            handleRecv(false, true);
            توڑو;

        حالت SysYield:
            handleYield();
            توڑو;

        طےشدہ:
            fail("Invalid syscall");
        }

    })

    جدول_وقت();
    activateThread();

    واپس EXCEPTION_NONE;
}
