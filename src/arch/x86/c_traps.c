/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <machine/fpu.h>
#અન્તર્ભાવ <arch/fastpath/fastpath.h>
#અન્તર્ભાવ <arch/kernel/traps.h>
#અન્તર્ભાવ <machine/debug.h>
#અન્તર્ભાવ <arch/object/vcpu.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <sel4/arch/vmenter.h>

#અન્તર્ભાવ <benchmark/benchmark_track.h>
#અન્તર્ભાવ <benchmark/benchmark_utilisation.h>

શૂન્ય VISIBLE c_nested_interrupt(પૂર્ણાંક irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    પ્રતિજ્ઞા(ARCH_ગાંઠ_STATE(x86KSPendingInterrupt) == int_invalid);
    ARCH_ગાંઠ_STATE(x86KSPendingInterrupt) = irq;
}

શૂન્ય VISIBLE NORETURN c_handle_interrupt(પૂર્ણાંક irq, પૂર્ણાંક syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    જો (config_set(CONFIG_ARCH_IA32) && config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    ગાંઠ_LOCK_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_entry_hook();

    જો (irq == int_page_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (ગાંઠ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_VMFault;
        ksKernelEntry.word = type;
#endif
        handleVMFaultEvent(type);
#ifdef CONFIG_HARDWARE_DEBUG_API
    } નહીંતર જો (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_DebugFault;
        ksKernelEntry.word = ગાંઠ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#endif
        handleUserLevelDebugException(irq);
#endif /* CONFIG_HARDWARE_DEBUG_API */
    } નહીંતર જો (irq < int_irq_min) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UserLevelFault;
        ksKernelEntry.word = irq;
#endif
        handleUserLevelFault(irq, ગાંઠ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } નહીંતર જો (likely(irq < int_trap_min)) {
        ARCH_ગાંઠ_STATE(x86KScurInterrupt) = irq;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_Interrupt;
        ksKernelEntry.word = irq;
#endif
        handleInterruptEntry();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } નહીંતર જો (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } નહીંતર {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        પૂર્ણાંક sys_અંક;
        ગાંઠ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_અંક = (irq << 24) | (syscall & 0x00ffffff);
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        ksKernelEntry.word = sys_અંક;
#endif
        handleUnknownSyscall(sys_અંક);
    }
    restore_user_context();
    UNREACHABLE();
}

શૂન્ય NORETURN slowpath(syscall_t syscall)
{

#ifdef CONFIG_VTX
    જો (syscall == SysVMEnter && ગાંઠ_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(ગાંઠ_STATE(ksCurThread)->tcbArch.tcbVCPU);
        જો (ગાંઠ_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(ગાંઠ_STATE(ksCurThread)->tcbBoundNotification) == NtfnState_Active) {
            completeSignal(ગાંઠ_STATE(ksCurThread)->tcbBoundNotification, ગાંઠ_STATE(ksCurThread));
            setRegister(ગાંઠ_STATE(ksCurThread), સંદેશ_Info_Register, SEL4_VMENTER_RESULT_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } નહીંતર {
            setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#endif
    /* check for undefined syscall */
    જો (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } નહીંતર {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
શૂન્ય VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t સંદેશ_Info, syscall_t syscall, word_t reply)
#નહીંતર
શૂન્ય VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t સંદેશ_Info, syscall_t syscall)
#endif
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    જો (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    જો (config_set(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        ગાંઠ_STATE(ksCurThread)->tcbArch.tcbContext.registers[NextIP] += 2;
    } નહીંતર {
        /* set FaultIP */
        setRegister(ગાંઠ_STATE(ksCurThread), FaultIP, getRegister(ગાંઠ_STATE(ksCurThread), NextIP) - 2);
    }

    ગાંઠ_LOCK_SYS;

    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, સંદેશ_Info, syscall);
    ksKernelEntry.is_fastpath = 1;
#endif /* TRACK_KERNEL_ENTRIES */

#ifdef CONFIG_FASTPATH
    જો (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, સંદેશ_Info);
        UNREACHABLE();
    } નહીંતર જો (syscall == (syscall_t)SysReplyRecv) {
#ifdef CONFIG_KERNEL_MCS
        fastpath_reply_recv(cptr, સંદેશ_Info, reply);
#નહીંતર
        fastpath_reply_recv(cptr, સંદેશ_Info);
#endif
        UNREACHABLE();
    }
#endif /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_VTX
શૂન્ય VISIBLE NORETURN c_handle_vmexit(શૂન્ય)
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
    tcb_t *cur_thread = ગાંઠ_STATE(ksCurThread);
    ARCH_ગાંઠ_STATE(x86KSCurrentGSBase) = -(word_t)1;
    ARCH_ગાંઠ_STATE(x86KSCurrentFSBase) = -(word_t)1;
    x86_load_fsgs_base(cur_thread, CURRENT_CPU_INDEX());
#endif

    c_entry_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#endif
