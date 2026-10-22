/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/fastpath/fastpath.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <machine/debug.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <sel4/arch/vmenter.h>

#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>

ಶೂನ್ಯ VISIBLE c_nested_ಅಡಚಣೆ(ಪೂರ್ಣಾಂಕ irq)
{
    /* This is not a real entry point, so we do not grab locks or
     * run c_entry/exit_hooks, since this occurs only if we're already
     * running inside the kernel. Just record the irq and return */
    ಪ್ರತಿಜ್ಞಾ(ARCH_ಗಂಟು_STATE(x86_KSPending_ಅಡಚಣೆ) == int_invalid);
    ARCH_ಗಂಟು_STATE(x86_KSPending_ಅಡಚಣೆ) = irq;
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_ಅಡಚಣೆ(ಪೂರ್ಣಾಂಕ irq, ಪೂರ್ಣಾಂಕ syscall)
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet
     * on x64 this code ran already */
    ಆದರೆ (config_ಗಣ(CONFIG_ARCH_IA32) && config_ಗಣ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Only grab the lock if we are not handling 'int_remote_call_ipi' interrupt
     * also flag this lock as IRQ lock if handling the irq interrupts. */
    ಗಂಟು_ಬೀಗ_IF(irq != int_remote_call_ipi,
                 irq >= int_irq_min && irq <= int_irq_max);

    c_ನಮೂದು_hook();

    ಆದರೆ (irq == int_ಪುಟ_fault) {
        /* Error code is in Error. Pull out bit 5, which is whether it was instruction or data */
        vm_fault_type_t type = (ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error] >> 4u) & 1u;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_VMFault;
        ks_Kernel_ನಮೂದು.ಶಬ್ದ = type;
#ಆದರೆ_ಅಂತ್ಯ
        handleVMFaultEvent(type);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (irq == int_debug || irq == int_software_break_request) {
        /* Debug exception */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_Debug_Fault;
        ks_Kernel_ನಮೂದು.ಶಬ್ದ = ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP];
#ಆದರೆ_ಅಂತ್ಯ
        handleUserLevelDebugException(irq);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HARDWARE_DEBUG_API */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (irq < int_irq_min) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_User_Level_Fault;
        ks_Kernel_ನಮೂದು.ಶಬ್ದ = irq;
#ಆದರೆ_ಅಂತ್ಯ
        handleUserLevelFault(irq, ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[Error]);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (likely(irq < int_trap_min)) {
        ARCH_ಗಂಟು_STATE(x86_KScur_ಅಡಚಣೆ) = irq;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_ಅಡಚಣೆ;
        ks_Kernel_ನಮೂದು.ಶಬ್ದ = irq;
#ಆದರೆ_ಅಂತ್ಯ
        handle_ಅಡಚಣೆ_ನಮೂದು();
        /* check for other pending interrupts */
        receivePendingIRQ();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (irq == int_spurious) {
        /* fall through to restore_user_context and do nothing */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Interpret a trap as an unknown syscall */
        /* Adjust FaultIP to point to trapping INT
         * instruction by subtracting 2 */
        ಪೂರ್ಣಾಂಕ sys_ಅಂಕೆ;
        ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[FaultIP] -= 2;
        /* trap number is MSBs of the syscall number and the LSBS of EAX */
        sys_ಅಂಕೆ = (irq << 24) | (syscall & 0x00ffffff);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_Unknown_Syscall;
        ks_Kernel_ನಮೂದು.ಶಬ್ದ = sys_ಅಂಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
        handleUnknownSyscall(sys_ಅಂಕೆ);
    }
    restore_user_context();
    UNREACHABLE();
}

ಶೂನ್ಯ NORETURN slowpath(syscall_t syscall)
{

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಆದರೆ (syscall == SysVMEnter && ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbVCPU) {
        vcpu_update_state_sysvmenter(ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbVCPU);
        ಆದರೆ (ಗಂಟು_STATE(ksCurThread)->tcbBoundNotification
            && notification_ptr_get_state(ಗಂಟು_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_ಸಕ್ರಿಯ) {
            completeSignal(ಗಂಟು_STATE(ksCurThread)->tcbBoundNotification, ಗಂಟು_STATE(ksCurThread));
            ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, SEL4_VMENTER_ಫಲಿತಾಂಶ_NOTIF);
            /* Any guest state that we should return is in the same
             * register position as sent to us, so we can just return
             * and let the user pick up the values they put in */
            restore_user_context();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_RunningVM);
            restore_user_context();
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
    /* check for undefined syscall */
    ಆದರೆ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#ಆದರೆ_ಅಂತ್ಯ /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.is_fastpath = 0;
#ಆದರೆ_ಅಂತ್ಯ /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ VISIBLE NORETURN c_handle_syscall(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info, syscall_t syscall, ಶಬ್ದ_t ಉತ್ತರ)
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ VISIBLE NORETURN c_handle_syscall(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info, syscall_t syscall)
#ಆದರೆ_ಅಂತ್ಯ
{
    /* need to run this first as the NODE_LOCK code might end up as a function call
     * with a return, and we need to make sure returns are not exploitable yet */
    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }

    /* Must happen before NODE_LOCK_SYS so that lock exit via IPI stall restores
       the correct program counter. */
    ಆದರೆ (config_ಗಣ(CONFIG_SYSENTER)) {
        /* increment NextIP to skip sysenter */
        ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers[ಮುಂದಿನ_IP] += 2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* set FaultIP */
        ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), FaultIP, get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಮುಂದಿನ_IP) - 2);
    }

    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ಆರಂಭ(cptr, ಸಂದೇಶ_Info, syscall);
    ks_Kernel_ನಮೂದು.is_fastpath = 1;
#ಆದರೆ_ಅಂತ್ಯ /* TRACK_KERNEL_ENTRIES */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_FASTPATH
    ಆದರೆ (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, ಸಂದೇಶ_Info);
        UNREACHABLE();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (syscall == (syscall_t)Sys_ಉತ್ತರ_Recv) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        fastpath_ಉತ್ತರ_recv(cptr, ಸಂದೇಶ_Info, ಉತ್ತರ);
#ಇಲ್ಲದಿದ್ದರೆ
        fastpath_ಉತ್ತರ_recv(cptr, ಸಂದೇಶ_Info);
#ಆದರೆ_ಅಂತ್ಯ
        UNREACHABLE();
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
ಶೂನ್ಯ VISIBLE NORETURN c_handle_vmexit(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_VMExit;
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    tcb_t *cur_thread = ಗಂಟು_STATE(ksCurThread);
    ARCH_ಗಂಟು_STATE(x86KSCurrentGSBase) = -(ಶಬ್ದ_t)1;
    ARCH_ಗಂಟು_STATE(x86KSCurrentFSBase) = -(ಶಬ್ದ_t)1;
    x86_ತುಂಬು_fsgs_ತಳ(cur_thread, CURRENT_CPU_ಸೂಚ್ಯಂಕ());
#ಆದರೆ_ಅಂತ್ಯ

    c_ನಮೂದು_hook();
    /* NODE_LOCK will get called in handleVmexit */
    handleVmexit();
    restore_user_context();
    UNREACHABLE();
}
#ಆದರೆ_ಅಂತ್ಯ
