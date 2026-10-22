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

فراغ VISIBLE c_nested_مداخلت(صحيح irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    أكد(ARCH_ڳنڍ_STATE(x86_KSPending_مداخلت) == int_invalid);
    ARCH_ڳنڍ_STATE(x86_KSPending_مداخلت) = irq;
}

فراغ VISIBLE NORETURN c_handle_مداخلت(صحيح irq, صحيح syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    إذا (config_سيٽ(CONFIG_ARCH_IA32) && config_سيٽ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    ڳنڍ_تالو_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_داخلا_hook();

    إذا (irq == int_صفحو_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (ڳنڍ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.رستو = داخلا_VMFault;
        ks_Kernel_داخلا.لفظ_2 = type;
#جيڪڏهن_پورو
        handleVMFaultEvent(type);
#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
    } وإلا إذا (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.رستو = داخلا_Debug_Fault;
        ks_Kernel_داخلا.لفظ_2 = ڳنڍ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#جيڪڏهن_پورو
        handleUserLevelDebugException(irq);
#جيڪڏهن_پورو /* CONFIG_HARDWARE_DEBUG_API */
    } وإلا إذا (irq < int_irq_min) {
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.رستو = داخلا_User_Level_Fault;
        ks_Kernel_داخلا.لفظ_2 = irq;
#جيڪڏهن_پورو
        handleUserLevelFault(irq, ڳنڍ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } وإلا إذا (likely(irq < int_trap_min)) {
        ARCH_ڳنڍ_STATE(x86_KScur_مداخلت) = irq;
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.رستو = داخلا_مداخلت;
        ks_Kernel_داخلا.لفظ_2 = irq;
#جيڪڏهن_پورو
        handle_مداخلت_داخلا();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } وإلا إذا (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } وإلا {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        صحيح sys_انگ;
        ڳنڍ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_انگ = (irq << 24) | (syscall & 0x00ffffff);
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.رستو = داخلا_Unknown_Syscall;
        ks_Kernel_داخلا.لفظ_2 = sys_انگ;
#جيڪڏهن_پورو
        handleUnknownSyscall(sys_انگ);
    }
    restore_user_context();
    UNREACHABLE();
}

فراغ NORETURN slowpath(syscall_t syscall)
{

#جيڪڏهن_وصف_ٿيل CONFIG_VTX
    إذا (syscall == SysVMEnter && ڳنڍ_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(ڳنڍ_STATE(ksCurThread)->tcbArch.tcbVCPU);
        إذا (ڳنڍ_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(ڳنڍ_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_سرگرم) {
            completeSignal(ڳنڍ_STATE(ksCurThread)->tcbBoundNotification, ڳنڍ_STATE(ksCurThread));
            سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), نياپو_Info_رجسٽر, SEL4_VMENTER_نتيجو_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } وإلا {
            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#جيڪڏهن_پورو
    /* check for undefined syscall */
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.رستو = داخلا_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#جيڪڏهن_پورو /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.is_fastpath = 0;
#جيڪڏهن_پورو /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
فراغ VISIBLE NORETURN c_handle_syscall(لفظ_t cptr, لفظ_t نياپو_Info, syscall_t syscall, لفظ_t جواب)
#وإلا
فراغ VISIBLE NORETURN c_handle_syscall(لفظ_t cptr, لفظ_t نياپو_Info, syscall_t syscall)
#جيڪڏهن_پورو
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    إذا (config_سيٽ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    إذا (config_سيٽ(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        ڳنڍ_STATE(ksCurThread)->tcbArch.tcbContext.registers[ايندڙ_IP] += 2;
    } وإلا {
        /* set FaultIP */
        سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), FaultIP, get_رجسٽر(ڳنڍ_STATE(ksCurThread), ايندڙ_IP) - 2);
    }

    ڳنڍ_تالو_SYS;

    c_داخلا_hook();

#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروعات(cptr, نياپو_Info, syscall);
    ks_Kernel_داخلا.is_fastpath = 1;
#جيڪڏهن_پورو /* TRACK_KERNEL_ENTRIES */

#جيڪڏهن_وصف_ٿيل CONFIG_FASTPATH
    إذا (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, نياپو_Info);
        UNREACHABLE();
    } وإلا إذا (syscall == (syscall_t)Sys_جواب_Recv) {
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        fastpath_جواب_recv(cptr, نياپو_Info, جواب);
#وإلا
        fastpath_جواب_recv(cptr, نياپو_Info);
#جيڪڏهن_پورو
        UNREACHABLE();
    }
#جيڪڏهن_پورو /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#جيڪڏهن_وصف_ٿيل CONFIG_VTX
فراغ VISIBLE NORETURN c_handle_vmexit(فراغ)
{
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.رستو = داخلا_VMExit;
#جيڪڏهن_پورو

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
#جيڪڏهن_وصف_نه_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
    tcb_t *cur_thread = ڳنڍ_STATE(ksCurThread);
    ARCH_ڳنڍ_STATE(x86KSCurrentGSBase) = -(لفظ_t)1;
    ARCH_ڳنڍ_STATE(x86KSCurrentFSBase) = -(لفظ_t)1;
    x86_ڀريو_fsgs_بنياد(cur_thread, CURRENT_CPU_اشاريو());
#جيڪڏهن_پورو

    c_داخلا_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#جيڪڏهن_پورو
