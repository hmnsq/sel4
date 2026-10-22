/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <arch/fastpath/fastpath.h>
#అన్తర్భావ <arch/kernel/traps.h>
#అన్తర్భావ <machine/debug.h>
#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <sel4/arch/vmenter.h>

#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <benchmark/benchmark_utilisation.h>

శూన్య VISIBLE c_nested_అంతరాయం(పూర్ణాంకం irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    ప్రతిజ్ఞా(ARCH_కణుపు_STATE(x86_KSPending_అంతరాయం) == int_invalid);
    ARCH_కణుపు_STATE(x86_KSPending_అంతరాయం) = irq;
}

శూన్య VISIBLE NORETURN c_handle_అంతరాయం(పూర్ణాంకం irq, పూర్ణాంకం syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    ఒకవేళ (config_సమితి(CONFIG_ARCH_IA32) && config_సమితి(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    కణుపు_తాళం_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_నమోదు_hook();

    ఒకవేళ (irq == int_పుట_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (కణుపు_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.మార్గం = నమోదు_VMFault;
        ks_Kernel_నమోదు.మాట = type;
#అయితే_ముగింపు
        handleVMFaultEvent(type);
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    } లేకపోతే ఒకవేళ (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.మార్గం = నమోదు_Debug_Fault;
        ks_Kernel_నమోదు.మాట = కణుపు_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#అయితే_ముగింపు
        handleUserLevelDebugException(irq);
#అయితే_ముగింపు /* CONFIG_HARDWARE_DEBUG_API */
    } లేకపోతే ఒకవేళ (irq < int_irq_min) {
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.మార్గం = నమోదు_User_Level_Fault;
        ks_Kernel_నమోదు.మాట = irq;
#అయితే_ముగింపు
        handleUserLevelFault(irq, కణుపు_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } లేకపోతే ఒకవేళ (likely(irq < int_trap_min)) {
        ARCH_కణుపు_STATE(x86_KScur_అంతరాయం) = irq;
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.మార్గం = నమోదు_అంతరాయం;
        ks_Kernel_నమోదు.మాట = irq;
#అయితే_ముగింపు
        handle_అంతరాయం_నమోదు();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } లేకపోతే ఒకవేళ (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } లేకపోతే {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        పూర్ణాంకం sys_అంకె;
        కణుపు_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_అంకె = (irq << 24) | (syscall & 0x00ffffff);
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.మార్గం = నమోదు_Unknown_Syscall;
        ks_Kernel_నమోదు.మాట = sys_అంకె;
#అయితే_ముగింపు
        handleUnknownSyscall(sys_అంకె);
    }
    restore_user_context();
    UNREACHABLE();
}

శూన్య NORETURN slowpath(syscall_t syscall)
{

#నిర్వచితమైతే CONFIG_VTX
    ఒకవేళ (syscall == SysVMEnter && కణుపు_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(కణుపు_STATE(ksCurThread)->tcbArch.tcbVCPU);
        ఒకవేళ (కణుపు_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(కణుపు_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_చురుకైన) {
            completeSignal(కణుపు_STATE(ksCurThread)->tcbBoundNotification, కణుపు_STATE(ksCurThread));
            సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), సందేశం_Info_రిజిస్టరు, SEL4_VMENTER_ఫలితం_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } లేకపోతే {
            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#అయితే_ముగింపు
    /* check for undefined syscall */
    ఒకవేళ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.మార్గం = నమోదు_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#అయితే_ముగింపు /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } లేకపోతే {
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.is_fastpath = 0;
#అయితే_ముగింపు /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య VISIBLE NORETURN c_handle_syscall(మాట_t cptr, మాట_t సందేశం_Info, syscall_t syscall, మాట_t జవాబు)
#లేకపోతే
శూన్య VISIBLE NORETURN c_handle_syscall(మాట_t cptr, మాట_t సందేశం_Info, syscall_t syscall)
#అయితే_ముగింపు
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    ఒకవేళ (config_సమితి(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        కణుపు_STATE(ksCurThread)->tcbArch.tcbContext.registers[తదుపరి_IP] += 2;
    } లేకపోతే {
        /* set FaultIP */
        సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), FaultIP, get_రిజిస్టరు(కణుపు_STATE(ksCurThread), తదుపరి_IP) - 2);
    }

    కణుపు_తాళం_SYS;

    c_నమోదు_hook();

#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ప్రారంభం(cptr, సందేశం_Info, syscall);
    ks_Kernel_నమోదు.is_fastpath = 1;
#అయితే_ముగింపు /* TRACK_KERNEL_ENTRIES */

#నిర్వచితమైతే CONFIG_FASTPATH
    ఒకవేళ (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, సందేశం_Info);
        UNREACHABLE();
    } లేకపోతే ఒకవేళ (syscall == (syscall_t)Sys_జవాబు_Recv) {
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        fastpath_జవాబు_recv(cptr, సందేశం_Info, జవాబు);
#లేకపోతే
        fastpath_జవాబు_recv(cptr, సందేశం_Info);
#అయితే_ముగింపు
        UNREACHABLE();
    }
#అయితే_ముగింపు /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#నిర్వచితమైతే CONFIG_VTX
శూన్య VISIBLE NORETURN c_handle_vmexit(శూన్య)
{
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.మార్గం = నమోదు_VMExit;
#అయితే_ముగింపు

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
#నిర్వచితం_కాకపోతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    tcb_t *cur_thread = కణుపు_STATE(ksCurThread);
    ARCH_కణుపు_STATE(x86KSCurrentGSBase) = -(మాట_t)1;
    ARCH_కణుపు_STATE(x86KSCurrentFSBase) = -(మాట_t)1;
    x86_ఎక్కించు_fsgs_పునాది(cur_thread, CURRENT_CPU_సూచీ());
#అయితే_ముగింపు

    c_నమోదు_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#అయితే_ముగింపు
