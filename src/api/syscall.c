/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_utilisation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/faults.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/faulthandler.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/traps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/ipi.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/capdl.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine/debug.h>
#ਜੇ_ਅੰਤ

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IRQ_REPORTING
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ check_ਰੁਕਾਵਟ(bool_t was_ਰੁਕਾਵਟ_ਇੰਦਰਾਜ)
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ check_ਰੁਕਾਵਟ(ਖਾਲੀ)
#ਜੇ_ਅੰਤ
{
    irq_t irq;

    irq = get_ਸਰਗਰਮ_IRQ();
    ਜੇ (IRQT_ਮੰਜ਼ਿਲ_IRQ(irq) != IRQT_ਮੰਜ਼ਿਲ_IRQ(irqInvalid)) {
        handle_ਰੁਕਾਵਟ(irq);
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IRQ_REPORTING
        ਜੇ (was_ਰੁਕਾਵਟ_ਇੰਦਰਾਜ) {
            userError("Spurious interrupt!");
        }
#ਜੇ_ਅੰਤ
        handleSpuriousIRQ();
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ check_ਰੁਕਾਵਟ(was_ਰੁਕਾਵਟ_ਇੰਦਰਾਜ) check_ਰੁਕਾਵਟ()
#ਜੇ_ਅੰਤ

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_ਰੁਕਾਵਟ_ਇੰਦਰਾਜ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (SMP_TERNARY(clh_is_self_ਵਿੱਚ_ਕਤਾਰ(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#ਜੇ_ਅੰਤ

    check_ਰੁਕਾਵਟ(/* was_interrupt_entry */ true);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (SMP_TERNARY(clh_is_self_ਵਿੱਚ_ਕਤਾਰ(), 1)) {
#ਜੇ_ਅੰਤ
        ਸਮਾਂ_ਸਾਰਣੀ();
        activateThread();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(ਸ਼ਬਦ_t w)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    ਜੇ (w == SysDebugPutChar) {
        kernel_putchar(get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ));
        ਵਾਪਸ EXCEPTION_NONE;
    }
    ਜੇ (w == SysDebugDumpScheduler) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#ਜੇ_ਅੰਤ
        ਵਾਪਸ EXCEPTION_NONE;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    ਜੇ (w == SysDebugHalt) {
        tcb_t *ਅਣਵਰਤਿਆ tptr = ਗੰਢ_STATE(ksCurThread);
        ਲਿਖੋ("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    ਜੇ (w == SysDebugSnapshot) {
        tcb_t *ਅਣਵਰਤਿਆ tptr = ਗੰਢ_STATE(ksCurThread);
        ਲਿਖੋ("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        ਵਾਪਸ EXCEPTION_NONE;
    }
    ਜੇ (w == SysDebugCapIdentify) {
        ਸ਼ਬਦ_t cptr = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
        lookup_Cap_And_ਥਾਂ_ret_t lu_ret = lookup_Cap_And_ਥਾਂ(ਗੰਢ_STATE(ksCurThread), cptr);
        ਸ਼ਬਦ_t cap_type = cap_get_capType(lu_ret.cap);
        ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ, cap_type);
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਜੇ (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ਅਚਲ ਅੱਖਰ *name;
        ਸ਼ਬਦ_t len;
        ਸ਼ਬਦ_t cptr = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
        lookup_Cap_And_ਥਾਂ_ret_t lu_ret = lookup_Cap_And_ਥਾਂ(ਗੰਢ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        ਸ਼ਬਦ_t cap_type = cap_get_capType(lu_ret.cap);
        ਜੇ (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ਅਚਲ ਅੱਖਰ *)(lookupIPCBuffer(true, ਗੰਢ_STATE(ksCurThread)) + 1);
        ਜੇ (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_ਸੁਨੇਹਾ_Max_ਲੰਮਾਈ * ਆਕਾਰ(ਸ਼ਬਦ_t));
        ਜੇ (len == se_L4_ਸੁਨੇਹਾ_Max_ਲੰਮਾਈ * ਆਕਾਰ(ਸ਼ਬਦ_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        ਸੈੱਟ_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        ਵਾਪਸ EXCEPTION_NONE;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਜੇ (w == SysDebugGetThreadAffinity) {
        ਸ਼ਬਦ_t cptr = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
        lookup_Cap_And_ਥਾਂ_ret_t lu_ret = lookup_Cap_And_ਥਾਂ(ਗੰਢ_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        ਸ਼ਬਦ_t cap_type = cap_get_capType(lu_ret.cap);
        ਜੇ (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        ਸ਼ਬਦ_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ, affinity);
        ਵਾਪਸ EXCEPTION_NONE;
    }
    ਜੇ (w == SysDebugSendIPI) {
        ਵਾਪਸ handle_SysDebugSendIPI();
    }
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DANGEROUS_ਕੋਡ_INJECTION
    ਜੇ (w == Sys_Debug_ਲੜੀਵਾਰ) {
        ((ਖਾਲੀ (*)(ਖਾਲੀ *))get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ))((ਖਾਲੀ *)get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread),
                                                                                                  ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ));
        ਵਾਪਸ EXCEPTION_NONE;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_X86_DANGEROUS_MSR
    ਜੇ (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
        ਜੇ (CONFIG_ਸ਼ਬਦ_ਮਾਪ == 32) {
            val = (uint64_t)getSyscallArg(0, ਨਿਰਾ) | ((uint64_t)getSyscallArg(1, ਨਿਰਾ) << 32);
        } ਨਹੀਂਤਾਂ {
            val = getSyscallArg(0, ਨਿਰਾ);
        }
        x86_wrmsr(reg, val);
        ਵਾਪਸ EXCEPTION_NONE;
    } ਨਹੀਂਤਾਂ ਜੇ (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
        val = x86_rdmsr(reg);
        ਪੂਰਨ ਅੰਕ = 1;
        ਜੇ (CONFIG_ਸ਼ਬਦ_ਮਾਪ == 32) {
            ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਨਿਰਾ, 0, val & 0xffffffff);
            ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਨਿਰਾ, 1, val >> 32);
            ਅੰਕ++;
        } ਨਹੀਂਤਾਂ {
            ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਨਿਰਾ, 0, val);
        }
        ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਅੰਕ)));
        ਵਾਪਸ EXCEPTION_NONE;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_BENCHMARKS
    ਚੋਣ (w) {
    ਹਾਲਤ SysBenchmarkFlushCaches:
        ਵਾਪਸ handle_SysBenchmarkFlushCaches();
    ਹਾਲਤ SysBenchmarkResetLog:
        ਵਾਪਸ handle_SysBenchmarkResetLog();
    ਹਾਲਤ SysBenchmarkFinalizeLog:
        ਵਾਪਸ handle_SysBenchmarkFinalizeLog();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
    ਹਾਲਤ Sys_Benchmark_ਸੈੱਟ_Log_ਬਫਰ:
        ਵਾਪਸ handle_Sys_Benchmark_ਸੈੱਟ_Log_ਬਫਰ();
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_UTILISATION
    ਹਾਲਤ SysBenchmarkGetThreadUtilisation:
        ਵਾਪਸ handle_SysBenchmarkGetThreadUtilisation();
    ਹਾਲਤ SysBenchmarkResetThreadUtilisation:
        ਵਾਪਸ handle_SysBenchmarkResetThreadUtilisation();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    ਹਾਲਤ Sys_Benchmark_Dump_All_ਧਾਗੇ_Utilisation:
        ਵਾਪਸ handle_Sys_Benchmark_Dump_All_ਧਾਗੇ_Utilisation();
    ਹਾਲਤ Sys_Benchmark_Reset_All_ਧਾਗੇ_Utilisation:
        ਵਾਪਸ handle_Sys_Benchmark_Reset_All_ਧਾਗੇ_Utilisation();
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
#ਜੇ_ਅੰਤ /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    ਹਾਲਤ SysBenchmarkNullSyscall:
        ਵਾਪਸ EXCEPTION_NONE;
    ਮੂਲ:
        ਰੋਕੋ; /* syscall is not for benchmarking */
    } /* end switch(w) */
#ਜੇ_ਅੰਤ /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ਸੈੱਟ_TLS_ਅਧਾਰ_SELF
        ਜੇ (w == Sys_ਸੈੱਟ_TLSBase)
        {
            ਸ਼ਬਦ_t tls_ਅਧਾਰ = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            ਵਾਪਸ Arch_ਸੈੱਟ_TLSRegister(tls_ਅਧਾਰ);
        }
#ਜੇ_ਅੰਤ
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(ਗੰਢ_STATE(ksCurThread));
    })

    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t handleUserLevelFault(ਸ਼ਬਦ_t w_a, ਸ਼ਬਦ_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(ਗੰਢ_STATE(ksCurThread));
    })
    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t ਸਥਿਤੀ = handleVMFault(ਗੰਢ_STATE(ksCurThread), vm_faultType);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE)
        {
            handleFault(ਗੰਢ_STATE(ksCurThread));
        }
    })

    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();

    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t ਪਹਿਲਾ_Phase, cptr_t cptr)
#ਨਹੀਂਤਾਂ
ਸਥਿਰ exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#ਜੇ_ਅੰਤ
{
    se_L4_ਸੁਨੇਹਾ_Info_t info;
    lookup_Cap_And_ਥਾਂ_ret_t lu_ret;
    ਸ਼ਬਦ_t *ਬਫਰ;
    exception_t ਸਥਿਤੀ;
    ਸ਼ਬਦ_t ਲੰਮਾਈ;
    tcb_t *thread;

    thread = ਗੰਢ_STATE(ksCurThread);

    info = ਸੁਨੇਹਾ_Info_From_ਸ਼ਬਦ(get_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ));
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    cptr_t cptr = get_ਰਜਿਸਟਰ(thread, cap_ਰਜਿਸਟਰ);
#ਜੇ_ਅੰਤ

    /* faulting section */
    lu_ret = lookup_Cap_And_ਥਾਂ(thread, cptr);

    ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        ਜੇ (isBlocking) {
            handleFault(thread);
        }

        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਬਫਰ = lookupIPCBuffer(false, thread);

    ਸਥਿਤੀ = lookupExtraCaps(thread, ਬਫਰ, info);

    ਜੇ (unlikely(ਸਥਿਤੀ != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        ਜੇ (isBlocking) {
            handleFault(thread);
        }
        ਵਾਪਸ EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    ਲੰਮਾਈ = se_L4_ਸੁਨੇਹਾ_Info_get_ਲੰਮਾਈ(info);
    ਜੇ (unlikely(ਲੰਮਾਈ > n_ਸੁਨੇਹਾ_Registers && !ਬਫਰ)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_ਸੁਨੇਹਾ_Registers);
        ਲੰਮਾਈ = n_ਸੁਨੇਹਾ_Registers;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਸਥਿਤੀ = decodeInvocation(se_L4_ਸੁਨੇਹਾ_Info_get_label(info), ਲੰਮਾਈ,
                              cptr, lu_ret.ਥਾਂ, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, ਪਹਿਲਾ_Phase, ਬਫਰ);
#ਨਹੀਂਤਾਂ
    ਸਥਿਤੀ = decodeInvocation(se_L4_ਸੁਨੇਹਾ_Info_get_label(info), ਲੰਮਾਈ,
                              cptr, lu_ret.ਥਾਂ, lu_ret.cap,
                              isBlocking, isCall, ਬਫਰ);
#ਜੇ_ਅੰਤ

    ਜੇ (unlikely(ਸਥਿਤੀ == EXCEPTION_PREEMPTED)) {
        ਵਾਪਸ ਸਥਿਤੀ;
    }

    ਜੇ (unlikely(ਸਥਿਤੀ == EXCEPTION_SYSCALL_ERROR)) {
        ਜੇ (isCall) {
            ਜਵਾਬ_From_Kernel_error(thread);
        }
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਜੇ (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        ਜੇ (isCall) {
            ਜਵਾਬ_From_Kernel_success_ਖਾਲੀ_ਥਾਵਾਂ(thread);
        }
        ਸੈੱਟ_Thread_State(thread, ThreadState_Running);
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਸਤਰਅੰਦਰ lookupCap_ret_t lookup_ਜਵਾਬ(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t ਜਵਾਬ_CPtr = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਜਵਾਬ_ਰਜਿਸਟਰ);
    lookupCap_ret_t lu_ret = lookupCap(ਗੰਢ_STATE(ksCurThread), ਜਵਾਬ_CPtr);
    ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(ਜਵਾਬ_CPtr, true);
        ਵਾਪਸ lu_ret;
    }

    ਜੇ (unlikely(cap_get_capType(lu_ret.cap) != cap_ਜਵਾਬ_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(ਜਵਾਬ_CPtr, true);
        lu_ret.ਸਥਿਤੀ = EXCEPTION_FAULT;
        ਵਾਪਸ lu_ret;
    }

    ਵਾਪਸ lu_ret;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ handle_ਜਵਾਬ(ਖਾਲੀ)
{
    cte_t *caller_ਥਾਂ;
    cap_t callerCap;

    caller_ਥਾਂ = TCB_PTR_CTE_PTR(ਗੰਢ_STATE(ksCurThread), tcbCaller);
    callerCap = caller_ਥਾਂ->cap;

    ਚੋਣ (cap_get_capType(callerCap)) {
    ਹਾਲਤ cap_ਜਵਾਬ_cap: {
        tcb_t *caller;

        ਜੇ (cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Master(callerCap)) {
            ਰੋਕੋ;
        }
        caller = TCB_PTR(cap_ਜਵਾਬ_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        ਪ੍ਰਤਿਜ੍ਞਾ(caller != ਗੰਢ_STATE(ksCurThread));
        do_ਜਵਾਬ_Transfer(ਗੰਢ_STATE(ksCurThread), caller, caller_ਥਾਂ,
                        cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Can_Grant(callerCap));
        ਵਾਪਸ;
    }

    ਹਾਲਤ cap_null_cap:
        /* Do nothing when no caller is pending */
        ਵਾਪਸ;

    ਮੂਲ:
        ਰੋਕੋ;
    }

    fail("handleReply: invalid caller cap");
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਖਾਲੀ handleRecv(bool_t isBlocking, bool_t can_ਜਵਾਬ)
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ handleRecv(bool_t isBlocking)
#ਜੇ_ਅੰਤ
{
    ਸ਼ਬਦ_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);

    lu_ret = lookupCap(ਗੰਢ_STATE(ksCurThread), epCPtr);

    ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ਗੰਢ_STATE(ksCurThread));
        ਵਾਪਸ;
    }

    ਚੋਣ (cap_get_capType(lu_ret.cap)) {
    ਹਾਲਤ cap_endpoint_cap:
        ਜੇ (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ਗੰਢ_STATE(ksCurThread));
            ਰੋਕੋ;
        }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t ਜਵਾਬ_cap = cap_null_cap_new();
        ਜੇ (can_ਜਵਾਬ) {
            lu_ret = lookup_ਜਵਾਬ();
            ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(ਗੰਢ_STATE(ksCurThread));
                ਵਾਪਸ;
            } ਨਹੀਂਤਾਂ {
                ਜਵਾਬ_cap = lu_ret.cap;
            }
        }
        receiveIPC(ਗੰਢ_STATE(ksCurThread), ep_cap, isBlocking, ਜਵਾਬ_cap);
#ਨਹੀਂਤਾਂ
        deleteCallerCap(ਗੰਢ_STATE(ksCurThread));
        receiveIPC(ਗੰਢ_STATE(ksCurThread), lu_ret.cap, isBlocking);
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;

    ਹਾਲਤ cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        ਜੇ (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != ਗੰਢ_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ਗੰਢ_STATE(ksCurThread));
            ਰੋਕੋ;
        }

        receiveSignal(ਗੰਢ_STATE(ksCurThread), lu_ret.cap, isBlocking);
        ਰੋਕੋ;
    }
    ਮੂਲ:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ਗੰਢ_STATE(ksCurThread));
        ਰੋਕੋ;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ mcs_Preemption_ਬਿੰਦੂ(ਖਾਲੀ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    ਜੇ (isSchedulable(ਗੰਢ_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } ਨਹੀਂਤਾਂ ਜੇ (sc_ਸਰਗਰਮ(ਗੰਢ_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(ਗੰਢ_STATE(ksConsumed), false);
    } ਨਹੀਂਤਾਂ {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        ਗੰਢ_STATE(ksConsumed) = 0;
    }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ handleRecv(isBlocking, can_ਜਵਾਬ) handleRecv(isBlocking)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mcs_Preemption_ਬਿੰਦੂ()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ handleInvocation(isCall, isBlocking, canDonate, ਪਹਿਲਾ_Phase, cptr) handleInvocation(isCall, isBlocking)
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ handleYield(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = ਗੰਢ_STATE(ksCurSC)->scConsumed + ਗੰਢ_STATE(ksConsumed);
    refill_t ਸਿਰ = *refill_ਸਿਰ(ਗੰਢ_STATE(ksCurSC));
    chargeBudget(ਸਿਰ.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    ਗੰਢ_STATE(ksCurSC)->scConsumed = consumed;
#ਨਹੀਂਤਾਂ
    tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(ਗੰਢ_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#ਜੇ_ਅੰਤ
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        ਚੋਣ (syscall)
        {
        ਹਾਲਤ SysSend:
            ret = handleInvocation(false, true, false, false, get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ));
            ਜੇ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ਬਿੰਦੂ();
                check_ਰੁਕਾਵਟ(/* was_interrupt_entry */ false);
            }

            ਰੋਕੋ;

        ਹਾਲਤ SysNBSend:
            ret = handleInvocation(false, false, false, false, get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ));
            ਜੇ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ਬਿੰਦੂ();
                check_ਰੁਕਾਵਟ(/* was_interrupt_entry */ false);
            }
            ਰੋਕੋ;

        ਹਾਲਤ SysCall:
            ret = handleInvocation(true, true, true, false, get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ));
            ਜੇ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ਬਿੰਦੂ();
                check_ਰੁਕਾਵਟ(/* was_interrupt_entry */ false);
            }
            ਰੋਕੋ;

        ਹਾਲਤ SysRecv:
            handleRecv(true, true);
            ਰੋਕੋ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਹਾਲਤ Sys_ਜਵਾਬ:
            handle_ਜਵਾਬ();
            ਰੋਕੋ;

        ਹਾਲਤ Sys_ਜਵਾਬ_Recv:
            handle_ਜਵਾਬ();
            handleRecv(true, true);
            ਰੋਕੋ;

#ਨਹੀਂਤਾਂ /* CONFIG_KERNEL_MCS */
        ਹਾਲਤ SysWait:
            handleRecv(true, false);
            ਰੋਕੋ;

        ਹਾਲਤ SysNBWait:
            handleRecv(false, false);
            ਰੋਕੋ;
        ਹਾਲਤ Sys_ਜਵਾਬ_Recv: {
            cptr_t ਜਵਾਬ = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਜਵਾਬ_ਰਜਿਸਟਰ);
            ret = handleInvocation(false, false, true, true, ਜਵਾਬ);
            /* reply is not preemptible, but to ease verification we check explicitly */
            ਜੇ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ਬਿੰਦੂ();
                check_ਰੁਕਾਵਟ(/* was_interrupt_entry */ false);
                ਰੋਕੋ;
            }
            handleRecv(true, true);
            ਰੋਕੋ;
        }

        ਹਾਲਤ SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            ਜੇ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ਬਿੰਦੂ();
                check_ਰੁਕਾਵਟ(/* was_interrupt_entry */ false);
                ਰੋਕੋ;
            }
            handleRecv(true, true);
            ਰੋਕੋ;
        }

        ਹਾਲਤ SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਜਵਾਬ_ਰਜਿਸਟਰ));
            ਜੇ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ਬਿੰਦੂ();
                check_ਰੁਕਾਵਟ(/* was_interrupt_entry */ false);
                ਰੋਕੋ;
            }
            handleRecv(true, false);
            ਰੋਕੋ;
#ਜੇ_ਅੰਤ
        ਹਾਲਤ SysNBRecv:
            handleRecv(false, true);
            ਰੋਕੋ;

        ਹਾਲਤ SysYield:
            handleYield();
            ਰੋਕੋ;

        ਮੂਲ:
            fail("Invalid syscall");
        }

    })

    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();

    ਵਾਪਸ EXCEPTION_NONE;
}
