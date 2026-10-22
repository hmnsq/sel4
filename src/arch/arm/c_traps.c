/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <arch/kernel/traps.h>
#অন্তর্ভাব <arch/object/vcpu.h>
#অন্তর্ভাব <arch/machine/registerset.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <machine/fpu.h>
#অন্তর্ভাব <mode/machine/debug.h>

#অন্তর্ভাব <sel4/benchmark_track_types.h>
#অন্তর্ভাব <benchmark/benchmark_track.h>
#অন্তর্ভাব <benchmark/benchmark_utilisation.h>
#অন্তর্ভাব <arch/machine.h>

শূন্য VISIBLE NORETURN c_handle_undefined_instruction(শূন্য)
{
    গ্রন্থি_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = getRegister(গ্রন্থি_STATE(ksCurThread), পরবর্তী_IP);
#endif

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ifdef CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#নতুবা_যদি CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#ifdef CONFIG_HARDWARE_DEBUG_API
    যদি (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } অন্যথা
#endif
    {
        handleUserLevelFault(esr, 0);
    }
#endif
    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_EXCEPTION_FASTPATH
শূন্য NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#endif

স্থির অন্তঃরেখ শূন্য NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    গ্রন্থি_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMFault;
    ksKernelEntry.word = getRegister(গ্রন্থি_STATE(ksCurThread), পরবর্তী_IP);
    ksKernelEntry.is_fastpath = false;
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# অন্যথা
    handleVMFaultEvent(type);
    restore_user_context();
#endif
    UNREACHABLE();
}

শূন্য VISIBLE NORETURN c_handle_তথ্য_fault(শূন্য)
{
    c_handle_vm_fault(se_L4_তথ্য_Fault);
}

শূন্য VISIBLE NORETURN c_handle_instruction_fault(শূন্য)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

শূন্য VISIBLE NORETURN c_handle_interrupt(শূন্য)
{
    গ্রন্থি_LOCK_IRQ_IF(IRQT_TO_IRQ(getActiveIRQ()) != irq_remote_call_ipi);
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_সূচক();
#endif

    handleInterruptEntry();
    restore_user_context();
}

শূন্য NORETURN slowpath(syscall_t syscall)
{
    যদি (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } অন্যথা {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

শূন্য VISIBLE c_handle_syscall(word_t cptr, word_t বার্তা_Info, syscall_t syscall)
{
    গ্রন্থি_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, বার্তা_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_সারি_আকার)
শূন্য VISIBLE c_handle_fastpath_call(word_t cptr, word_t বার্তা_Info)
{
    গ্রন্থি_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, বার্তা_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, বার্তা_Info);
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
#ifdef CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_সারি_আকার)
শূন্য VISIBLE c_handle_fastpath_signal(word_t cptr, word_t বার্তা_Info)
{
    গ্রন্থি_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, বার্তা_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
    fastpath_signal(cptr, বার্তা_Info);
    UNREACHABLE();
}
#endif /* CONFIG_SIGNAL_FASTPATH */
#endif /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_সারি_আকার)
#ifdef CONFIG_KERNEL_MCS
শূন্য VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t বার্তা_Info, word_t reply)
#অন্যথা
শূন্য VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t বার্তা_Info)
#endif
{
    গ্রন্থি_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, বার্তা_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, বার্তা_Info, reply);
#অন্যথা
    fastpath_reply_recv(cptr, বার্তা_Info);
#endif
    UNREACHABLE();
}

#endif

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN শূন্য c_handle_vcpu_fault(word_t hsr)
{
    গ্রন্থি_LOCK_SYS;

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
