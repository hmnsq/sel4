/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/fastpath/fastpath.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/traps.h>
#ଅନ୍ତର୍ଭାଵ <machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/vcpu.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <sel4/arch/vmenter.h>

#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_track.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_utilisation.h>

ଶୂନ୍ୟ VISIBLE c_nested_interrupt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    ପ୍ରତିଜ୍ଞା(ARCH_ଗଣ୍ଠି_STATE(x86KSPendingInterrupt) == int_invalid);
    ARCH_ଗଣ୍ଠି_STATE(x86KSPendingInterrupt) = irq;
}

ଶୂନ୍ୟ VISIBLE NORETURN c_handle_interrupt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq, ପୂର୍ଣ୍ଣସଂଖ୍ୟା syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    ଯଦି (config_set(CONFIG_ARCH_IA32) && config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    ଗଣ୍ଠି_LOCK_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_entry_hook();

    ଯଦି (irq == int_page_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_VMFault;
        ksKernelEntry.word = type;
#endif
        handleVMFaultEvent(type);
#ifdef CONFIG_HARDWARE_DEBUG_API
    } ଅନ୍ୟଥା ଯଦି (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_DebugFault;
        ksKernelEntry.word = ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#endif
        handleUserLevelDebugException(irq);
#endif /* CONFIG_HARDWARE_DEBUG_API */
    } ଅନ୍ୟଥା ଯଦି (irq < int_irq_min) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UserLevelFault;
        ksKernelEntry.word = irq;
#endif
        handleUserLevelFault(irq, ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } ଅନ୍ୟଥା ଯଦି (likely(irq < int_trap_min)) {
        ARCH_ଗଣ୍ଠି_STATE(x86KScurInterrupt) = irq;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_Interrupt;
        ksKernelEntry.word = irq;
#endif
        handleInterruptEntry();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } ଅନ୍ୟଥା ଯଦି (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } ଅନ୍ୟଥା {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sys_ଅଙ୍କ;
        ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_ଅଙ୍କ = (irq << 24) | (syscall & 0x00ffffff);
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        ksKernelEntry.word = sys_ଅଙ୍କ;
#endif
        handleUnknownSyscall(sys_ଅଙ୍କ);
    }
    restore_user_context();
    UNREACHABLE();
}

ଶୂନ୍ୟ NORETURN slowpath(syscall_t syscall)
{

#ifdef CONFIG_VTX
    ଯଦି (syscall == SysVMEnter && ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbVCPU);
        ଯଦି (ଗଣ୍ଠି_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(ଗଣ୍ଠି_STATE(ksCurThread)->tcbBoundNotification) == NtfnState_Active) {
            completeSignal(ଗଣ୍ଠି_STATE(ksCurThread)->tcbBoundNotification, ଗଣ୍ଠି_STATE(ksCurThread));
            setRegister(ଗଣ୍ଠି_STATE(ksCurThread), ସନ୍ଦେଶ_Info_Register, SEL4_VMENTER_RESULT_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } ଅନ୍ୟଥା {
            setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#endif
    /* check for undefined syscall */
    ଯଦି (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ଅନ୍ୟଥା {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t ସନ୍ଦେଶ_Info, syscall_t syscall, word_t reply)
#ଅନ୍ୟଥା
ଶୂନ୍ୟ VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t ସନ୍ଦେଶ_Info, syscall_t syscall)
#endif
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    ଯଦି (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    ଯଦି (config_set(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbContext.registers[NextIP] += 2;
    } ଅନ୍ୟଥା {
        /* set FaultIP */
        setRegister(ଗଣ୍ଠି_STATE(ksCurThread), FaultIP, getRegister(ଗଣ୍ଠି_STATE(ksCurThread), NextIP) - 2);
    }

    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ସନ୍ଦେଶ_Info, syscall);
    ksKernelEntry.is_fastpath = 1;
#endif /* TRACK_KERNEL_ENTRIES */

#ifdef CONFIG_FASTPATH
    ଯଦି (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, ସନ୍ଦେଶ_Info);
        UNREACHABLE();
    } ଅନ୍ୟଥା ଯଦି (syscall == (syscall_t)SysReplyRecv) {
#ifdef CONFIG_KERNEL_MCS
        fastpath_reply_recv(cptr, ସନ୍ଦେଶ_Info, reply);
#ଅନ୍ୟଥା
        fastpath_reply_recv(cptr, ସନ୍ଦେଶ_Info);
#endif
        UNREACHABLE();
    }
#endif /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_VTX
ଶୂନ୍ୟ VISIBLE NORETURN c_handle_vmexit(ଶୂନ୍ୟ)
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
    tcb_t *cur_thread = ଗଣ୍ଠି_STATE(ksCurThread);
    ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentGSBase) = -(word_t)1;
    ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentFSBase) = -(word_t)1;
    x86_load_fsgs_base(cur_thread, CURRENT_CPU_INDEX());
#endif

    c_entry_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#endif
