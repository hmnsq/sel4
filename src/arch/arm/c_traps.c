/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <mode/machine/debug.h>

#ಅನ್ತರ್ಭಾವ <sel4/benchmark_track_types.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>

ಶೂನ್ಯ VISIBLE NORETURN c_handle_undefined_instruction(ಶೂನ್ಯ)
{
    ಗಂಟು_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = getRegister(ಗಂಟು_STATE(ksCurThread), NextIP);
#endif

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ifdef CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#ನೋಚೇದ್ಯದಿ CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#ifdef CONFIG_HARDWARE_DEBUG_API
    ಯದಿ (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } ಅನ್ಯಥಾ
#endif
    {
        handleUserLevelFault(esr, 0);
    }
#endif
    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_EXCEPTION_FASTPATH
ಶೂನ್ಯ NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#endif

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    ಗಂಟು_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMFault;
    ksKernelEntry.word = getRegister(ಗಂಟು_STATE(ksCurThread), NextIP);
    ksKernelEntry.is_fastpath = false;
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# ಅನ್ಯಥಾ
    handleVMFaultEvent(type);
    restore_user_context();
#endif
    UNREACHABLE();
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_data_fault(ಶೂನ್ಯ)
{
    c_handle_vm_fault(seL4_DataFault);
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_instruction_fault(ಶೂನ್ಯ)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_interrupt(ಶೂನ್ಯ)
{
    ಗಂಟು_LOCK_IRQ_IF(IRQT_TO_IRQ(getActiveIRQ()) != irq_remote_call_ipi);
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_INDEX();
#endif

    handleInterruptEntry();
    restore_user_context();
}

ಶೂನ್ಯ NORETURN slowpath(syscall_t syscall)
{
    ಯದಿ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ಅನ್ಯಥಾ {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

ಶೂನ್ಯ VISIBLE c_handle_syscall(word_t cptr, word_t ಸಂದೇಶ_Info, syscall_t syscall)
{
    ಗಂಟು_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ಸಂದೇಶ_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_ಸಾಲು_SIZE)
ಶೂನ್ಯ VISIBLE c_handle_fastpath_call(word_t cptr, word_t ಸಂದೇಶ_Info)
{
    ಗಂಟು_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ಸಂದೇಶ_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, ಸಂದೇಶ_Info);
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
#ifdef CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_ಸಾಲು_SIZE)
ಶೂನ್ಯ VISIBLE c_handle_fastpath_signal(word_t cptr, word_t ಸಂದೇಶ_Info)
{
    ಗಂಟು_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ಸಂದೇಶ_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
    fastpath_signal(cptr, ಸಂದೇಶ_Info);
    UNREACHABLE();
}
#endif /* CONFIG_SIGNAL_FASTPATH */
#endif /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_ಸಾಲು_SIZE)
#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t ಸಂದೇಶ_Info, word_t reply)
#ಅನ್ಯಥಾ
ಶೂನ್ಯ VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t ಸಂದೇಶ_Info)
#endif
{
    ಗಂಟು_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ಸಂದೇಶ_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, ಸಂದೇಶ_Info, reply);
#ಅನ್ಯಥಾ
    fastpath_reply_recv(cptr, ಸಂದೇಶ_Info);
#endif
    UNREACHABLE();
}

#endif

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN ಶೂನ್ಯ c_handle_vcpu_fault(word_t hsr)
{
    ಗಂಟು_LOCK_SYS;

    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VCPUFault;
    ksKernelEntry.word = hsr;
#endif
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
