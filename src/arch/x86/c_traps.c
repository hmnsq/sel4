/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/statedata.h>
#ضمن <machine/fpu.h>
#ضمن <arch/fastpath/fastpath.h>
#ضمن <arch/kernel/traps.h>
#ضمن <machine/debug.h>
#ضمن <arch/object/vcpu.h>
#ضمن <api/syscall.h>
#ضمن <sel4/arch/vmenter.h>

#ضمن <benchmark/benchmark_track.h>
#ضمن <benchmark/benchmark_utilisation.h>

فراغ VISIBLE c_nested_مداخله(صحيح irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    أكد(ARCH_غوټه_STATE(x86_KSPending_مداخله) == int_invalid);
    ARCH_غوټه_STATE(x86_KSPending_مداخله) = irq;
}

فراغ VISIBLE NORETURN c_handle_مداخله(صحيح irq, صحيح syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    إذا (config_سېټ(CONFIG_ARCH_IA32) && config_سېټ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    غوټه_قلف_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_ثبت_hook();

    إذا (irq == int_پاڼه_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (غوټه_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.لار = ثبت_VMFault;
        ks_Kernel_ثبت.کلمه = type;
#د_که_پای
        handleVMFaultEvent(type);
#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
    } وإلا إذا (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.لار = ثبت_Debug_Fault;
        ks_Kernel_ثبت.کلمه = غوټه_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#د_که_پای
        handleUserLevelDebugException(irq);
#د_که_پای /* CONFIG_HARDWARE_DEBUG_API */
    } وإلا إذا (irq < int_irq_min) {
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.لار = ثبت_User_Level_Fault;
        ks_Kernel_ثبت.کلمه = irq;
#د_که_پای
        handleUserLevelFault(irq, غوټه_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } وإلا إذا (likely(irq < int_trap_min)) {
        ARCH_غوټه_STATE(x86_KScur_مداخله) = irq;
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.لار = ثبت_مداخله;
        ks_Kernel_ثبت.کلمه = irq;
#د_که_پای
        handle_مداخله_ثبت();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } وإلا إذا (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } وإلا {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        صحيح sys_عدد;
        غوټه_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_عدد = (irq << 24) | (syscall & 0x00ffffff);
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.لار = ثبت_Unknown_Syscall;
        ks_Kernel_ثبت.کلمه = sys_عدد;
#د_که_پای
        handleUnknownSyscall(sys_عدد);
    }
    restore_user_context();
    UNREACHABLE();
}

فراغ NORETURN slowpath(syscall_t syscall)
{

#که_تعریف_شوی CONFIG_VTX
    إذا (syscall == SysVMEnter && غوټه_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(غوټه_STATE(ksCurThread)->tcbArch.tcbVCPU);
        إذا (غوټه_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(غوټه_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_فعال) {
            completeSignal(غوټه_STATE(ksCurThread)->tcbBoundNotification, غوټه_STATE(ksCurThread));
            سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), پیغام_Info_ثبتوونکی, SEL4_VMENTER_پایله_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } وإلا {
            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#د_که_پای
    /* check for undefined syscall */
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.لار = ثبت_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#د_که_پای /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.is_fastpath = 0;
#د_که_پای /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ VISIBLE NORETURN c_handle_syscall(کلمه_t cptr, کلمه_t پیغام_Info, syscall_t syscall, کلمه_t ځواب)
#وإلا
فراغ VISIBLE NORETURN c_handle_syscall(کلمه_t cptr, کلمه_t پیغام_Info, syscall_t syscall)
#د_که_پای
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    إذا (config_سېټ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    إذا (config_سېټ(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        غوټه_STATE(ksCurThread)->tcbArch.tcbContext.registers[بل_IP] += 2;
    } وإلا {
        /* set FaultIP */
        سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), FaultIP, get_ثبتوونکی(غوټه_STATE(ksCurThread), بل_IP) - 2);
    }

    غوټه_قلف_SYS;

    c_ثبت_hook();

#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_پیل(cptr, پیغام_Info, syscall);
    ks_Kernel_ثبت.is_fastpath = 1;
#د_که_پای /* TRACK_KERNEL_ENTRIES */

#که_تعریف_شوی CONFIG_FASTPATH
    إذا (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, پیغام_Info);
        UNREACHABLE();
    } وإلا إذا (syscall == (syscall_t)Sys_ځواب_Recv) {
#که_تعریف_شوی CONFIG_KERNEL_MCS
        fastpath_ځواب_recv(cptr, پیغام_Info, ځواب);
#وإلا
        fastpath_ځواب_recv(cptr, پیغام_Info);
#د_که_پای
        UNREACHABLE();
    }
#د_که_پای /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#که_تعریف_شوی CONFIG_VTX
فراغ VISIBLE NORETURN c_handle_vmexit(فراغ)
{
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.لار = ثبت_VMExit;
#د_که_پای

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
#که_نه_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
    tcb_t *cur_thread = غوټه_STATE(ksCurThread);
    ARCH_غوټه_STATE(x86KSCurrentGSBase) = -(کلمه_t)1;
    ARCH_غوټه_STATE(x86KSCurrentFSBase) = -(کلمه_t)1;
    x86_بارول_fsgs_بنسټ(cur_thread, CURRENT_CPU_شاخص());
#د_که_پای

    c_ثبت_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#د_که_پای
