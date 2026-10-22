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

فراغ VISIBLE c_nested_رکاوٹ(صحيح irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    أكد(ARCH_گرہ_STATE(x86_KSPending_رکاوٹ) == int_invalid);
    ARCH_گرہ_STATE(x86_KSPending_رکاوٹ) = irq;
}

فراغ VISIBLE NORETURN c_handle_رکاوٹ(صحيح irq, صحيح syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    إذا (config_سیٹ(CONFIG_ARCH_IA32) && config_سیٹ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    گرہ_جندرا_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_اندراج_hook();

    إذا (irq == int_ورقہ_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.رستہ = اندراج_VMFault;
        ks_Kernel_اندراج.لفظ = type;
#جے_انت
        handleVMFaultEvent(type);
#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    } وإلا إذا (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.رستہ = اندراج_Debug_Fault;
        ks_Kernel_اندراج.لفظ = گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#جے_انت
        handleUserLevelDebugException(irq);
#جے_انت /* CONFIG_HARDWARE_DEBUG_API */
    } وإلا إذا (irq < int_irq_min) {
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.رستہ = اندراج_User_Level_Fault;
        ks_Kernel_اندراج.لفظ = irq;
#جے_انت
        handleUserLevelFault(irq, گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } وإلا إذا (likely(irq < int_trap_min)) {
        ARCH_گرہ_STATE(x86_KScur_رکاوٹ) = irq;
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.رستہ = اندراج_رکاوٹ;
        ks_Kernel_اندراج.لفظ = irq;
#جے_انت
        handle_رکاوٹ_اندراج();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } وإلا إذا (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } وإلا {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        صحيح sys_عدد;
        گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_عدد = (irq << 24) | (syscall & 0x00ffffff);
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.رستہ = اندراج_Unknown_Syscall;
        ks_Kernel_اندراج.لفظ = sys_عدد;
#جے_انت
        handleUnknownSyscall(sys_عدد);
    }
    restore_user_context();
    UNREACHABLE();
}

فراغ NORETURN slowpath(syscall_t syscall)
{

#جے_تعریف_شدہ CONFIG_VTX
    إذا (syscall == SysVMEnter && گرہ_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(گرہ_STATE(ksCurThread)->tcbArch.tcbVCPU);
        إذا (گرہ_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(گرہ_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_سرگرم) {
            completeSignal(گرہ_STATE(ksCurThread)->tcbBoundNotification, گرہ_STATE(ksCurThread));
            سیٹ_رجسٹر(گرہ_STATE(ksCurThread), پیغام_Info_رجسٹر, SEL4_VMENTER_نتیجہ_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } وإلا {
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#جے_انت
    /* check for undefined syscall */
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.رستہ = اندراج_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#جے_انت /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.is_fastpath = 0;
#جے_انت /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ VISIBLE NORETURN c_handle_syscall(لفظ_t cptr, لفظ_t پیغام_Info, syscall_t syscall, لفظ_t جواب)
#وإلا
فراغ VISIBLE NORETURN c_handle_syscall(لفظ_t cptr, لفظ_t پیغام_Info, syscall_t syscall)
#جے_انت
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    إذا (config_سیٹ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    إذا (config_سیٹ(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[اگلا_IP] += 2;
    } وإلا {
        /* set FaultIP */
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), FaultIP, get_رجسٹر(گرہ_STATE(ksCurThread), اگلا_IP) - 2);
    }

    گرہ_جندرا_SYS;

    c_اندراج_hook();

#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیغام_Info, syscall);
    ks_Kernel_اندراج.is_fastpath = 1;
#جے_انت /* TRACK_KERNEL_ENTRIES */

#جے_تعریف_شدہ CONFIG_FASTPATH
    إذا (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, پیغام_Info);
        UNREACHABLE();
    } وإلا إذا (syscall == (syscall_t)Sys_جواب_Recv) {
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        fastpath_جواب_recv(cptr, پیغام_Info, جواب);
#وإلا
        fastpath_جواب_recv(cptr, پیغام_Info);
#جے_انت
        UNREACHABLE();
    }
#جے_انت /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#جے_تعریف_شدہ CONFIG_VTX
فراغ VISIBLE NORETURN c_handle_vmexit(فراغ)
{
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.رستہ = اندراج_VMExit;
#جے_انت

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
#جے_بنا_تعریف CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    tcb_t *cur_thread = گرہ_STATE(ksCurThread);
    ARCH_گرہ_STATE(x86KSCurrentGSBase) = -(لفظ_t)1;
    ARCH_گرہ_STATE(x86KSCurrentFSBase) = -(لفظ_t)1;
    x86_لدو_fsgs_ادھار(cur_thread, CURRENT_CPU_اشاریہ());
#جے_انت

    c_اندراج_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#جے_انت
