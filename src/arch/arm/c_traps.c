/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <arch/kernel/traps.h>
#അന്തര്ഭാവ <arch/object/vcpu.h>
#അന്തര്ഭാവ <arch/machine/registerset.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <machine/fpu.h>
#അന്തര്ഭാവ <mode/machine/debug.h>

#അന്തര്ഭാവ <sel4/benchmark_track_types.h>
#അന്തര്ഭാവ <benchmark/benchmark_track.h>
#അന്തര്ഭാവ <benchmark/benchmark_utilisation.h>
#അന്തര്ഭാവ <arch/machine.h>

ശൂന്യം VISIBLE NORETURN c_handle_undefined_instruction(ശൂന്യം)
{
    കണ്ണി_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = getRegister(കണ്ണി_STATE(ksCurThread), NextIP);
#endif

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ifdef CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#അല്ലെങ്കിലെങ്കിൽ CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#ifdef CONFIG_HARDWARE_DEBUG_API
    എങ്കിൽ (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } അല്ലെങ്കിൽ
#endif
    {
        handleUserLevelFault(esr, 0);
    }
#endif
    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_EXCEPTION_FASTPATH
ശൂന്യം NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#endif

സ്ഥിതം വരിയിൽ ശൂന്യം NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    കണ്ണി_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMFault;
    ksKernelEntry.word = getRegister(കണ്ണി_STATE(ksCurThread), NextIP);
    ksKernelEntry.is_fastpath = false;
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# അല്ലെങ്കിൽ
    handleVMFaultEvent(type);
    restore_user_context();
#endif
    UNREACHABLE();
}

ശൂന്യം VISIBLE NORETURN c_handle_data_fault(ശൂന്യം)
{
    c_handle_vm_fault(seL4_DataFault);
}

ശൂന്യം VISIBLE NORETURN c_handle_instruction_fault(ശൂന്യം)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

ശൂന്യം VISIBLE NORETURN c_handle_interrupt(ശൂന്യം)
{
    കണ്ണി_LOCK_IRQ_IF(IRQT_TO_IRQ(getActiveIRQ()) != irq_remote_call_ipi);
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_INDEX();
#endif

    handleInterruptEntry();
    restore_user_context();
}

ശൂന്യം NORETURN slowpath(syscall_t syscall)
{
    എങ്കിൽ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } അല്ലെങ്കിൽ {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

ശൂന്യം VISIBLE c_handle_syscall(word_t cptr, word_t സന്ദേശം_Info, syscall_t syscall)
{
    കണ്ണി_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, സന്ദേശം_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_വരി_SIZE)
ശൂന്യം VISIBLE c_handle_fastpath_call(word_t cptr, word_t സന്ദേശം_Info)
{
    കണ്ണി_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, സന്ദേശം_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, സന്ദേശം_Info);
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
#ifdef CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_വരി_SIZE)
ശൂന്യം VISIBLE c_handle_fastpath_signal(word_t cptr, word_t സന്ദേശം_Info)
{
    കണ്ണി_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, സന്ദേശം_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
    fastpath_signal(cptr, സന്ദേശം_Info);
    UNREACHABLE();
}
#endif /* CONFIG_SIGNAL_FASTPATH */
#endif /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_വരി_SIZE)
#ifdef CONFIG_KERNEL_MCS
ശൂന്യം VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t സന്ദേശം_Info, word_t reply)
#അല്ലെങ്കിൽ
ശൂന്യം VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t സന്ദേശം_Info)
#endif
{
    കണ്ണി_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, സന്ദേശം_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, സന്ദേശം_Info, reply);
#അല്ലെങ്കിൽ
    fastpath_reply_recv(cptr, സന്ദേശം_Info);
#endif
    UNREACHABLE();
}

#endif

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN ശൂന്യം c_handle_vcpu_fault(word_t hsr)
{
    കണ്ണി_LOCK_SYS;

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
