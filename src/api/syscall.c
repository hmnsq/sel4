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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
#ಅನ್ತರ್ಭಾವ <smp/ipi.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
#ಅನ್ತರ್ಭಾವ <mode/machine/debug.h>
#ಆದರೆ_ಅಂತ್ಯ

/**
 * FIXME: This is a temporary hack to prevent the printing of incorrect
 *        spurious interrupt warnings on MCS when checkInterrupt() is called
 *        following preemptionPoint and the reason is because running
 *        out of sufficient budget, rather than an active IRQ.
 *        See issue https://github.com/seL4/seL4/issues/1540 and
 *        https://github.com/seL4/seL4/pull/1544.
 **/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IRQ_REPORTING
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ check_ಅಡಚಣೆ(bool_t was_ಅಡಚಣೆ_ನಮೂದು)
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ check_ಅಡಚಣೆ(ಶೂನ್ಯ)
#ಆದರೆ_ಅಂತ್ಯ
{
    irq_t irq;

    irq = get_ಸಕ್ರಿಯ_IRQ();
    ಆದರೆ (IRQT_ಗಮ್ಯ_IRQ(irq) != IRQT_ಗಮ್ಯ_IRQ(irqInvalid)) {
        handle_ಅಡಚಣೆ(irq);
    } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IRQ_REPORTING
        ಆದರೆ (was_ಅಡಚಣೆ_ನಮೂದು) {
            userError("Spurious interrupt!");
        }
#ಆದರೆ_ಅಂತ್ಯ
        handleSpuriousIRQ();
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_IRQ_REPORTING
/** Part of the temporary hack above **/
#ವ್ಯಾಖ್ಯಾನಿಸು check_ಅಡಚಣೆ(was_ಅಡಚಣೆ_ನಮೂದು) check_ಅಡಚಣೆ()
#ಆದರೆ_ಅಂತ್ಯ

/* The haskell function 'handleEvent' is split into 'handleXXX' variants
 * for each event causing a kernel entry */

exception_t handle_ಅಡಚಣೆ_ನಮೂದು(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (SMP_TERNARY(clh_is_self_ಒಳ_ಸರತಿ(), 1)) {
        updateTimestamp();
        checkBudget();
    }
#ಆದರೆ_ಅಂತ್ಯ

    check_ಅಡಚಣೆ(/* was_interrupt_entry */ true);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (SMP_TERNARY(clh_is_self_ಒಳ_ಸರತಿ(), 1)) {
#ಆದರೆ_ಅಂತ್ಯ
        ವೇಳಾಪಟ್ಟಿ();
        activateThread();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t handleUnknownSyscall(ಶಬ್ದ_t w)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    ಆದರೆ (w == SysDebugPutChar) {
        kernel_putchar(get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್));
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }
    ಆದರೆ (w == SysDebugDumpScheduler) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
        debug_dumpScheduler();
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    ಆದರೆ (w == SysDebugHalt) {
        tcb_t *ಬಳಸದ tptr = ಗಂಟು_STATE(ksCurThread);
        ಬರೆ("Debug halt syscall from user thread %p \"%s\"\n", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        halt();
    }
    ಆದರೆ (w == SysDebugSnapshot) {
        tcb_t *ಬಳಸದ tptr = ಗಂಟು_STATE(ksCurThread);
        ಬರೆ("Debug snapshot syscall from user thread %p \"%s\"\n",
               tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
        debug_capDL();
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }
    ಆದರೆ (w == SysDebugCapIdentify) {
        ಶಬ್ದ_t cptr = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
        lookup_Cap_And_ಸ್ಥಾನ_ret_t lu_ret = lookup_Cap_And_ಸ್ಥಾನ(ಗಂಟು_STATE(ksCurThread), cptr);
        ಶಬ್ದ_t cap_type = cap_get_capType(lu_ret.cap);
        ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್, cap_type);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಆದರೆ (w == SysDebugNameThread) {
        /* This is a syscall meant to aid debugging, so if anything goes wrong
         * then assume the system is completely misconfigured and halt */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
        ಶಬ್ದ_t len;
        ಶಬ್ದ_t cptr = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
        lookup_Cap_And_ಸ್ಥಾನ_ret_t lu_ret = lookup_Cap_And_ಸ್ಥಾನ(ಗಂಟು_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        ಶಬ್ದ_t cap_type = cap_get_capType(lu_ret.cap);
        ಆದರೆ (cap_type != cap_thread_cap) {
            userError("SysDebugNameThread: cap is not a TCB, halting");
            halt();
        }
        /* Add 1 to the IPC buffer to skip the message info word */
        name = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(lookupIPCBuffer(true, ಗಂಟು_STATE(ksCurThread)) + 1);
        ಆದರೆ (!name) {
            userError("SysDebugNameThread: Failed to lookup IPC buffer, halting");
            halt();
        }
        /* ensure the name isn't too long */
        len = strnlen(name, se_L4_ಸಂದೇಶ_Max_ಉದ್ದಳತೆ * ಗಾತ್ರ(ಶಬ್ದ_t));
        ಆದರೆ (len == se_L4_ಸಂದೇಶ_Max_ಉದ್ದಳತೆ * ಗಾತ್ರ(ಶಬ್ದ_t)) {
            userError("SysDebugNameThread: Name too long, halting");
            halt();
        }
        ಗಣ_Thread_Name(TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap)), name);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಆದರೆ (w == SysDebugGetThreadAffinity) {
        ಶಬ್ದ_t cptr = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
        lookup_Cap_And_ಸ್ಥಾನ_ret_t lu_ret = lookup_Cap_And_ಸ್ಥಾನ(ಗಂಟು_STATE(ksCurThread), cptr);
        /* ensure we got a TCB cap */
        ಶಬ್ದ_t cap_type = cap_get_capType(lu_ret.cap);
        ಆದರೆ (cap_type != cap_thread_cap) {
            userError("SysDebugGetThreadAffinity: cap is not a TCB, halting");
            halt();
        }
        ಶಬ್ದ_t affinity = TCB_PTR(cap_thread_cap_get_capTCBPtr(lu_ret.cap))->tcbAffinity;
        ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್, affinity);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }
    ಆದರೆ (w == SysDebugSendIPI) {
        ಹಿಂತಿರುಗಿ handle_SysDebugSendIPI();
    }
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DANGEROUS_ಸಂಕೇತ_INJECTION
    ಆದರೆ (w == Sys_Debug_ಸತತ) {
        ((ಶೂನ್ಯ (*)(ಶೂನ್ಯ *))get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್))((ಶೂನ್ಯ *)get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread),
                                                                                                  ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್));
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_X86_DANGEROUS_MSR
    ಆದರೆ (w == SysX86DangerousWRMSR) {
        uint64_t val;
        uint32_t reg = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
        ಆದರೆ (CONFIG_ಶಬ್ದ_ಗಾತ್ರ == 32) {
            val = (uint64_t)getSyscallArg(0, ಖಾಲಿ) | ((uint64_t)getSyscallArg(1, ಖಾಲಿ) << 32);
        } ಇಲ್ಲದಿದ್ದರೆ {
            val = getSyscallArg(0, ಖಾಲಿ);
        }
        x86_wrmsr(reg, val);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (w == SysX86DangerousRDMSR) {
        uint64_t val;
        uint32_t reg = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
        val = x86_rdmsr(reg);
        ಪೂರ್ಣಾಂಕ ಅಂಕೆ = 1;
        ಆದರೆ (CONFIG_ಶಬ್ದ_ಗಾತ್ರ == 32) {
            ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಖಾಲಿ, 0, val & 0xffffffff);
            ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಖಾಲಿ, 1, val >> 32);
            ಅಂಕೆ++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಖಾಲಿ, 0, val);
        }
        ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಅಂಕೆ)));
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ENABLE_BENCHMARKS
    ಆಯ್ಕೆ (w) {
    ಸ್ಥಿತಿ SysBenchmarkFlushCaches:
        ಹಿಂತಿರುಗಿ handle_SysBenchmarkFlushCaches();
    ಸ್ಥಿತಿ SysBenchmarkResetLog:
        ಹಿಂತಿರುಗಿ handle_SysBenchmarkResetLog();
    ಸ್ಥಿತಿ SysBenchmarkFinalizeLog:
        ಹಿಂತಿರುಗಿ handle_SysBenchmarkFinalizeLog();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
    ಸ್ಥಿತಿ Sys_Benchmark_ಗಣ_Log_ಬಫರ್:
        ಹಿಂತಿರುಗಿ handle_Sys_Benchmark_ಗಣ_Log_ಬಫರ್();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_UTILISATION
    ಸ್ಥಿತಿ SysBenchmarkGetThreadUtilisation:
        ಹಿಂತಿರುಗಿ handle_SysBenchmarkGetThreadUtilisation();
    ಸ್ಥಿತಿ SysBenchmarkResetThreadUtilisation:
        ಹಿಂತಿರುಗಿ handle_SysBenchmarkResetThreadUtilisation();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    ಸ್ಥಿತಿ Sys_Benchmark_Dump_All_ಎಳೆಗಳು_Utilisation:
        ಹಿಂತಿರುಗಿ handle_Sys_Benchmark_Dump_All_ಎಳೆಗಳು_Utilisation();
    ಸ್ಥಿತಿ Sys_Benchmark_Reset_All_ಎಳೆಗಳು_Utilisation:
        ಹಿಂತಿರುಗಿ handle_Sys_Benchmark_Reset_All_ಎಳೆಗಳು_Utilisation();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_BENCHMARK_TRACK_UTILISATION */
    ಸ್ಥಿತಿ SysBenchmarkNullSyscall:
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು; /* syscall is not for benchmarking */
    } /* end switch(w) */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ENABLE_BENCHMARKS */

    MCS_DO_IF_BUDGET({
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ಗಣ_TLS_ತಳ_SELF
        ಆದರೆ (w == Sys_ಗಣ_TLSBase)
        {
            ಶಬ್ದ_t tls_ತಳ = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
            /*
             * This updates the real register as opposed to the thread state
             * value. For many architectures, the TLS variables only get
             * updated on a thread switch.
             */
            ಹಿಂತಿರುಗಿ Arch_ಗಣ_TLSRegister(tls_ತಳ);
        }
#ಆದರೆ_ಅಂತ್ಯ
        current_fault = seL4_Fault_UnknownSyscall_new(w);
        handleFault(ಗಂಟು_STATE(ksCurThread));
    })

    ವೇಳಾಪಟ್ಟಿ();
    activateThread();

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t handleUserLevelFault(ಶಬ್ದ_t w_a, ಶಬ್ದ_t w_b)
{
    MCS_DO_IF_BUDGET({
        current_fault = seL4_Fault_UserException_new(w_a, w_b);
        handleFault(ಗಂಟು_STATE(ksCurThread));
    })
    ವೇಳಾಪಟ್ಟಿ();
    activateThread();

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t handleVMFaultEvent(vm_fault_type_t vm_faultType)
{
    MCS_DO_IF_BUDGET({

        exception_t ಸ್ಥಿತಿ_2 = handleVMFault(ಗಂಟು_STATE(ksCurThread), vm_faultType);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)
        {
            handleFault(ಗಂಟು_STATE(ksCurThread));
        }
    })

    ವೇಳಾಪಟ್ಟಿ();
    activateThread();

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ exception_t handleInvocation(bool_t isCall, bool_t isBlocking, bool_t canDonate, bool_t ಮೊದಲ_Phase, cptr_t cptr)
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ exception_t handleInvocation(bool_t isCall, bool_t isBlocking)
#ಆದರೆ_ಅಂತ್ಯ
{
    se_L4_ಸಂದೇಶ_Info_t info;
    lookup_Cap_And_ಸ್ಥಾನ_ret_t lu_ret;
    ಶಬ್ದ_t *ಬಫರ್;
    exception_t ಸ್ಥಿತಿ_2;
    ಶಬ್ದ_t ಉದ್ದಳತೆ;
    tcb_t *thread;

    thread = ಗಂಟು_STATE(ksCurThread);

    info = ಸಂದೇಶ_Info_From_ಶಬ್ದ(get_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್));
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    cptr_t cptr = get_ರಿಜಿಸ್ಟರ್(thread, cap_ರಿಜಿಸ್ಟರ್);
#ಆದರೆ_ಅಂತ್ಯ

    /* faulting section */
    lu_ret = lookup_Cap_And_ಸ್ಥಾನ(thread, cptr);

    ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        userError("Invocation of invalid cap #%lu.", cptr);
        current_fault = seL4_Fault_CapFault_new(cptr, false);

        ಆದರೆ (isBlocking) {
            handleFault(thread);
        }

        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಬಫರ್ = lookupIPCBuffer(false, thread);

    ಸ್ಥಿತಿ_2 = lookupExtraCaps(thread, ಬಫರ್, info);

    ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        userError("Lookup of extra caps failed.");
        ಆದರೆ (isBlocking) {
            handleFault(thread);
        }
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    /* Syscall error/Preemptible section */
    ಉದ್ದಳತೆ = se_L4_ಸಂದೇಶ_Info_get_ಉದ್ದಳತೆ(info);
    ಆದರೆ (unlikely(ಉದ್ದಳತೆ > n_ಸಂದೇಶ_Registers && !ಬಫರ್)) {
        /* If no IPC buffer is present the kernel truncates the maximum message length to n_msgRegisters.
         * The kernel truncates rather than returns because not all message transfer points in the kernel
         * are allowed to return an error.
         */
        userError("Warning: No IPC buffer for thread. Truncating message length to: %d.", n_ಸಂದೇಶ_Registers);
        ಉದ್ದಳತೆ = n_ಸಂದೇಶ_Registers;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ_2 = decodeInvocation(se_L4_ಸಂದೇಶ_Info_get_label(info), ಉದ್ದಳತೆ,
                              cptr, lu_ret.ಸ್ಥಾನ, lu_ret.cap,
                              isBlocking, isCall,
                              canDonate, ಮೊದಲ_Phase, ಬಫರ್);
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿತಿ_2 = decodeInvocation(se_L4_ಸಂದೇಶ_Info_get_label(info), ಉದ್ದಳತೆ,
                              cptr, lu_ret.ಸ್ಥಾನ, lu_ret.cap,
                              isBlocking, isCall, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 == EXCEPTION_PREEMPTED)) {
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 == EXCEPTION_SYSCALL_ERROR)) {
        ಆದರೆ (isCall) {
            ಉತ್ತರ_From_Kernel_error(thread);
        }
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಆದರೆ (unlikely(
            thread_state_get_tsType(thread->tcbState) == ThreadState_Restart)) {
        ಆದರೆ (isCall) {
            ಉತ್ತರ_From_Kernel_success_ಖಾಲಿಗಳು(thread);
        }
        ಗಣ_Thread_State(thread, ThreadState_Running);
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಸಾಲೊಳಗೆ lookupCap_ret_t lookup_ಉತ್ತರ(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t ಉತ್ತರ_CPtr = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಉತ್ತರ_ರಿಜಿಸ್ಟರ್);
    lookupCap_ret_t lu_ret = lookupCap(ಗಂಟು_STATE(ksCurThread), ಉತ್ತರ_CPtr);
    ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        userError("Reply cap lookup failed");
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(ಉತ್ತರ_CPtr, true);
        ಹಿಂತಿರುಗಿ lu_ret;
    }

    ಆದರೆ (unlikely(cap_get_capType(lu_ret.cap) != cap_ಉತ್ತರ_cap)) {
        userError("Cap in reply slot is not a reply cap");
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(ಉತ್ತರ_CPtr, true);
        lu_ret.ಸ್ಥಿತಿ_2 = EXCEPTION_FAULT;
        ಹಿಂತಿರುಗಿ lu_ret;
    }

    ಹಿಂತಿರುಗಿ lu_ret;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ handle_ಉತ್ತರ(ಶೂನ್ಯ)
{
    cte_t *caller_ಸ್ಥಾನ;
    cap_t callerCap;

    caller_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCaller);
    callerCap = caller_ಸ್ಥಾನ->cap;

    ಆಯ್ಕೆ (cap_get_capType(callerCap)) {
    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap: {
        tcb_t *caller;

        ಆದರೆ (cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Master(callerCap)) {
            ನಿಲ್ಲಿಸು;
        }
        caller = TCB_PTR(cap_ಉತ್ತರ_cap_get_cap_TCBPtr(callerCap));
        /* Haskell error:
         * "handleReply: caller must not be the current thread" */
        ಪ್ರತಿಜ್ಞಾ(caller != ಗಂಟು_STATE(ksCurThread));
        do_ಉತ್ತರ_Transfer(ಗಂಟು_STATE(ksCurThread), caller, caller_ಸ್ಥಾನ,
                        cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Can_Grant(callerCap));
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿತಿ cap_null_cap:
        /* Do nothing when no caller is pending */
        ಹಿಂತಿರುಗಿ;

    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    fail("handleReply: invalid caller cap");
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಶೂನ್ಯ handleRecv(bool_t isBlocking, bool_t can_ಉತ್ತರ)
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ handleRecv(bool_t isBlocking)
#ಆದರೆ_ಅಂತ್ಯ
{
    ಶಬ್ದ_t epCPtr;
    lookupCap_ret_t lu_ret;

    epCPtr = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);

    lu_ret = lookupCap(ಗಂಟು_STATE(ksCurThread), epCPtr);

    ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        /* current_lookup_fault has been set by lookupCap */
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ಗಂಟು_STATE(ksCurThread));
        ಹಿಂತಿರುಗಿ;
    }

    ಆಯ್ಕೆ (cap_get_capType(lu_ret.cap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಆದರೆ (unlikely(!cap_endpoint_cap_get_capCanReceive(lu_ret.cap))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ಗಂಟು_STATE(ksCurThread));
            ನಿಲ್ಲಿಸು;
        }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        cap_t ep_cap = lu_ret.cap;
        cap_t ಉತ್ತರ_cap = cap_null_cap_new();
        ಆದರೆ (can_ಉತ್ತರ) {
            lu_ret = lookup_ಉತ್ತರ();
            ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                /* lookup_fault has been set by lookupReply */
                handleFault(ಗಂಟು_STATE(ksCurThread));
                ಹಿಂತಿರುಗಿ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಉತ್ತರ_cap = lu_ret.cap;
            }
        }
        receiveIPC(ಗಂಟು_STATE(ksCurThread), ep_cap, isBlocking, ಉತ್ತರ_cap);
#ಇಲ್ಲದಿದ್ದರೆ
        deleteCallerCap(ಗಂಟು_STATE(ksCurThread));
        receiveIPC(ಗಂಟು_STATE(ksCurThread), lu_ret.cap, isBlocking);
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_notification_cap: {
        notification_t *ntfnPtr;
        tcb_t *boundTCB;
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(lu_ret.cap));
        boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        ಆದರೆ (unlikely(!cap_notification_cap_get_capNtfnCanReceive(lu_ret.cap)
                     || (boundTCB && boundTCB != ಗಂಟು_STATE(ksCurThread)))) {
            current_lookup_fault = lookup_fault_missing_capability_new(0);
            current_fault = seL4_Fault_CapFault_new(epCPtr, true);
            handleFault(ಗಂಟು_STATE(ksCurThread));
            ನಿಲ್ಲಿಸು;
        }

        receiveSignal(ಗಂಟು_STATE(ksCurThread), lu_ret.cap, isBlocking);
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        current_lookup_fault = lookup_fault_missing_capability_new(0);
        current_fault = seL4_Fault_CapFault_new(epCPtr, true);
        handleFault(ಗಂಟು_STATE(ksCurThread));
        ನಿಲ್ಲಿಸು;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ mcs_Preemption_ಬಿಂದು(ಶೂನ್ಯ)
{
    /* at this point we could be handling a timer interrupt which actually ends the current
     * threads timeslice. However, preemption is possible on revoke, which could have deleted
     * the current thread and/or the current scheduling context, rendering them invalid. */
    ಆದರೆ (isSchedulable(ಗಂಟು_STATE(ksCurThread))) {
        /* if the thread is schedulable, the tcb and scheduling context are still valid */
        checkBudget();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sc_ಸಕ್ರಿಯ(ಗಂಟು_STATE(ksCurSC))) {
        /* otherwise, if the thread is not schedulable, the SC could be valid - charge it if so */
        chargeBudget(ಗಂಟು_STATE(ksConsumed), false);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* If the current SC is no longer configured the time can no
         * longer be charged to it. Simply dropping the consumed time
         * here is equivalent to having charged the consumed time and
         * then having cleared the SC. */
        ಗಂಟು_STATE(ksConsumed) = 0;
    }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು handleRecv(isBlocking, can_ಉತ್ತರ) handleRecv(isBlocking)
#ವ್ಯಾಖ್ಯಾನಿಸು mcs_Preemption_ಬಿಂದು()
#ವ್ಯಾಖ್ಯಾನಿಸು handleInvocation(isCall, isBlocking, canDonate, ಮೊದಲ_Phase, cptr) handleInvocation(isCall, isBlocking)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ handleYield(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* Yield the current remaining budget */
    ticks_t consumed = ಗಂಟು_STATE(ksCurSC)->scConsumed + ಗಂಟು_STATE(ksConsumed);
    refill_t ತಲೆ = *refill_ತಲೆ(ಗಂಟು_STATE(ksCurSC));
    chargeBudget(ತಲೆ.rAmount, false);
    /* Manually updated the scConsumed so that the full timeslice isn't added, just what was consumed */
    ಗಂಟು_STATE(ksCurSC)->scConsumed = consumed;
#ಇಲ್ಲದಿದ್ದರೆ
    tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(ಗಂಟು_STATE(ksCurThread));
    SCHED_APPEND_CURRENT_TCB;
    rescheduleRequired();
#ಆದರೆ_ಅಂತ್ಯ
}

exception_t handleSyscall(syscall_t syscall)
{
    exception_t ret;
    MCS_DO_IF_BUDGET({
        ಆಯ್ಕೆ (syscall)
        {
        ಸ್ಥಿತಿ SysSend:
            ret = handleInvocation(false, true, false, false, get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್));
            ಆದರೆ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ಬಿಂದು();
                check_ಅಡಚಣೆ(/* was_interrupt_entry */ false);
            }

            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ SysNBSend:
            ret = handleInvocation(false, false, false, false, get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್));
            ಆದರೆ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ಬಿಂದು();
                check_ಅಡಚಣೆ(/* was_interrupt_entry */ false);
            }
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ SysCall:
            ret = handleInvocation(true, true, true, false, get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್));
            ಆದರೆ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ಬಿಂದು();
                check_ಅಡಚಣೆ(/* was_interrupt_entry */ false);
            }
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ SysRecv:
            handleRecv(true, true);
            ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
        ಸ್ಥಿತಿ Sys_ಉತ್ತರ:
            handle_ಉತ್ತರ();
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ Sys_ಉತ್ತರ_Recv:
            handle_ಉತ್ತರ();
            handleRecv(true, true);
            ನಿಲ್ಲಿಸು;

#ಇಲ್ಲದಿದ್ದರೆ /* CONFIG_KERNEL_MCS */
        ಸ್ಥಿತಿ SysWait:
            handleRecv(true, false);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ SysNBWait:
            handleRecv(false, false);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ Sys_ಉತ್ತರ_Recv: {
            cptr_t ಉತ್ತರ = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಉತ್ತರ_ರಿಜಿಸ್ಟರ್);
            ret = handleInvocation(false, false, true, true, ಉತ್ತರ);
            /* reply is not preemptible, but to ease verification we check explicitly */
            ಆದರೆ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ಬಿಂದು();
                check_ಅಡಚಣೆ(/* was_interrupt_entry */ false);
                ನಿಲ್ಲಿಸು;
            }
            handleRecv(true, true);
            ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ SysNBSendRecv: {
            cptr_t dest = getNBSendRecvDest();
            ret = handleInvocation(false, false, true, true, dest);
            ಆದರೆ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ಬಿಂದು();
                check_ಅಡಚಣೆ(/* was_interrupt_entry */ false);
                ನಿಲ್ಲಿಸು;
            }
            handleRecv(true, true);
            ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ SysNBSendWait:
            ret = handleInvocation(false, false, true, true, get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಉತ್ತರ_ರಿಜಿಸ್ಟರ್));
            ಆದರೆ (unlikely(ret != EXCEPTION_NONE)) {
                mcs_Preemption_ಬಿಂದು();
                check_ಅಡಚಣೆ(/* was_interrupt_entry */ false);
                ನಿಲ್ಲಿಸು;
            }
            handleRecv(true, false);
            ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
        ಸ್ಥಿತಿ SysNBRecv:
            handleRecv(false, true);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ SysYield:
            handleYield();
            ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
            fail("Invalid syscall");
        }

    })

    ವೇಳಾಪಟ್ಟಿ();
    activateThread();

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
