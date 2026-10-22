/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <model/statedata.h>
#समावेश <machine/fpu.h>
#समावेश <arch/fastpath/fastpath.h>
#समावेश <arch/kernel/traps.h>
#समावेश <machine/debug.h>
#समावेश <arch/object/vcpu.h>
#समावेश <api/syscall.h>
#समावेश <sel4/arch/vmenter.h>

#समावेश <benchmark/benchmark_track.h>
#समावेश <benchmark/benchmark_utilisation.h>

व्योम VISIBLE c_nested_interrupt(पूर्णांक irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    निश्चित(ARCH_गाँठ_STATE(x86KSPendingInterrupt) == int_invalid);
    ARCH_गाँठ_STATE(x86KSPendingInterrupt) = irq;
}

व्योम VISIBLE NORETURN c_handle_interrupt(पूर्णांक irq, पूर्णांक syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    अगर (config_set(CONFIG_ARCH_IA32) && config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    गाँठ_LOCK_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_entry_hook();

    अगर (irq == int_page_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (गाँठ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_VMFault;
        ksKernelEntry.word = type;
#पूर्ण_अगर
        handleVMFaultEvent(type);
#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
    } अन्यथा अगर (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_DebugFault;
        ksKernelEntry.word = गाँठ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#पूर्ण_अगर
        handleUserLevelDebugException(irq);
#पूर्ण_अगर /* CONFIG_HARDWARE_DEBUG_API */
    } अन्यथा अगर (irq < int_irq_min) {
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UserLevelFault;
        ksKernelEntry.word = irq;
#पूर्ण_अगर
        handleUserLevelFault(irq, गाँठ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } अन्यथा अगर (likely(irq < int_trap_min)) {
        ARCH_गाँठ_STATE(x86KScurInterrupt) = irq;
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_Interrupt;
        ksKernelEntry.word = irq;
#पूर्ण_अगर
        handleInterruptEntry();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } अन्यथा अगर (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } अन्यथा {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        पूर्णांक sys_अंक;
        गाँठ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_अंक = (irq << 24) | (syscall & 0x00ffffff);
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        ksKernelEntry.word = sys_अंक;
#पूर्ण_अगर
        handleUnknownSyscall(sys_अंक);
    }
    restore_user_context();
    UNREACHABLE();
}

व्योम NORETURN slowpath(syscall_t syscall)
{

#अगर_घोषित CONFIG_VTX
    अगर (syscall == SysVMEnter && गाँठ_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(गाँठ_STATE(ksCurThread)->tcbArch.tcbVCPU);
        अगर (गाँठ_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(गाँठ_STATE(ksCurThread)->tcbBoundNotification) == NtfnState_Active) {
            completeSignal(गाँठ_STATE(ksCurThread)->tcbBoundNotification, गाँठ_STATE(ksCurThread));
            setRegister(गाँठ_STATE(ksCurThread), संदेश_Info_Register, SEL4_VMENTER_परिणाम_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } अन्यथा {
            setThreadState(गाँठ_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#पूर्ण_अगर
    /* check for undefined syscall */
    अगर (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#पूर्ण_अगर /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } अन्यथा {
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#पूर्ण_अगर /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t संदेश_Info, syscall_t syscall, word_t reply)
#अन्यथा
व्योम VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t संदेश_Info, syscall_t syscall)
#पूर्ण_अगर
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    अगर (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    अगर (config_set(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        गाँठ_STATE(ksCurThread)->tcbArch.tcbContext.registers[अग्र_IP] += 2;
    } अन्यथा {
        /* set FaultIP */
        setRegister(गाँठ_STATE(ksCurThread), FaultIP, getRegister(गाँठ_STATE(ksCurThread), अग्र_IP) - 2);
    }

    गाँठ_LOCK_SYS;

    c_entry_hook();

#अगर_घोषित TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, syscall);
    ksKernelEntry.is_fastpath = 1;
#पूर्ण_अगर /* TRACK_KERNEL_ENTRIES */

#अगर_घोषित CONFIG_FASTPATH
    अगर (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, संदेश_Info);
        UNREACHABLE();
    } अन्यथा अगर (syscall == (syscall_t)SysReplyRecv) {
#अगर_घोषित CONFIG_KERNEL_MCS
        fastpath_reply_recv(cptr, संदेश_Info, reply);
#अन्यथा
        fastpath_reply_recv(cptr, संदेश_Info);
#पूर्ण_अगर
        UNREACHABLE();
    }
#पूर्ण_अगर /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#अगर_घोषित CONFIG_VTX
व्योम VISIBLE NORETURN c_handle_vmexit(व्योम)
{
#अगर_घोषित TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMExit;
#पूर्ण_अगर

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
#अगर_अघोषित CONFIG_X86_64_VTX_64BIT_GUESTS
    tcb_t *cur_thread = गाँठ_STATE(ksCurThread);
    ARCH_गाँठ_STATE(x86KSCurrentGSBase) = -(word_t)1;
    ARCH_गाँठ_STATE(x86KSCurrentFSBase) = -(word_t)1;
    x86_load_fsgs_base(cur_thread, CURRENT_CPU_सूचकांक());
#पूर्ण_अगर

    c_entry_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#पूर्ण_अगर
