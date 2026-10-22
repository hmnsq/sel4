/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/fastpath/fastpath.h>
#अन्तर्भाव <arch/kernel/traps.h>
#अन्तर्भाव <machine/debug.h>
#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <sel4/arch/vmenter.h>

#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>

शून्य VISIBLE c_nested_interrupt(पूर्णाङ्क irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    प्रतिज्ञा(ARCH_ग्रन्थिः_STATE(x86KSPendingInterrupt) == int_invalid);
    ARCH_ग्रन्थिः_STATE(x86KSPendingInterrupt) = irq;
}

शून्य VISIBLE NORETURN c_handle_interrupt(पूर्णाङ्क irq, पूर्णाङ्क syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    सचे (config_set(CONFIG_ARCH_IA32) && config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    ग्रन्थिः_LOCK_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_entry_hook();

    सचे (irq == int_page_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_VMFault;
        ksKernelEntry.word = type;
#endif
        handleVMFaultEvent(type);
#ifdef CONFIG_HARDWARE_DEBUG_API
    } अञ्ञथा सचे (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_DebugFault;
        ksKernelEntry.word = ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#endif
        handleUserLevelDebugException(irq);
#endif /* CONFIG_HARDWARE_DEBUG_API */
    } अञ्ञथा सचे (irq < int_irq_min) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UserLevelFault;
        ksKernelEntry.word = irq;
#endif
        handleUserLevelFault(irq, ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } अञ्ञथा सचे (likely(irq < int_trap_min)) {
        ARCH_ग्रन्थिः_STATE(x86KScurInterrupt) = irq;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_Interrupt;
        ksKernelEntry.word = irq;
#endif
        handleInterruptEntry();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } अञ्ञथा सचे (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } अञ्ञथा {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        पूर्णाङ्क sys_अङ्कः;
        ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_अङ्कः = (irq << 24) | (syscall & 0x00ffffff);
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        ksKernelEntry.word = sys_अङ्कः;
#endif
        handleUnknownSyscall(sys_अङ्कः);
    }
    restore_user_context();
    UNREACHABLE();
}

शून्य NORETURN slowpath(syscall_t syscall)
{

#ifdef CONFIG_VTX
    सचे (syscall == SysVMEnter && ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbVCPU);
        सचे (ग्रन्थिः_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(ग्रन्थिः_STATE(ksCurThread)->tcbBoundNotification) == NtfnState_Active) {
            completeSignal(ग्रन्थिः_STATE(ksCurThread)->tcbBoundNotification, ग्रन्थिः_STATE(ksCurThread));
            setRegister(ग्रन्थिः_STATE(ksCurThread), सन्देशः_Info_Register, SEL4_VMENTER_RESULT_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } अञ्ञथा {
            setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#endif
    /* check for undefined syscall */
    सचे (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } अञ्ञथा {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
शून्य VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t सन्देशः_Info, syscall_t syscall, word_t reply)
#अञ्ञथा
शून्य VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t सन्देशः_Info, syscall_t syscall)
#endif
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    सचे (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    सचे (config_set(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[NextIP] += 2;
    } अञ्ञथा {
        /* set FaultIP */
        setRegister(ग्रन्थिः_STATE(ksCurThread), FaultIP, getRegister(ग्रन्थिः_STATE(ksCurThread), NextIP) - 2);
    }

    ग्रन्थिः_LOCK_SYS;

    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, syscall);
    ksKernelEntry.is_fastpath = 1;
#endif /* TRACK_KERNEL_ENTRIES */

#ifdef CONFIG_FASTPATH
    सचे (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, सन्देशः_Info);
        UNREACHABLE();
    } अञ्ञथा सचे (syscall == (syscall_t)SysReplyRecv) {
#ifdef CONFIG_KERNEL_MCS
        fastpath_reply_recv(cptr, सन्देशः_Info, reply);
#अञ्ञथा
        fastpath_reply_recv(cptr, सन्देशः_Info);
#endif
        UNREACHABLE();
    }
#endif /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_VTX
शून्य VISIBLE NORETURN c_handle_vmexit(शून्य)
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
    tcb_t *cur_thread = ग्रन्थिः_STATE(ksCurThread);
    ARCH_ग्रन्थिः_STATE(x86KSCurrentGSBase) = -(word_t)1;
    ARCH_ग्रन्थिः_STATE(x86KSCurrentFSBase) = -(word_t)1;
    x86_load_fsgs_base(cur_thread, CURRENT_CPU_INDEX());
#endif

    c_entry_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#endif
