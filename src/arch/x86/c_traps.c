/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <machine/fpu.h>
#அந்தர்பாவ <arch/fastpath/fastpath.h>
#அந்தர்பாவ <arch/kernel/traps.h>
#அந்தர்பாவ <machine/debug.h>
#அந்தர்பாவ <arch/object/vcpu.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <sel4/arch/vmenter.h>

#அந்தர்பாவ <benchmark/benchmark_track.h>
#அந்தர்பாவ <benchmark/benchmark_utilisation.h>

வெறுமை VISIBLE c_nested_interrupt(முழு irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    ப்ரதிஜ்ஞா(ARCH_முனை_STATE(x86KSPendingInterrupt) == int_invalid);
    ARCH_முனை_STATE(x86KSPendingInterrupt) = irq;
}

வெறுமை VISIBLE NORETURN c_handle_interrupt(முழு irq, முழு syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    எனில் (config_set(CONFIG_ARCH_IA32) && config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    முனை_LOCK_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_entry_hook();

    எனில் (irq == int_page_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (முனை_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_VMFault;
        ksKernelEntry.word = type;
#endif
        handleVMFaultEvent(type);
#ifdef CONFIG_HARDWARE_DEBUG_API
    } இல்லையெனில் எனில் (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_DebugFault;
        ksKernelEntry.word = முனை_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#endif
        handleUserLevelDebugException(irq);
#endif /* CONFIG_HARDWARE_DEBUG_API */
    } இல்லையெனில் எனில் (irq < int_irq_min) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UserLevelFault;
        ksKernelEntry.word = irq;
#endif
        handleUserLevelFault(irq, முனை_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } இல்லையெனில் எனில் (likely(irq < int_trap_min)) {
        ARCH_முனை_STATE(x86KScurInterrupt) = irq;
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_Interrupt;
        ksKernelEntry.word = irq;
#endif
        handleInterruptEntry();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } இல்லையெனில் எனில் (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } இல்லையெனில் {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        முழு sys_இலக்கம்;
        முனை_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_இலக்கம் = (irq << 24) | (syscall & 0x00ffffff);
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        ksKernelEntry.word = sys_இலக்கம்;
#endif
        handleUnknownSyscall(sys_இலக்கம்);
    }
    restore_user_context();
    UNREACHABLE();
}

வெறுமை NORETURN slowpath(syscall_t syscall)
{

#ifdef CONFIG_VTX
    எனில் (syscall == SysVMEnter && முனை_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(முனை_STATE(ksCurThread)->tcbArch.tcbVCPU);
        எனில் (முனை_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(முனை_STATE(ksCurThread)->tcbBoundNotification) == NtfnState_Active) {
            completeSignal(முனை_STATE(ksCurThread)->tcbBoundNotification, முனை_STATE(ksCurThread));
            setRegister(முனை_STATE(ksCurThread), செய்தி_Info_Register, SEL4_VMENTER_RESULT_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } இல்லையெனில் {
            setThreadState(முனை_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#endif
    /* check for undefined syscall */
    எனில் (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } இல்லையெனில் {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
வெறுமை VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t செய்தி_Info, syscall_t syscall, word_t reply)
#இல்லையெனில்
வெறுமை VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t செய்தி_Info, syscall_t syscall)
#endif
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    எனில் (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    எனில் (config_set(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        முனை_STATE(ksCurThread)->tcbArch.tcbContext.registers[NextIP] += 2;
    } இல்லையெனில் {
        /* set FaultIP */
        setRegister(முனை_STATE(ksCurThread), FaultIP, getRegister(முனை_STATE(ksCurThread), NextIP) - 2);
    }

    முனை_LOCK_SYS;

    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, செய்தி_Info, syscall);
    ksKernelEntry.is_fastpath = 1;
#endif /* TRACK_KERNEL_ENTRIES */

#ifdef CONFIG_FASTPATH
    எனில் (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, செய்தி_Info);
        UNREACHABLE();
    } இல்லையெனில் எனில் (syscall == (syscall_t)SysReplyRecv) {
#ifdef CONFIG_KERNEL_MCS
        fastpath_reply_recv(cptr, செய்தி_Info, reply);
#இல்லையெனில்
        fastpath_reply_recv(cptr, செய்தி_Info);
#endif
        UNREACHABLE();
    }
#endif /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_VTX
வெறுமை VISIBLE NORETURN c_handle_vmexit(வெறுமை)
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
    tcb_t *cur_thread = முனை_STATE(ksCurThread);
    ARCH_முனை_STATE(x86KSCurrentGSBase) = -(word_t)1;
    ARCH_முனை_STATE(x86KSCurrentFSBase) = -(word_t)1;
    x86_load_fsgs_base(cur_thread, CURRENT_CPU_INDEX());
#endif

    c_entry_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#endif
