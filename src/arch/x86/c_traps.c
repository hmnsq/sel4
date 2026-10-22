/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/fastpath/fastpath.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <machine/debug.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <sel4/arch/vmenter.h>

#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>

ಶೂನ್ಯ VISIBLE c_nested_interrupt(ಪೂರ್ಣಾಙ್ಕ irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    ಪ್ರತಿಜ್ಞಾ(ARCH_ಗಂಟು_STATE(x86KSPendingInterrupt) == int_invalid);
    ARCH_ಗಂಟು_STATE(x86KSPendingInterrupt) = irq;
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_interrupt(ಪೂರ್ಣಾಙ್ಕ irq, ಪೂರ್ಣಾಙ್ಕ syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    ಯದಿ (config_set(CONFIG_ARCH_IA32) && config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    ಗಂಟು_LOCK_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_entry_hook();

    ಯದಿ (irq == int_page_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_VMFault;
        ksKernelEntry.word = type;
#endif
        handleVMFaultEvent(type);
#ifdef CONFIG_HARDWARE_DEBUG_API
    } ಅನ್ಯಥಾ ಯದಿ (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_DebugFault;
        ksKernelEntry.word = ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#endif
        handleUserLevelDebugException(irq);
#endif /* CONFIG_HARDWARE_DEBUG_API */
    } ಅನ್ಯಥಾ ಯದಿ (irq < int_irq_min) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UserLevelFault;
        ksKernelEntry.word = irq;
#endif
        handleUserLevelFault(irq, ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } ಅನ್ಯಥಾ ಯದಿ (likely(irq < int_trap_min)) {
        ARCH_ಗಂಟು_STATE(x86KScurInterrupt) = irq;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_Interrupt;
        ksKernelEntry.word = irq;
#endif
        handleInterruptEntry();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } ಅನ್ಯಥಾ ಯದಿ (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } ಅನ್ಯಥಾ {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        ಪೂರ್ಣಾಙ್ಕ sys_ಅಂಕೆ;
        ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_ಅಂಕೆ = (irq << 24) | (syscall & 0x00ffffff);
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        ksKernelEntry.word = sys_ಅಂಕೆ;
#endif
        handleUnknownSyscall(sys_ಅಂಕೆ);
    }
    restore_user_context();
    UNREACHABLE();
}

ಶೂನ್ಯ NORETURN slowpath(syscall_t syscall)
{

#ifdef CONFIG_VTX
    ಯದಿ (syscall == SysVMEnter && ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbVCPU);
        ಯದಿ (ಗಂಟು_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(ಗಂಟು_STATE(ksCurThread)->tcbBoundNotification) == NtfnState_Active) {
            completeSignal(ಗಂಟು_STATE(ksCurThread)->tcbBoundNotification, ಗಂಟು_STATE(ksCurThread));
            setRegister(ಗಂಟು_STATE(ksCurThread), ಸಂದೇಶ_Info_Register, SEL4_VMENTER_RESULT_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } ಅನ್ಯಥಾ {
            setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#endif
    /* check for undefined syscall */
    ಯದಿ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ಅನ್ಯಥಾ {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t ಸಂದೇಶ_Info, syscall_t syscall, word_t reply)
#ಅನ್ಯಥಾ
ಶೂನ್ಯ VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t ಸಂದೇಶ_Info, syscall_t syscall)
#endif
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    ಯದಿ (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    ಯದಿ (config_set(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[NextIP] += 2;
    } ಅನ್ಯಥಾ {
        /* set FaultIP */
        setRegister(ಗಂಟು_STATE(ksCurThread), FaultIP, getRegister(ಗಂಟು_STATE(ksCurThread), NextIP) - 2);
    }

    ಗಂಟು_LOCK_SYS;

    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ಸಂದೇಶ_Info, syscall);
    ksKernelEntry.is_fastpath = 1;
#endif /* TRACK_KERNEL_ENTRIES */

#ifdef CONFIG_FASTPATH
    ಯದಿ (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, ಸಂದೇಶ_Info);
        UNREACHABLE();
    } ಅನ್ಯಥಾ ಯದಿ (syscall == (syscall_t)SysReplyRecv) {
#ifdef CONFIG_KERNEL_MCS
        fastpath_reply_recv(cptr, ಸಂದೇಶ_Info, reply);
#ಅನ್ಯಥಾ
        fastpath_reply_recv(cptr, ಸಂದೇಶ_Info);
#endif
        UNREACHABLE();
    }
#endif /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_VTX
ಶೂನ್ಯ VISIBLE NORETURN c_handle_vmexit(ಶೂನ್ಯ)
{
#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMExit;
#endif

    /* We *always* need to flush the rsb as a guest may have been able to train the rsb with kernel addresses */
    x86_flush_rsb();

    /* When we switched out of VMX mode the FS and GS registers were
     * clobbered and set to potentially undefined values. we need to
     * make sure we reload the correct values of FS and GS.
     * Unfortunately our cached values in x86KSCurrent[FG]SBase now
     * mismatch what is in the hardware. To force a reload to happen we
     * set the cached value to something that is guaranteed to not be
     * the target threads value, ensuring both the cache and the
     * hardware get updated.
     *
     * This needs to happen before the entry hook which will try to
     * restore the registers without having a means to determine whether
     * they may have been dirtied by a VM exit. */
#ifndef CONFIG_X86_64_VTX_64BIT_GUESTS
    tcb_t *cur_thread = ಗಂಟು_STATE(ksCurThread);
    ARCH_ಗಂಟು_STATE(x86KSCurrentGSBase) = -(word_t)1;
    ARCH_ಗಂಟು_STATE(x86KSCurrentFSBase) = -(word_t)1;
    x86_load_fsgs_base(cur_thread, CURRENT_CPU_INDEX());
#endif

    c_entry_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#endif
