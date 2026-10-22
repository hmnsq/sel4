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

शून्य VISIBLE c_nested_व्यवधानम्(पूर्णाङ्क irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    प्रतिज्ञा(ARCH_ग्रन्थिः_STATE(x86_KSPending_व्यवधानम्) == int_invalid);
    ARCH_ग्रन्थिः_STATE(x86_KSPending_व्यवधानम्) = irq;
}

शून्य VISIBLE NORETURN c_handle_व्यवधानम्(पूर्णाङ्क irq, पूर्णाङ्क syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    यदि (config_समुच्चयः(CONFIG_ARCH_IA32) && config_समुच्चयः(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    ग्रन्थिः_तालकम्_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_प्रविष्टिः_hook();

    यदि (irq == int_page_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_VMFault;
        ks_Kernel_प्रविष्टिः.word = type;
#यदि_अन्तः
        handleVMFaultEvent(type);
#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    } अन्यथा यदि (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_Debug_Fault;
        ks_Kernel_प्रविष्टिः.word = ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#यदि_अन्तः
        handleUserLevelDebugException(irq);
#यदि_अन्तः /* CONFIG_HARDWARE_DEBUG_API */
    } अन्यथा यदि (irq < int_irq_min) {
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_User_Level_Fault;
        ks_Kernel_प्रविष्टिः.word = irq;
#यदि_अन्तः
        handleUserLevelFault(irq, ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } अन्यथा यदि (likely(irq < int_trap_min)) {
        ARCH_ग्रन्थिः_STATE(x86_KScur_व्यवधानम्) = irq;
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_व्यवधानम्;
        ks_Kernel_प्रविष्टिः.word = irq;
#यदि_अन्तः
        handle_व्यवधानम्_प्रविष्टिः();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } अन्यथा यदि (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } अन्यथा {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        पूर्णाङ्क sys_अङ्कः;
        ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_अङ्कः = (irq << 24) | (syscall & 0x00ffffff);
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_Unknown_Syscall;
        ks_Kernel_प्रविष्टिः.word = sys_अङ्कः;
#यदि_अन्तः
        handleUnknownSyscall(sys_अङ्कः);
    }
    restore_user_context();
    UNREACHABLE();
}

शून्य NORETURN slowpath(syscall_t syscall)
{

#यदि_परिभाषितम् CONFIG_VTX
    यदि (syscall == SysVMEnter && ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbVCPU);
        यदि (ग्रन्थिः_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(ग्रन्थिः_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_सक्रिय) {
            completeSignal(ग्रन्थिः_STATE(ksCurThread)->tcbBoundNotification, ग्रन्थिः_STATE(ksCurThread));
            समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), सन्देशः_Info_पञ्जिकाकोशः, SEL4_VMENTER_परिणाम_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } अन्यथा {
            समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#यदि_अन्तः
    /* check for undefined syscall */
    यदि (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#यदि_अन्तः /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } अन्यथा {
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.is_fastpath = 0;
#यदि_अन्तः /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t सन्देशः_Info, syscall_t syscall, word_t प्रत्युत्तरम्)
#अन्यथा
शून्य VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t सन्देशः_Info, syscall_t syscall)
#यदि_अन्तः
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    यदि (config_समुच्चयः(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers[अग्र_IP] += 2;
    } अन्यथा {
        /* set FaultIP */
        समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), FaultIP, get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), अग्र_IP) - 2);
    }

    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();

#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, syscall);
    ks_Kernel_प्रविष्टिः.is_fastpath = 1;
#यदि_अन्तः /* TRACK_KERNEL_ENTRIES */

#यदि_परिभाषितम् CONFIG_FASTPATH
    यदि (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, सन्देशः_Info);
        UNREACHABLE();
    } अन्यथा यदि (syscall == (syscall_t)Sys_प्रत्युत्तरम्_Recv) {
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        fastpath_प्रत्युत्तरम्_recv(cptr, सन्देशः_Info, प्रत्युत्तरम्);
#अन्यथा
        fastpath_प्रत्युत्तरम्_recv(cptr, सन्देशः_Info);
#यदि_अन्तः
        UNREACHABLE();
    }
#यदि_अन्तः /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#यदि_परिभाषितम् CONFIG_VTX
शून्य VISIBLE NORETURN c_handle_vmexit(शून्य)
{
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_VMExit;
#यदि_अन्तः

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
#यदि_अपरिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
    tcb_t *cur_thread = ग्रन्थिः_STATE(ksCurThread);
    ARCH_ग्रन्थिः_STATE(x86KSCurrentGSBase) = -(word_t)1;
    ARCH_ग्रन्थिः_STATE(x86KSCurrentFSBase) = -(word_t)1;
    x86_आरोपय_fsgs_आधारः(cur_thread, CURRENT_CPU_सूचकांक());
#यदि_अन्तः

    c_प्रविष्टिः_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#यदि_अन्तः
