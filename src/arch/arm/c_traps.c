/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <arch/kernel/traps.h>
#شامل <arch/object/vcpu.h>
#شامل <arch/machine/registerset.h>
#شامل <api/syscall.h>
#شامل <machine/fpu.h>
#شامل <mode/machine/debug.h>

#شامل <sel4/benchmark_track_types.h>
#شامل <benchmark/benchmark_track.h>
#شامل <benchmark/benchmark_utilisation.h>
#شامل <arch/machine.h>

تهی VISIBLE NORETURN c_handle_undefined_instruction(تهی)
{
    گره_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = getRegister(گره_STATE(ksCurThread), NextIP);
#endif

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ifdef CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#وگرنهاگر CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#ifdef CONFIG_HARDWARE_DEBUG_API
    اگر (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } وگرنه
#endif
    {
        handleUserLevelFault(esr, 0);
    }
#endif
    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_EXCEPTION_FASTPATH
تهی NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#endif

ایستا درونخطی تهی NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    گره_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMFault;
    ksKernelEntry.word = getRegister(گره_STATE(ksCurThread), NextIP);
    ksKernelEntry.is_fastpath = false;
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# وگرنه
    handleVMFaultEvent(type);
    restore_user_context();
#endif
    UNREACHABLE();
}

تهی VISIBLE NORETURN c_handle_data_fault(تهی)
{
    c_handle_vm_fault(seL4_DataFault);
}

تهی VISIBLE NORETURN c_handle_instruction_fault(تهی)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

تهی VISIBLE NORETURN c_handle_interrupt(تهی)
{
    گره_LOCK_IRQ_IF(IRQT_TO_IRQ(getActiveIRQ()) != irq_remote_call_ipi);
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_INDEX();
#endif

    handleInterruptEntry();
    restore_user_context();
}

تهی NORETURN slowpath(syscall_t syscall)
{
    اگر (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وگرنه {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

تهی VISIBLE c_handle_syscall(word_t cptr, word_t پیام_Info, syscall_t syscall)
{
    گره_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, پیام_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_سطر_SIZE)
تهی VISIBLE c_handle_fastpath_call(word_t cptr, word_t پیام_Info)
{
    گره_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, پیام_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, پیام_Info);
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
#ifdef CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_سطر_SIZE)
تهی VISIBLE c_handle_fastpath_signal(word_t cptr, word_t پیام_Info)
{
    گره_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, پیام_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
    fastpath_signal(cptr, پیام_Info);
    UNREACHABLE();
}
#endif /* CONFIG_SIGNAL_FASTPATH */
#endif /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_سطر_SIZE)
#ifdef CONFIG_KERNEL_MCS
تهی VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t پیام_Info, word_t reply)
#وگرنه
تهی VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t پیام_Info)
#endif
{
    گره_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, پیام_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, پیام_Info, reply);
#وگرنه
    fastpath_reply_recv(cptr, پیام_Info);
#endif
    UNREACHABLE();
}

#endif

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN تهی c_handle_vcpu_fault(word_t hsr)
{
    گره_LOCK_SYS;

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
