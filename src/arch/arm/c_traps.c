/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <arch/kernel/traps.h>
#அந்தர்பாவ <arch/object/vcpu.h>
#அந்தர்பாவ <arch/machine/registerset.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <machine/fpu.h>
#அந்தர்பாவ <mode/machine/debug.h>

#அந்தர்பாவ <sel4/benchmark_track_types.h>
#அந்தர்பாவ <benchmark/benchmark_track.h>
#அந்தர்பாவ <benchmark/benchmark_utilisation.h>
#அந்தர்பாவ <arch/machine.h>

வெறுமை VISIBLE NORETURN c_handle_undefined_instruction(வெறுமை)
{
    முனை_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = getRegister(முனை_STATE(ksCurThread), NextIP);
#endif

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ifdef CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#அல்லதெனில் CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#ifdef CONFIG_HARDWARE_DEBUG_API
    எனில் (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } இல்லையெனில்
#endif
    {
        handleUserLevelFault(esr, 0);
    }
#endif
    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_EXCEPTION_FASTPATH
வெறுமை NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#endif

நிலையான உள்வரி வெறுமை NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    முனை_LOCK_SYS;
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_VMFault;
    ksKernelEntry.word = getRegister(முனை_STATE(ksCurThread), NextIP);
    ksKernelEntry.is_fastpath = false;
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# இல்லையெனில்
    handleVMFaultEvent(type);
    restore_user_context();
#endif
    UNREACHABLE();
}

வெறுமை VISIBLE NORETURN c_handle_data_fault(வெறுமை)
{
    c_handle_vm_fault(seL4_DataFault);
}

வெறுமை VISIBLE NORETURN c_handle_instruction_fault(வெறுமை)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

வெறுமை VISIBLE NORETURN c_handle_interrupt(வெறுமை)
{
    முனை_LOCK_IRQ_IF(IRQT_TO_IRQ(getActiveIRQ()) != irq_remote_call_ipi);
    c_entry_hook();

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_Interrupt;
    ksKernelEntry.word = IRQT_TO_IRQ(getActiveIRQ());
    ksKernelEntry.core = CURRENT_CPU_INDEX();
#endif

    handleInterruptEntry();
    restore_user_context();
}

வெறுமை NORETURN slowpath(syscall_t syscall)
{
    எனில் (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } இல்லையெனில் {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

வெறுமை VISIBLE c_handle_syscall(word_t cptr, word_t செய்தி_Info, syscall_t syscall)
{
    முனை_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, செய்தி_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_வரி_SIZE)
வெறுமை VISIBLE c_handle_fastpath_call(word_t cptr, word_t செய்தி_Info)
{
    முனை_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, செய்தி_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, செய்தி_Info);
    UNREACHABLE();
}

#ifdef CONFIG_KERNEL_MCS
#ifdef CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_வரி_SIZE)
வெறுமை VISIBLE c_handle_fastpath_signal(word_t cptr, word_t செய்தி_Info)
{
    முனை_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, செய்தி_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
    fastpath_signal(cptr, செய்தி_Info);
    UNREACHABLE();
}
#endif /* CONFIG_SIGNAL_FASTPATH */
#endif /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_வரி_SIZE)
#ifdef CONFIG_KERNEL_MCS
வெறுமை VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t செய்தி_Info, word_t reply)
#இல்லையெனில்
வெறுமை VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t செய்தி_Info)
#endif
{
    முனை_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, செய்தி_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, செய்தி_Info, reply);
#இல்லையெனில்
    fastpath_reply_recv(cptr, செய்தி_Info);
#endif
    UNREACHABLE();
}

#endif

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN வெறுமை c_handle_vcpu_fault(word_t hsr)
{
    முனை_LOCK_SYS;

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
