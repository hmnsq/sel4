/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/traps.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/vcpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>
#ଅନ୍ତର୍ଭାଵ <mode/machine/debug.h>

#ଅନ୍ତର୍ଭାଵ <sel4/benchmark_track_types.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_track.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_utilisation.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>

ଶୂନ୍ୟ VISIBLE NORETURN c_handle_undefined_instruction(ଶୂନ୍ୟ)
{
    ଗଣ୍ଠି_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), NextIP);
#endif

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ifdef CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#ନଚେତଯଦି CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#ifdef CONFIG_HARDWARE_DEBUG_API
    ଯଦି (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } ଅନ୍ୟଥା
#endif
    {
        handleUserLevelFault(esr, 0);
    }
#endif
    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_EXCEPTION_FASTPATH
ଶୂନ୍ୟ NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#endif

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    ଗଣ୍ଠି_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMFault;
    ksKernelEntry.word = getRegister(ଗଣ୍ଠି_STATE(ksCurThread), NextIP);
    ksKernelEntry.is_fastpath = false;
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# ଅନ୍ୟଥା
    handleVMFaultEvent(type);
    restore_user_context();
#endif
    UNREACHABLE();
}

ଶୂନ୍ୟ VISIBLE NORETURN c_handle_data_fault(ଶୂନ୍ୟ)
{
    c_handle_vm_fault(seL4_DataFault);
}

ଶୂନ୍ୟ VISIBLE NORETURN c_handle_instruction_fault(ଶୂନ୍ୟ)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

ଶୂନ୍ୟ VISIBLE NORETURN c_handle_interrupt(ଶୂନ୍ୟ)
{
    ଗଣ୍ଠି_LOCK_IRQ_IF(IRQT_TO_IRQ(getActiveIRQ()) != irq_remote_call_ipi);
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_INDEX();
#endif

    handleInterruptEntry();
    restore_user_context();
}

ଶୂନ୍ୟ NORETURN slowpath(syscall_t syscall)
{
    ଯଦି (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ଅନ୍ୟଥା {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

ଶୂନ୍ୟ VISIBLE c_handle_syscall(word_t cptr, word_t ସନ୍ଦେଶ_Info, syscall_t syscall)
{
    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ସନ୍ଦେଶ_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_ଧାଡ଼ି_SIZE)
ଶୂନ୍ୟ VISIBLE c_handle_fastpath_call(word_t cptr, word_t ସନ୍ଦେଶ_Info)
{
    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ସନ୍ଦେଶ_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, ସନ୍ଦେଶ_Info);
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
#ifdef CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_ଧାଡ଼ି_SIZE)
ଶୂନ୍ୟ VISIBLE c_handle_fastpath_signal(word_t cptr, word_t ସନ୍ଦେଶ_Info)
{
    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ସନ୍ଦେଶ_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
    fastpath_signal(cptr, ସନ୍ଦେଶ_Info);
    UNREACHABLE();
}
#endif /* CONFIG_SIGNAL_FASTPATH */
#endif /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_ଧାଡ଼ି_SIZE)
#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t ସନ୍ଦେଶ_Info, word_t reply)
#ଅନ୍ୟଥା
ଶୂନ୍ୟ VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t ସନ୍ଦେଶ_Info)
#endif
{
    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ସନ୍ଦେଶ_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, ସନ୍ଦେଶ_Info, reply);
#ଅନ୍ୟଥା
    fastpath_reply_recv(cptr, ସନ୍ଦେଶ_Info);
#endif
    UNREACHABLE();
}

#endif

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN ଶୂନ୍ୟ c_handle_vcpu_fault(word_t hsr)
{
    ଗଣ୍ଠି_LOCK_SYS;

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
