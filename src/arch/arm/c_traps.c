/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/kernel/traps.h>
#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <arch/machine/registerset.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <mode/machine/debug.h>

#अन्तर्भाव <sel4/benchmark_track_types.h>
#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>
#अन्तर्भाव <arch/machine.h>

शून्य VISIBLE NORETURN c_handle_undefined_instruction(शून्य)
{
    गाँठ_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = getRegister(गाँठ_STATE(ksCurThread), NextIP);
#endif

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ifdef CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#नोचेद्यदि CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#ifdef CONFIG_HARDWARE_DEBUG_API
    यदि (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } अन्यथा
#endif
    {
        handleUserLevelFault(esr, 0);
    }
#endif
    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_EXCEPTION_FASTPATH
शून्य NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#endif

स्थिर अन्तःस्थ शून्य NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    गाँठ_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMFault;
    ksKernelEntry.word = getRegister(गाँठ_STATE(ksCurThread), NextIP);
    ksKernelEntry.is_fastpath = false;
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# अन्यथा
    handleVMFaultEvent(type);
    restore_user_context();
#endif
    UNREACHABLE();
}

शून्य VISIBLE NORETURN c_handle_data_fault(शून्य)
{
    c_handle_vm_fault(seL4_DataFault);
}

शून्य VISIBLE NORETURN c_handle_instruction_fault(शून्य)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

शून्य VISIBLE NORETURN c_handle_interrupt(शून्य)
{
    गाँठ_LOCK_IRQ_IF(IRQT_TO_IRQ(getActiveIRQ()) != irq_remote_call_ipi);
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_INDEX();
#endif

    handleInterruptEntry();
    restore_user_context();
}

शून्य NORETURN slowpath(syscall_t syscall)
{
    यदि (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } अन्यथा {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

शून्य VISIBLE c_handle_syscall(word_t cptr, word_t संदेश_Info, syscall_t syscall)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_पंक्ति_SIZE)
शून्य VISIBLE c_handle_fastpath_call(word_t cptr, word_t संदेश_Info)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, संदेश_Info);
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
#ifdef CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_पंक्ति_SIZE)
शून्य VISIBLE c_handle_fastpath_signal(word_t cptr, word_t संदेश_Info)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
    fastpath_signal(cptr, संदेश_Info);
    UNREACHABLE();
}
#endif /* CONFIG_SIGNAL_FASTPATH */
#endif /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_पंक्ति_SIZE)
#ifdef CONFIG_KERNEL_MCS
शून्य VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t संदेश_Info, word_t reply)
#अन्यथा
शून्य VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t संदेश_Info)
#endif
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, संदेश_Info, reply);
#अन्यथा
    fastpath_reply_recv(cptr, संदेश_Info);
#endif
    UNREACHABLE();
}

#endif

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN शून्य c_handle_vcpu_fault(word_t hsr)
{
    गाँठ_LOCK_SYS;

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
