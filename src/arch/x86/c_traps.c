/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/fastpath/fastpath.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/traps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/arch/vmenter.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_utilisation.h>

ਖਾਲੀ VISIBLE c_nested_ਰੁਕਾਵਟ(ਪੂਰਨ irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    ਪ੍ਰਤਿਜ੍ਞਾ(ARCH_ਗੰਢ_STATE(x86_KSPending_ਰੁਕਾਵਟ) == int_invalid);
    ARCH_ਗੰਢ_STATE(x86_KSPending_ਰੁਕਾਵਟ) = irq;
}

ਖਾਲੀ VISIBLE NORETURN c_handle_ਰੁਕਾਵਟ(ਪੂਰਨ irq, ਪੂਰਨ syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_IA32) && config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    ਗੰਢ_ਤਾਲਾ_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_ਇੰਦਰਾਜ_hook();

    ਜੇ (irq == int_ਪੰਨਾ_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_VMFault;
        ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = type;
#ਜੇ_ਅੰਤ
        handleVMFaultEvent(type);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    } ਨਹੀਂਤਾਂ ਜੇ (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_Debug_Fault;
        ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#ਜੇ_ਅੰਤ
        handleUserLevelDebugException(irq);
#ਜੇ_ਅੰਤ /* CONFIG_HARDWARE_DEBUG_API */
    } ਨਹੀਂਤਾਂ ਜੇ (irq < int_irq_min) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_User_Level_Fault;
        ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = irq;
#ਜੇ_ਅੰਤ
        handleUserLevelFault(irq, ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } ਨਹੀਂਤਾਂ ਜੇ (likely(irq < int_trap_min)) {
        ARCH_ਗੰਢ_STATE(x86_KScur_ਰੁਕਾਵਟ) = irq;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_ਰੁਕਾਵਟ;
        ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = irq;
#ਜੇ_ਅੰਤ
        handle_ਰੁਕਾਵਟ_ਇੰਦਰਾਜ();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } ਨਹੀਂਤਾਂ ਜੇ (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } ਨਹੀਂਤਾਂ {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        ਪੂਰਨ sys_ਅੰਕ;
        ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_ਅੰਕ = (irq << 24) | (syscall & 0x00ffffff);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_Unknown_Syscall;
        ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = sys_ਅੰਕ;
#ਜੇ_ਅੰਤ
        handleUnknownSyscall(sys_ਅੰਕ);
    }
    restore_user_context();
    UNREACHABLE();
}

ਖਾਲੀ NORETURN slowpath(syscall_t syscall)
{

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਜੇ (syscall == SysVMEnter && ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbVCPU);
        ਜੇ (ਗੰਢ_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(ਗੰਢ_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_ਸਰਗਰਮ) {
            completeSignal(ਗੰਢ_STATE(ksCurThread)->tcbBoundNotification, ਗੰਢ_STATE(ksCurThread));
            ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, SEL4_VMENTER_ਨਤੀਜਾ_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } ਨਹੀਂਤਾਂ {
            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#ਜੇ_ਅੰਤ
    /* check for undefined syscall */
    ਜੇ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#ਜੇ_ਅੰਤ /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 0;
#ਜੇ_ਅੰਤ /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ VISIBLE NORETURN c_handle_syscall(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info, syscall_t syscall, ਸ਼ਬਦ_t ਜਵਾਬ)
#ਨਹੀਂਤਾਂ
ਖਾਲੀ VISIBLE NORETURN c_handle_syscall(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info, syscall_t syscall)
#ਜੇ_ਅੰਤ
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    ਜੇ (config_ਸੈੱਟ(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbContext.registers[ਅਗਲਾ_IP] += 2;
    } ਨਹੀਂਤਾਂ {
        /* set FaultIP */
        ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), FaultIP, get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਅਗਲਾ_IP) - 2);
    }

    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ਸ਼ੁਰੂ(cptr, ਸੁਨੇਹਾ_Info, syscall);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 1;
#ਜੇ_ਅੰਤ /* TRACK_KERNEL_ENTRIES */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_FASTPATH
    ਜੇ (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, ਸੁਨੇਹਾ_Info);
        UNREACHABLE();
    } ਨਹੀਂਤਾਂ ਜੇ (syscall == (syscall_t)Sys_ਜਵਾਬ_Recv) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        fastpath_ਜਵਾਬ_recv(cptr, ਸੁਨੇਹਾ_Info, ਜਵਾਬ);
#ਨਹੀਂਤਾਂ
        fastpath_ਜਵਾਬ_recv(cptr, ਸੁਨੇਹਾ_Info);
#ਜੇ_ਅੰਤ
        UNREACHABLE();
    }
#ਜੇ_ਅੰਤ /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
ਖਾਲੀ VISIBLE NORETURN c_handle_vmexit(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_VMExit;
#ਜੇ_ਅੰਤ

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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    tcb_t *cur_thread = ਗੰਢ_STATE(ksCurThread);
    ARCH_ਗੰਢ_STATE(x86KSCurrentGSBase) = -(ਸ਼ਬਦ_t)1;
    ARCH_ਗੰਢ_STATE(x86KSCurrentFSBase) = -(ਸ਼ਬਦ_t)1;
    x86_ਲੱਦੋ_fsgs_ਅਧਾਰ(cur_thread, CURRENT_CPU_ਸੂਚਕਾਂਕ());
#ਜੇ_ਅੰਤ

    c_ਇੰਦਰਾਜ_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#ਜੇ_ਅੰਤ
