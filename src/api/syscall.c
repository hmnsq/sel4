/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <benchmark/benchmark.h>
#అన్తర్భావ <arch/benchmark.h>
#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <benchmark/benchmark_utilisation.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/faults.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/faulthandler.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <object/interrupt.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <kernel/traps.h>
#అన్తర్భావ <arch/machine.h>
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
#అన్తర్భావ <smp/ipi.h>
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
#అన్తర్భావ <arch/machine/capdl.h>
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
#అన్తర్భావ <mode/machine/debug.h>
#అయితే_ముగింపు

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#నిర్వచితమైతే CONFIG_IRQ_REPORTING
స్థిర అంతఃవరుస శూన్య check_అంతరాయం(bool_t was_అంతరాయం_నమోదు)
#లేకపోతే
స్థిర అంతఃవరుస శూన్య check_అంతరాయం(శూన్య)
#అయితే_ముగింపు
{
    irq_t irq;

    irq = get_చురుకైన_IRQ();
    ఒకవేళ (IRQT_గమ్యం_IRQ(irq) != IRQT_గమ్యం_IRQ(irqInvalid)) {
        handle_అంతరాయం(irq);
    } లేకపోతే {
#నిర్వచితమైతే CONFIG_IRQ_REPORTING
        ఒకవేళ (was_అంతరాయం_నమోదు) {
            userError("Spurious interrupt!");
        }
#అయితే_ముగింపు
        handleSpuriousIRQ();
    }
}

#నిర్వచితం_కాకపోతే CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#నిర్వచించు check_అంతరాయం(was_అంతరాయం_నమోదు) check_అంతరాయం()
#అయితే_ముగింపు

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_అంతరాయం_నమోదు(శూన్య)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (SMP_TERNARY(clh_is_self_లోపల_వరుస(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#అయితే_ముగింపు

    check_అంతరాయం(/* was_interrupt_entry */ true);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (SMP_TERNARY(clh_is_self_లోపల_వరుస(), 1)) {
#అయితే_ముగింపు
        కాలపట్టిక();
        activateThread();
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    }
#అయితే_ముగింపు

    తిరిగి EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(మాట_t w)
{
#నిర్వచితమైతే CONFIG_PRINTING
    ఒకవేళ (w == SysDebugPutChar) {
        kernel_putchar(get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు));
        తిరిగి EXCEPTION_NONE;
    }
    ఒకవేళ (w == SysDebugDumpScheduler) {
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#అయితే_ముగింపు
        తిరిగి EXCEPTION_NONE;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    ఒకవేళ (w == SysDebugHalt) {
        tcb_t *వాడని tptr = కణుపు_STATE(ksCurThread);
        రాయి("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    ఒకవేళ (w == SysDebugSnapshot) {
        tcb_t *వాడని tptr = కణుపు_STATE(ksCurThread);
        రాయి("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        తిరిగి EXCEPTION_NONE;
    }
    ఒకవేళ (w == SysDebugCapIdentify) {
        మాట_t cptr = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
        lookup_Cap_And_స్థానం_ret_t lu_ret = lookup_Cap_And_స్థానం(కణుపు_STATE(ksCurThread), cptr);
        మాట_t cap_type = cap_get_capType(lu_ret.cap);
        సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు, cap_type);
        తిరిగి EXCEPTION_NONE;
    }

    ఒకవేళ (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        స్థిరరాశి అక్షరం *name;
        మాట_t len;
        మాట_t cptr = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
        lookup_Cap_And_స్థానం_ret_t lu_ret = lookup_Cap_And_స్థానం(కణుపు_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        మాట_t cap_type = cap_get_capType(lu_ret.cap);
        ఒకవేళ (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (స్థిరరాశి అక్షరం *)(lookupIPCBuffer(true, కణుపు_STATE(ksCurThread)) + 1);
        ఒకవేళ (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_సందేశం_Max_నిడివి * పరిమాణం(మాట_t));
        ఒకవేళ (len == se_L4_సందేశం_Max_నిడివి * పరిమాణం(మాట_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        సమితి_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        తిరిగి EXCEPTION_NONE;
    }
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    ఒకవేళ (w == SysDebugGetThreadAffinity) {
        మాట_t cptr = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
        lookup_Cap_And_స్థానం_ret_t lu_ret = lookup_Cap_And_స్థానం(కణుపు_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        మాట_t cap_type = cap_get_capType(lu_ret.cap);
        ఒకవేళ (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        మాట_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు, affinity);
        తిరిగి EXCEPTION_NONE;
    }
    ఒకవేళ (w == SysDebugSendIPI) {
        తిరిగి handle_SysDebugSendIPI();
    }
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */

#నిర్వచితమైతే CONFIG_DANGEROUS_సంకేతం_INJECTION
    ఒకవేళ (w == Sys_Debug_నిరంతరం) {
        ((శూన్య (*)(శూన్య *))get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు))((శూన్య *)get_రిజిస్టరు(కణుపు_STATE(ksCurThread),
                                                                                                  సందేశం_Info_రిజిస్టరు));
        తిరిగి EXCEPTION_NONE;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_X86_DANGEROUS_MSR
    ఒకవేళ (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
        ఒకవేళ (CONFIG_మాట_కొలత == 32) {
            val = (uint64_t)getSyscallArg(0, ఖాళీ) | ((uint64_t)getSyscallArg(1, ఖాళీ) << 32);
        } లేకపోతే {
            val = getSyscallArg(0, ఖాళీ);
        }
        x86_wrmsr(reg, val);
        తిరిగి EXCEPTION_NONE;
    } లేకపోతే ఒకవేళ (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
        val = x86_rdmsr(reg);
        పూర్ణాంకం అంకె = 1;
        ఒకవేళ (CONFIG_మాట_కొలత == 32) {
            సమితి_MR(కణుపు_STATE(ksCurThread), ఖాళీ, 0, val & 0xffffffff);
            సమితి_MR(కణుపు_STATE(ksCurThread), ఖాళీ, 1, val >> 32);
            అంకె++;
        } లేకపోతే {
            సమితి_MR(కణుపు_STATE(ksCurThread), ఖాళీ, 0, val);
        }
        సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(se_L4_సందేశం_Info_new(0, 0, 0, అంకె)));
        తిరిగి EXCEPTION_NONE;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ENABLE_BENCHMARKS
    ఎంపిక (w) {
    స్థితి SysBenchmarkFlushCaches:
        తిరిగి handle_SysBenchmarkFlushCaches();
    స్థితి SysBenchmarkResetLog:
        తిరిగి handle_SysBenchmarkResetLog();
    స్థితి SysBenchmarkFinalizeLog:
        తిరిగి handle_SysBenchmarkFinalizeLog();
#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
    స్థితి Sys_Benchmark_సమితి_Log_బఫరు:
        తిరిగి handle_Sys_Benchmark_సమితి_Log_బఫరు();
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */
#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_UTILISATION
    స్థితి SysBenchmarkGetThreadUtilisation:
        తిరిగి handle_SysBenchmarkGetThreadUtilisation();
    స్థితి SysBenchmarkResetThreadUtilisation:
        తిరిగి handle_SysBenchmarkResetThreadUtilisation();
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    స్థితి Sys_Benchmark_Dump_All_దారాలు_Utilisation:
        తిరిగి handle_Sys_Benchmark_Dump_All_దారాలు_Utilisation();
    స్థితి Sys_Benchmark_Reset_All_దారాలు_Utilisation:
        తిరిగి handle_Sys_Benchmark_Reset_All_దారాలు_Utilisation();
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
#అయితే_ముగింపు /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    స్థితి SysBenchmarkNullSyscall:
        తిరిగి EXCEPTION_NONE;
    అప్రమేయ:
        ఆపు; /* syscall is not for benchmarking */
    } /* end switch(w) */
#అయితే_ముగింపు /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#నిర్వచితమైతే CONFIG_సమితి_TLS_పునాది_SELF
        ఒకవేళ (w == Sys_సమితి_TLSBase)
        {
            మాట_t tls_పునాది = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            తిరిగి Arch_సమితి_TLSRegister(tls_పునాది);
        }
#అయితే_ముగింపు
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(కణుపు_STATE(ksCurThread));
    })

    కాలపట్టిక();
    activateThread();

    తిరిగి EXCEPTION_NONE;
}

exception_t handleUserLevelFault(మాట_t w_a, మాట_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(కణుపు_STATE(ksCurThread));
    })
    కాలపట్టిక();
    activateThread();

    తిరిగి EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t స్థితి_2 = handleVMFault(కణుపు_STATE(ksCurThread), vm_faultType);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE)
        {
            handleFault(కణుపు_STATE(ksCurThread));
        }
    })

    కాలపట్టిక();
    activateThread();

    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t మొదటి_Phase, cptr_t cptr)
#లేకపోతే
స్థిర exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#అయితే_ముగింపు
{
    se_L4_సందేశం_Info_t info;
    lookup_Cap_And_స్థానం_ret_t lu_ret;
    మాట_t *బఫరు;
    exception_t స్థితి_2;
    మాట_t నిడివి;
    tcb_t *thread;

    thread = కణుపు_STATE(ksCurThread);

    info = సందేశం_Info_From_మాట(get_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు));
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    cptr_t cptr = get_రిజిస్టరు(thread, cap_రిజిస్టరు);
#అయితే_ముగింపు

    /* faulting section */
    lu_ret = lookup_Cap_And_స్థానం(thread, cptr);

    ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        ఒకవేళ (isBlocking) {
            handleFault(thread);
        }

        తిరిగి EXCEPTION_NONE;
    }

    బఫరు = lookupIPCBuffer(false, thread);

    స్థితి_2 = lookupExtraCaps(thread, బఫరు, info);

    ఒకవేళ (unlikely(స్థితి_2 != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        ఒకవేళ (isBlocking) {
            handleFault(thread);
        }
        తిరిగి EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    నిడివి = se_L4_సందేశం_Info_get_నిడివి(info);
    ఒకవేళ (unlikely(నిడివి > n_సందేశం_Registers && !బఫరు)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_సందేశం_Registers);
        నిడివి = n_సందేశం_Registers;
    }
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి_2 = decodeInvocation(se_L4_సందేశం_Info_get_label(info), నిడివి,
                              cptr, lu_ret.స్థానం, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, మొదటి_Phase, బఫరు);
#లేకపోతే
    స్థితి_2 = decodeInvocation(se_L4_సందేశం_Info_get_label(info), నిడివి,
                              cptr, lu_ret.స్థానం, lu_ret.cap,
                              isBlocking, isCall, బఫరు);
#అయితే_ముగింపు

    ఒకవేళ (unlikely(స్థితి_2 == EXCEPTION_PREEMPTED)) {
        తిరిగి స్థితి_2;
    }

    ఒకవేళ (unlikely(స్థితి_2 == EXCEPTION_SYSCALL_ERROR)) {
        ఒకవేళ (isCall) {
            జవాబు_From_Kernel_error(thread);
        }
        తిరిగి EXCEPTION_NONE;
    }

    ఒకవేళ (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        ఒకవేళ (isCall) {
            జవాబు_From_Kernel_success_ఖాళీలు(thread);
        }
        సమితి_Thread_State(thread, ThreadState_Running);
    }

    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర అంతఃవరుస lookupCap_ret_t lookup_జవాబు(శూన్య)
{
    మాట_t జవాబు_CPtr = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), జవాబు_రిజిస్టరు);
    lookupCap_ret_t lu_ret = lookupCap(కణుపు_STATE(ksCurThread), జవాబు_CPtr);
    ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(జవాబు_CPtr, true);
        తిరిగి lu_ret;
    }

    ఒకవేళ (unlikely(cap_get_capType(lu_ret.cap) != cap_జవాబు_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(జవాబు_CPtr, true);
        lu_ret.స్థితి_2 = EXCEPTION_FAULT;
        తిరిగి lu_ret;
    }

    తిరిగి lu_ret;
}
#లేకపోతే
స్థిర శూన్య handle_జవాబు(శూన్య)
{
    cte_t *caller_స్థానం;
    cap_t callerCap;

    caller_స్థానం = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbCaller);
    callerCap = caller_స్థానం->cap;

    ఎంపిక (cap_get_capType(callerCap)) {
    స్థితి cap_జవాబు_cap: {
        tcb_t *caller;

        ఒకవేళ (cap_జవాబు_cap_get_cap_జవాబు_Master(callerCap)) {
            ఆపు;
        }
        caller = TCB_PTR(cap_జవాబు_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        ప్రతిజ్ఞా(caller != కణుపు_STATE(ksCurThread));
        do_జవాబు_Transfer(కణుపు_STATE(ksCurThread), caller, caller_స్థానం,
                        cap_జవాబు_cap_get_cap_జవాబు_Can_Grant(callerCap));
        తిరిగి;
    }

    స్థితి cap_null_cap:
        /* Do nothing when no caller is pending */
        తిరిగి;

    అప్రమేయ:
        ఆపు;
    }

    fail("handleReply: invalid caller cap");
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర శూన్య handleRecv(bool_t isBlocking, bool_t can_జవాబు)
#లేకపోతే
స్థిర శూన్య handleRecv(bool_t isBlocking)
#అయితే_ముగింపు
{
    మాట_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);

    lu_ret = lookupCap(కణుపు_STATE(ksCurThread), epCPtr);

    ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(కణుపు_STATE(ksCurThread));
        తిరిగి;
    }

    ఎంపిక (cap_get_capType(lu_ret.cap)) {
    స్థితి cap_endpoint_cap:
        ఒకవేళ (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(కణుపు_STATE(ksCurThread));
            ఆపు;
        }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t జవాబు_cap = cap_null_cap_new();
        ఒకవేళ (can_జవాబు) {
            lu_ret = lookup_జవాబు();
            ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(కణుపు_STATE(ksCurThread));
                తిరిగి;
            } లేకపోతే {
                జవాబు_cap = lu_ret.cap;
            }
        }
        receiveIPC(కణుపు_STATE(ksCurThread), ep_cap, isBlocking, జవాబు_cap);
#లేకపోతే
        deleteCallerCap(కణుపు_STATE(ksCurThread));
        receiveIPC(కణుపు_STATE(ksCurThread), lu_ret.cap, isBlocking);
#అయితే_ముగింపు
        ఆపు;

    స్థితి cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        ఒకవేళ (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != కణుపు_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(కణుపు_STATE(ksCurThread));
            ఆపు;
        }

        receiveSignal(కణుపు_STATE(ksCurThread), lu_ret.cap, isBlocking);
        ఆపు;
    }
    అప్రమేయ:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(కణుపు_STATE(ksCurThread));
        ఆపు;
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర అంతఃవరుస శూన్య mcs_Preemption_బిందువు(శూన్య)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    ఒకవేళ (isSchedulable(కణుపు_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } లేకపోతే ఒకవేళ (sc_చురుకైన(కణుపు_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(కణుపు_STATE(ksConsumed), false);
    } లేకపోతే {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        కణుపు_STATE(ksConsumed) = 0;
    }
}
#లేకపోతే
#నిర్వచించు handleRecv(isBlocking, can_జవాబు) handleRecv(isBlocking)
#నిర్వచించు mcs_Preemption_బిందువు()
#నిర్వచించు handleInvocation(isCall, isBlocking, canDonate, మొదటి_Phase, cptr) handleInvocation(isCall, isBlocking)
#అయితే_ముగింపు

స్థిర శూన్య handleYield(శూన్య)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = కణుపు_STATE(ksCurSC)->scConsumed + కణుపు_STATE(ksConsumed);
    refill_t తల = *refill_తల(కణుపు_STATE(ksCurSC));
    chargeBudget(తల.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    కణుపు_STATE(ksCurSC)->scConsumed = consumed;
#లేకపోతే
    tcb_Sched_వరుస_నుండి_తీయి(కణుపు_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#అయితే_ముగింపు
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        ఎంపిక (syscall)
        {
        స్థితి SysSend:
            ret = handleInvocation(false, true, false, false, get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు));
            ఒకవేళ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_బిందువు();
                check_అంతరాయం(/* was_interrupt_entry */ false);
            }

            ఆపు;

        స్థితి SysNBSend:
            ret = handleInvocation(false, false, false, false, get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు));
            ఒకవేళ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_బిందువు();
                check_అంతరాయం(/* was_interrupt_entry */ false);
            }
            ఆపు;

        స్థితి SysCall:
            ret = handleInvocation(true, true, true, false, get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు));
            ఒకవేళ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_బిందువు();
                check_అంతరాయం(/* was_interrupt_entry */ false);
            }
            ఆపు;

        స్థితి SysRecv:
            handleRecv(true, true);
            ఆపు;
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
        స్థితి Sys_జవాబు:
            handle_జవాబు();
            ఆపు;

        స్థితి Sys_జవాబు_Recv:
            handle_జవాబు();
            handleRecv(true, true);
            ఆపు;

#లేకపోతే /* CONFIG_KERNEL_MCS */
        స్థితి SysWait:
            handleRecv(true, false);
            ఆపు;

        స్థితి SysNBWait:
            handleRecv(false, false);
            ఆపు;
        స్థితి Sys_జవాబు_Recv: {
            cptr_t జవాబు = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), జవాబు_రిజిస్టరు);
            ret = handleInvocation(false, false, true, true, జవాబు);
            /* reply is not preemptible, but to ease verification we check explicitly */
            ఒకవేళ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_బిందువు();
                check_అంతరాయం(/* was_interrupt_entry */ false);
                ఆపు;
            }
            handleRecv(true, true);
            ఆపు;
        }

        స్థితి SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            ఒకవేళ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_బిందువు();
                check_అంతరాయం(/* was_interrupt_entry */ false);
                ఆపు;
            }
            handleRecv(true, true);
            ఆపు;
        }

        స్థితి SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_రిజిస్టరు(కణుపు_STATE(ksCurThread), జవాబు_రిజిస్టరు));
            ఒకవేళ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_బిందువు();
                check_అంతరాయం(/* was_interrupt_entry */ false);
                ఆపు;
            }
            handleRecv(true, false);
            ఆపు;
#అయితే_ముగింపు
        స్థితి SysNBRecv:
            handleRecv(false, true);
            ఆపు;

        స్థితి SysYield:
            handleYield();
            ఆపు;

        అప్రమేయ:
            fail("Invalid syscall");
        }

    })

    కాలపట్టిక();
    activateThread();

    తిరిగి EXCEPTION_NONE;
}
