/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <arch/kernel/traps.h>
#समावेश <arch/object/vcpu.h>
#समावेश <arch/machine/registerset.h>
#समावेश <api/syscall.h>
#समावेश <machine/fpu.h>
#समावेश <mode/machine/debug.h>

#समावेश <sel4/benchmark_track_types.h>
#समावेश <benchmark/benchmark_track.h>
#समावेश <benchmark/benchmark_utilisation.h>
#समावेश <arch/machine.h>

व्योम VISIBLE NORETURN c_handle_undefined_instruction(व्योम)
{
    गाँठ_LOCK_SYS;
    c_entry_hook();

#अगर_घोषित TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = getRegister(गाँठ_STATE(ksCurThread), अग्र_IP);
#पूर्ण_अगर

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#अगर_घोषित CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#या_अगर CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
    अगर (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } अन्यथा
#पूर्ण_अगर
    {
        handleUserLevelFault(esr, 0);
    }
#पूर्ण_अगर
    restore_user_context();
    UNREACHABLE();
}

#अगर_घोषित CONFIG_EXCEPTION_FASTPATH
व्योम NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#पूर्ण_अगर

अटल अंतरभूत व्योम NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    गाँठ_LOCK_SYS;
    c_entry_hook();

#अगर_घोषित TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMFault;
    ksKernelEntry.word = getRegister(गाँठ_STATE(ksCurThread), अग्र_IP);
    ksKernelEntry.is_fastpath = false;
#पूर्ण_अगर

#अगर_घोषित CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# अन्यथा
    handleVMFaultEvent(type);
    restore_user_context();
#पूर्ण_अगर
    UNREACHABLE();
}

व्योम VISIBLE NORETURN c_handle_तथ्य_fault(व्योम)
{
    c_handle_vm_fault(se_L4_तथ्य_Fault);
}

व्योम VISIBLE NORETURN c_handle_instruction_fault(व्योम)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

व्योम VISIBLE NORETURN c_handle_interrupt(व्योम)
{
    गाँठ_LOCK_IRQ_IF(IRQT_TO_IRQ(getActiveIRQ()) != irq_remote_call_ipi);
    c_entry_hook();

#अगर_घोषित TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_सूचकांक();
#पूर्ण_अगर

    handleInterruptEntry();
    restore_user_context();
}

व्योम NORETURN slowpath(syscall_t syscall)
{
    अगर (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#पूर्ण_अगर /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } अन्यथा {
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#पूर्ण_अगर /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

व्योम VISIBLE c_handle_syscall(word_t cptr, word_t संदेश_Info, syscall_t syscall)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#अगर_घोषित TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#पूर्ण_अगर /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#अगर_घोषित CONFIG_FASTPATH
ALIGN(L1_CACHE_पंक्ति_आकार)
व्योम VISIBLE c_handle_fastpath_call(word_t cptr, word_t संदेश_Info)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#अगर_घोषित TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#पूर्ण_अगर /* DEBUG */

    fastpath_call(cptr, संदेश_Info);
    UNREACHABLE();
}

#अगर_घोषित CONFIG_KERNEL_MCS
#अगर_घोषित CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_पंक्ति_आकार)
व्योम VISIBLE c_handle_fastpath_signal(word_t cptr, word_t संदेश_Info)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#अगर_घोषित TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#पूर्ण_अगर /* DEBUG */
    fastpath_signal(cptr, संदेश_Info);
    UNREACHABLE();
}
#पूर्ण_अगर /* CONFIG_SIGNAL_FASTPATH */
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_पंक्ति_आकार)
#अगर_घोषित CONFIG_KERNEL_MCS
व्योम VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t संदेश_Info, word_t reply)
#अन्यथा
व्योम VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t संदेश_Info)
#पूर्ण_अगर
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#अगर_घोषित TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#पूर्ण_अगर /* DEBUG */

#अगर_घोषित CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, संदेश_Info, reply);
#अन्यथा
    fastpath_reply_recv(cptr, संदेश_Info);
#पूर्ण_अगर
    UNREACHABLE();
}

#पूर्ण_अगर

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN व्योम c_handle_vcpu_fault(word_t hsr)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();

#अगर_घोषित TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VCPUFault;
    ksKernelEntry.word = hsr;
#पूर्ण_अगर
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
