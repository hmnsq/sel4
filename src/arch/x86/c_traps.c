/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <model/statedata.h>
#شامل <machine/fpu.h>
#شامل <arch/fastpath/fastpath.h>
#شامل <arch/kernel/traps.h>
#شامل <machine/debug.h>
#شامل <arch/object/vcpu.h>
#شامل <api/syscall.h>
#شامل <sel4/arch/vmenter.h>

#شامل <benchmark/benchmark_track.h>
#شامل <benchmark/benchmark_utilisation.h>

خالی VISIBLE c_nested_مداخلت(صحیح irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    أكد(ARCH_گرہ_STATE(x86_KSPending_مداخلت) == int_invalid);
    ARCH_گرہ_STATE(x86_KSPending_مداخلت) = irq;
}

خالی VISIBLE NORETURN c_handle_مداخلت(صحیح irq, صحیح syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    اگر (config_سیٹ(CONFIG_ARCH_IA32) && config_سیٹ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    گرہ_تالا_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_اندراج_hook();

    اگر (irq == int_صفحہ_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.راستہ = اندراج_VMFault;
        ks_Kernel_اندراج.لفظ = type;
#اگر_ختم
        handleVMFaultEvent(type);
#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    } ورنہ اگر (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.راستہ = اندراج_Debug_Fault;
        ks_Kernel_اندراج.لفظ = گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#اگر_ختم
        handleUserLevelDebugException(irq);
#اگر_ختم /* CONFIG_HARDWARE_DEBUG_API */
    } ورنہ اگر (irq < int_irq_min) {
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.راستہ = اندراج_User_Level_Fault;
        ks_Kernel_اندراج.لفظ = irq;
#اگر_ختم
        handleUserLevelFault(irq, گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } ورنہ اگر (likely(irq < int_trap_min)) {
        ARCH_گرہ_STATE(x86_KScur_مداخلت) = irq;
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.راستہ = اندراج_مداخلت;
        ks_Kernel_اندراج.لفظ = irq;
#اگر_ختم
        handle_مداخلت_اندراج();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } ورنہ اگر (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } ورنہ {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        صحیح sys_عدد;
        گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_عدد = (irq << 24) | (syscall & 0x00ffffff);
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.راستہ = اندراج_Unknown_Syscall;
        ks_Kernel_اندراج.لفظ = sys_عدد;
#اگر_ختم
        handleUnknownSyscall(sys_عدد);
    }
    restore_user_context();
    UNREACHABLE();
}

خالی NORETURN slowpath(syscall_t syscall)
{

#اگر_تعریف_شدہ CONFIG_VTX
    اگر (syscall == SysVMEnter && گرہ_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(گرہ_STATE(ksCurThread)->tcbArch.tcbVCPU);
        اگر (گرہ_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(گرہ_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_فعال) {
            completeSignal(گرہ_STATE(ksCurThread)->tcbBoundNotification, گرہ_STATE(ksCurThread));
            سیٹ_رجسٹر(گرہ_STATE(ksCurThread), پیغام_Info_رجسٹر, SEL4_VMENTER_نتیجہ_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } ورنہ {
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#اگر_ختم
    /* check for undefined syscall */
    اگر (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.راستہ = اندراج_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#اگر_ختم /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ورنہ {
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.is_fastpath = 0;
#اگر_ختم /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی VISIBLE NORETURN c_handle_syscall(لفظ_t cptr, لفظ_t پیغام_Info, syscall_t syscall, لفظ_t جواب)
#ورنہ
خالی VISIBLE NORETURN c_handle_syscall(لفظ_t cptr, لفظ_t پیغام_Info, syscall_t syscall)
#اگر_ختم
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    اگر (config_سیٹ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    اگر (config_سیٹ(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers[اگلا_IP] += 2;
    } ورنہ {
        /* set FaultIP */
        سیٹ_رجسٹر(گرہ_STATE(ksCurThread), FaultIP, get_رجسٹر(گرہ_STATE(ksCurThread), اگلا_IP) - 2);
    }

    گرہ_تالا_SYS;

    c_اندراج_hook();

#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_آغاز(cptr, پیغام_Info, syscall);
    ks_Kernel_اندراج.is_fastpath = 1;
#اگر_ختم /* TRACK_KERNEL_ENTRIES */

#اگر_تعریف_شدہ CONFIG_FASTPATH
    اگر (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, پیغام_Info);
        UNREACHABLE();
    } ورنہ اگر (syscall == (syscall_t)Sys_جواب_Recv) {
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        fastpath_جواب_recv(cptr, پیغام_Info, جواب);
#ورنہ
        fastpath_جواب_recv(cptr, پیغام_Info);
#اگر_ختم
        UNREACHABLE();
    }
#اگر_ختم /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#اگر_تعریف_شدہ CONFIG_VTX
خالی VISIBLE NORETURN c_handle_vmexit(خالی)
{
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.راستہ = اندراج_VMExit;
#اگر_ختم

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
#اگر_غیر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    tcb_t *cur_thread = گرہ_STATE(ksCurThread);
    ARCH_گرہ_STATE(x86KSCurrentGSBase) = -(لفظ_t)1;
    ARCH_گرہ_STATE(x86KSCurrentFSBase) = -(لفظ_t)1;
    x86_بھرو_fsgs_بنیاد(cur_thread, CURRENT_CPU_اشاریہ());
#اگر_ختم

    c_اندراج_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#اگر_ختم
