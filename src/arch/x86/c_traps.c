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

فراغ VISIBLE c_nested_وقفه(صحيح irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    أكد(ARCH_گره_STATE(x86_KSPending_وقفه) == int_invalid);
    ARCH_گره_STATE(x86_KSPending_وقفه) = irq;
}

فراغ VISIBLE NORETURN c_handle_وقفه(صحيح irq, صحيح syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    إذا (config_مجموعه(CONFIG_ARCH_IA32) && config_مجموعه(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    گره_قفل_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_مدخل_hook();

    إذا (irq == int_صفحه_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (گره_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.مسیر = مدخل_VMFault;
        ks_Kernel_مدخل.کلمه = type;
#پایان_اگر
        handleVMFaultEvent(type);
#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    } وإلا إذا (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.مسیر = مدخل_Debug_Fault;
        ks_Kernel_مدخل.کلمه = گره_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#پایان_اگر
        handleUserLevelDebugException(irq);
#پایان_اگر /* CONFIG_HARDWARE_DEBUG_API */
    } وإلا إذا (irq < int_irq_min) {
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.مسیر = مدخل_User_Level_Fault;
        ks_Kernel_مدخل.کلمه = irq;
#پایان_اگر
        handleUserLevelFault(irq, گره_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } وإلا إذا (likely(irq < int_trap_min)) {
        ARCH_گره_STATE(x86_KScur_وقفه) = irq;
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.مسیر = مدخل_وقفه;
        ks_Kernel_مدخل.کلمه = irq;
#پایان_اگر
        handle_وقفه_مدخل();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } وإلا إذا (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } وإلا {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        صحيح sys_رقم;
        گره_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_رقم = (irq << 24) | (syscall & 0x00ffffff);
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.مسیر = مدخل_Unknown_Syscall;
        ks_Kernel_مدخل.کلمه = sys_رقم;
#پایان_اگر
        handleUnknownSyscall(sys_رقم);
    }
    restore_user_context();
    UNREACHABLE();
}

فراغ NORETURN slowpath(syscall_t syscall)
{

#اگر_تعریف_شده CONFIG_VTX
    إذا (syscall == SysVMEnter && گره_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(گره_STATE(ksCurThread)->tcbArch.tcbVCPU);
        إذا (گره_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(گره_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_فعال) {
            completeSignal(گره_STATE(ksCurThread)->tcbBoundNotification, گره_STATE(ksCurThread));
            مجموعه_ثبات(گره_STATE(ksCurThread), پیام_Info_ثبات, SEL4_VMENTER_نتیجه_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } وإلا {
            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#پایان_اگر
    /* check for undefined syscall */
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.مسیر = مدخل_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#پایان_اگر /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.is_fastpath = 0;
#پایان_اگر /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ VISIBLE NORETURN c_handle_syscall(کلمه_t cptr, کلمه_t پیام_Info, syscall_t syscall, کلمه_t پاسخ)
#وإلا
فراغ VISIBLE NORETURN c_handle_syscall(کلمه_t cptr, کلمه_t پیام_Info, syscall_t syscall)
#پایان_اگر
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    إذا (config_مجموعه(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    إذا (config_مجموعه(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        گره_STATE(ksCurThread)->tcbArch.tcbContext.registers[بعدی_IP] += 2;
    } وإلا {
        /* set FaultIP */
        مجموعه_ثبات(گره_STATE(ksCurThread), FaultIP, get_ثبات(گره_STATE(ksCurThread), بعدی_IP) - 2);
    }

    گره_قفل_SYS;

    c_مدخل_hook();

#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیام_Info, syscall);
    ks_Kernel_مدخل.is_fastpath = 1;
#پایان_اگر /* TRACK_KERNEL_ENTRIES */

#اگر_تعریف_شده CONFIG_FASTPATH
    إذا (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, پیام_Info);
        UNREACHABLE();
    } وإلا إذا (syscall == (syscall_t)Sys_پاسخ_Recv) {
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        fastpath_پاسخ_recv(cptr, پیام_Info, پاسخ);
#وإلا
        fastpath_پاسخ_recv(cptr, پیام_Info);
#پایان_اگر
        UNREACHABLE();
    }
#پایان_اگر /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#اگر_تعریف_شده CONFIG_VTX
فراغ VISIBLE NORETURN c_handle_vmexit(فراغ)
{
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    ks_Kernel_مدخل.مسیر = مدخل_VMExit;
#پایان_اگر

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
#اگر_تعریف_نشده CONFIG_X86_64_VTX_64_بیت_GUESTS
    tcb_t *cur_thread = گره_STATE(ksCurThread);
    ARCH_گره_STATE(x86KSCurrentGSBase) = -(کلمه_t)1;
    ARCH_گره_STATE(x86KSCurrentFSBase) = -(کلمه_t)1;
    x86_بارگیری_fsgs_پایه(cur_thread, CURRENT_CPU_شاخص());
#پایان_اگر

    c_مدخل_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#پایان_اگر
