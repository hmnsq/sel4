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
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
#ضمن <smp/ipi.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
#ضمن <arch/machine/capdl.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
#ضمن <mode/machine/debug.h>
#جيڪڏهن_پورو

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#جيڪڏهن_وصف_ٿيل CONFIG_IRQ_REPORTING
ساكن مضمن فراغ check_مداخلت(bool_t was_مداخلت_داخلا)
#وإلا
ساكن مضمن فراغ check_مداخلت(فراغ)
#جيڪڏهن_پورو
{
    irq_t irq;

    irq = get_سرگرم_IRQ();
    إذا (IRQT_منزل_IRQ(irq) != IRQT_منزل_IRQ(irqInvalid)) {
        handle_مداخلت(irq);
    } وإلا {
#جيڪڏهن_وصف_ٿيل CONFIG_IRQ_REPORTING
        إذا (was_مداخلت_داخلا) {
            userError("Spurious interrupt!");
        }
#جيڪڏهن_پورو
        handleSpuriousIRQ();
    }
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#وصف_ڪريو check_مداخلت(was_مداخلت_داخلا) check_مداخلت()
#جيڪڏهن_پورو

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_مداخلت_داخلا(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_اندريون_قطار(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#جيڪڏهن_پورو

    check_مداخلت(/* was_interrupt_entry */ true);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (SMP_TERNARY(clh_is_self_اندريون_قطار(), 1)) {
#جيڪڏهن_پورو
        وقت_جدول();
        activateThread();
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    }
#جيڪڏهن_پورو

    أرجع EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(لفظ_t w)
{
#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
    إذا (w == SysDebugPutChar) {
        kernel_putchar(get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر));
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugDumpScheduler) {
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#جيڪڏهن_پورو
        أرجع EXCEPTION_NONE;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
    إذا (w == SysDebugHalt) {
        tcb_t *اڻ_استعمال_ٿيل tptr = ڳنڍ_STATE(ksCurThread);
        لکو("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    إذا (w == SysDebugSnapshot) {
        tcb_t *اڻ_استعمال_ٿيل tptr = ڳنڍ_STATE(ksCurThread);
        لکو("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugCapIdentify) {
        لفظ_t cptr = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
        lookup_Cap_And_جڳهه_ret_t lu_ret = lookup_Cap_And_جڳهه(ڳنڍ_STATE(ksCurThread), cptr);
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر, cap_type);
        أرجع EXCEPTION_NONE;
    }

    إذا (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ثابت حرف *name;
        لفظ_t len;
        لفظ_t cptr = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
        lookup_Cap_And_جڳهه_ret_t lu_ret = lookup_Cap_And_جڳهه(ڳنڍ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ثابت حرف *)(lookupIPCBuffer(true, ڳنڍ_STATE(ksCurThread)) + 1);
        إذا (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_نياپو_Max_ڊيگهائي * حجم(لفظ_t));
        إذا (len == se_L4_نياپو_Max_ڊيگهائي * حجم(لفظ_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        سيٽ_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        أرجع EXCEPTION_NONE;
    }
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    إذا (w == SysDebugGetThreadAffinity) {
        لفظ_t cptr = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
        lookup_Cap_And_جڳهه_ret_t lu_ret = lookup_Cap_And_جڳهه(ڳنڍ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        لفظ_t cap_type = cap_get_capType(lu_ret.cap);
        إذا (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        لفظ_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر, affinity);
        أرجع EXCEPTION_NONE;
    }
    إذا (w == SysDebugSendIPI) {
        أرجع handle_SysDebugSendIPI();
    }
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */

#جيڪڏهن_وصف_ٿيل CONFIG_DANGEROUS_ڪوڊ_INJECTION
    إذا (w == Sys_Debug_لاڳيتو) {
        ((فراغ (*)(فراغ *))get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر))((فراغ *)get_رجسٽر(ڳنڍ_STATE(ksCurThread),
                                                                                                  نياپو_Info_رجسٽر));
        أرجع EXCEPTION_NONE;
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_X86_DANGEROUS_MSR
    إذا (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
        إذا (CONFIG_لفظ_سائيز == 32) {
            val = (uint64_t)getSyscallArg(0, عدم) | ((uint64_t)getSyscallArg(1, عدم) << 32);
        } وإلا {
            val = getSyscallArg(0, عدم);
        }
        x86_wrmsr(reg, val);
        أرجع EXCEPTION_NONE;
    } وإلا إذا (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
        val = x86_rdmsr(reg);
        صحيح انگ = 1;
        إذا (CONFIG_لفظ_سائيز == 32) {
            سيٽ_MR(ڳنڍ_STATE(ksCurThread), عدم, 0, val & 0xffffffff);
            سيٽ_MR(ڳنڍ_STATE(ksCurThread), عدم, 1, val >> 32);
            انگ++;
        } وإلا {
            سيٽ_MR(ڳنڍ_STATE(ksCurThread), عدم, 0, val);
        }
        سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(se_L4_پيغام_Info_new(0, 0, 0, انگ)));
        أرجع EXCEPTION_NONE;
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ENABLE_BENCHMARKS
    بدل (w) {
    حالة SysBenchmarkFlushCaches:
        أرجع handle_SysBenchmarkFlushCaches();
    حالة SysBenchmarkResetLog:
        أرجع handle_SysBenchmarkResetLog();
    حالة SysBenchmarkFinalizeLog:
        أرجع handle_SysBenchmarkFinalizeLog();
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
    حالة Sys_Benchmark_سيٽ_Log_بفر:
        أرجع handle_Sys_Benchmark_سيٽ_Log_بفر();
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */
#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_UTILISATION
    حالة SysBenchmarkGetThreadUtilisation:
        أرجع handle_SysBenchmarkGetThreadUtilisation();
    حالة SysBenchmarkResetThreadUtilisation:
        أرجع handle_SysBenchmarkResetThreadUtilisation();
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
    حالة Sys_Benchmark_Dump_All_ڌاڳا_Utilisation:
        أرجع handle_Sys_Benchmark_Dump_All_ڌاڳا_Utilisation();
    حالة Sys_Benchmark_Reset_All_ڌاڳا_Utilisation:
        أرجع handle_Sys_Benchmark_Reset_All_ڌاڳا_Utilisation();
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
#جيڪڏهن_پورو /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    حالة SysBenchmarkNullSyscall:
        أرجع EXCEPTION_NONE;
    افتراضي:
        اكسر; /* syscall is not for benchmarking */
    } /* end switch(w) */
#جيڪڏهن_پورو /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#جيڪڏهن_وصف_ٿيل CONFIG_سيٽ_TLS_بنياد_SELF
        إذا (w == Sys_سيٽ_TLSBase)
        {
            لفظ_t tls_بنياد = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            أرجع Arch_سيٽ_TLSRegister(tls_بنياد);
        }
#جيڪڏهن_پورو
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(ڳنڍ_STATE(ksCurThread));
    })

    وقت_جدول();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleUserLevelFault(لفظ_t w_a, لفظ_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(ڳنڍ_STATE(ksCurThread));
    })
    وقت_جدول();
    activateThread();

    أرجع EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t حالت = handleVMFault(ڳنڍ_STATE(ksCurThread), vm_faultType);
        إذا (حالت != EXCEPTION_NONE)
        {
            handleFault(ڳنڍ_STATE(ksCurThread));
        }
    })

    وقت_جدول();
    activateThread();

    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t پهريون_Phase, cptr_t cptr)
#وإلا
ساكن exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#جيڪڏهن_پورو
{
    se_L4_پيغام_Info_t info;
    lookup_Cap_And_جڳهه_ret_t lu_ret;
    لفظ_t *بفر;
    exception_t حالت;
    لفظ_t ڊيگهائي;
    tcb_t *thread;

    thread = ڳنڍ_STATE(ksCurThread);

    info = پيغام_Info_From_لفظ(get_رجسٽر(thread, نياپو_Info_رجسٽر));
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    cptr_t cptr = get_رجسٽر(thread, cap_رجسٽر);
#جيڪڏهن_پورو

    /* faulting section */
    lu_ret = lookup_Cap_And_جڳهه(thread, cptr);

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
    ڊيگهائي = se_L4_پيغام_Info_get_ڊيگهائي(info);
    إذا (unlikely(ڊيگهائي > n_نياپو_Registers && !بفر)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_نياپو_Registers);
        ڊيگهائي = n_نياپو_Registers;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالت = decodeInvocation(se_L4_پيغام_Info_get_label(info), ڊيگهائي,
                              cptr, lu_ret.جڳهه, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, پهريون_Phase, بفر);
#وإلا
    حالت = decodeInvocation(se_L4_پيغام_Info_get_label(info), ڊيگهائي,
                              cptr, lu_ret.جڳهه, lu_ret.cap,
                              isBlocking, isCall, بفر);
#جيڪڏهن_پورو

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
            جواب_From_Kernel_success_خالي_جايون(thread);
        }
        سيٽ_Thread_State(thread, ThreadState_Running);
    }

    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
ساكن مضمن lookupCap_ret_t lookup_جواب(فراغ)
{
    لفظ_t جواب_CPtr = get_رجسٽر(ڳنڍ_STATE(ksCurThread), جواب_رجسٽر);
    lookupCap_ret_t lu_ret = lookupCap(ڳنڍ_STATE(ksCurThread), جواب_CPtr);
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
    cte_t *caller_جڳهه;
    cap_t callerCap;

    caller_جڳهه = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbCaller);
    callerCap = caller_جڳهه->cap;

    بدل (cap_get_capType(callerCap)) {
    حالة cap_جواب_cap: {
        tcb_t *caller;

        إذا (cap_جواب_cap_get_cap_جواب_Master(callerCap)) {
            اكسر;
        }
        caller = TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        أكد(caller != ڳنڍ_STATE(ksCurThread));
        do_جواب_Transfer(ڳنڍ_STATE(ksCurThread), caller, caller_جڳهه,
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
ساكن فراغ handleRecv(bool_t isBlocking, bool_t can_جواب)
#وإلا
ساكن فراغ handleRecv(bool_t isBlocking)
#جيڪڏهن_پورو
{
    لفظ_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);

    lu_ret = lookupCap(ڳنڍ_STATE(ksCurThread), epCPtr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ڳنڍ_STATE(ksCurThread));
        أرجع;
    }

    بدل (cap_get_capType(lu_ret.cap)) {
    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ڳنڍ_STATE(ksCurThread));
            اكسر;
        }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t جواب_cap = cap_null_cap_new();
        إذا (can_جواب) {
            lu_ret = lookup_جواب();
            إذا (lu_ret.حالت != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(ڳنڍ_STATE(ksCurThread));
                أرجع;
            } وإلا {
                جواب_cap = lu_ret.cap;
            }
        }
        receiveIPC(ڳنڍ_STATE(ksCurThread), ep_cap, isBlocking, جواب_cap);
#وإلا
        deleteCallerCap(ڳنڍ_STATE(ksCurThread));
        receiveIPC(ڳنڍ_STATE(ksCurThread), lu_ret.cap, isBlocking);
#جيڪڏهن_پورو
        اكسر;

    حالة cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != ڳنڍ_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ڳنڍ_STATE(ksCurThread));
            اكسر;
        }

        receiveSignal(ڳنڍ_STATE(ksCurThread), lu_ret.cap, isBlocking);
        اكسر;
    }
    افتراضي:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ڳنڍ_STATE(ksCurThread));
        اكسر;
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
ساكن مضمن فراغ mcs_Preemption_نقطو(فراغ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    إذا (isSchedulable(ڳنڍ_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } وإلا إذا (sc_سرگرم(ڳنڍ_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(ڳنڍ_STATE(ksConsumed), false);
    } وإلا {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        ڳنڍ_STATE(ksConsumed) = 0;
    }
}
#وإلا
#وصف_ڪريو handleRecv(isBlocking, can_جواب) handleRecv(isBlocking)
#وصف_ڪريو mcs_Preemption_نقطو()
#وصف_ڪريو handleInvocation(isCall, isBlocking, canDonate, پهريون_Phase, cptr) handleInvocation(isCall, isBlocking)
#جيڪڏهن_پورو

ساكن فراغ handleYield(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = ڳنڍ_STATE(ksCurSC)->scConsumed + ڳنڍ_STATE(ksConsumed);
    refill_t سر = *refill_سر(ڳنڍ_STATE(ksCurSC));
    chargeBudget(سر.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    ڳنڍ_STATE(ksCurSC)->scConsumed = consumed;
#وإلا
    tcb_Sched_قطار_مان_ڪڍو(ڳنڍ_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#جيڪڏهن_پورو
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        بدل (syscall)
        {
        حالة SysSend:
            ret = handleInvocation(false, true, false, false, get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطو();
                check_مداخلت(/* was_interrupt_entry */ false);
            }

            اكسر;

        حالة SysNBSend:
            ret = handleInvocation(false, false, false, false, get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطو();
                check_مداخلت(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysCall:
            ret = handleInvocation(true, true, true, false, get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطو();
                check_مداخلت(/* was_interrupt_entry */ false);
            }
            اكسر;

        حالة SysRecv:
            handleRecv(true, true);
            اكسر;
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
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
            cptr_t جواب = get_رجسٽر(ڳنڍ_STATE(ksCurThread), جواب_رجسٽر);
            ret = handleInvocation(false, false, true, true, جواب);
            /* reply is not preemptible, but to ease verification we check explicitly */
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطو();
                check_مداخلت(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطو();
                check_مداخلت(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, true);
            اكسر;
        }

        حالة SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_رجسٽر(ڳنڍ_STATE(ksCurThread), جواب_رجسٽر));
            إذا (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_نقطو();
                check_مداخلت(/* was_interrupt_entry */ false);
                اكسر;
            }
            handleRecv(true, false);
            اكسر;
#جيڪڏهن_پورو
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

    وقت_جدول();
    activateThread();

    أرجع EXCEPTION_NONE;
}
