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
    ಗಂಟು_ಬೀಗ_SYS;
    c_ನಮೂದು_hook();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_User_Level_Fault;
    ks_Kernel_ನಮೂದು.ಶಬ್ದ = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಮುಂದಿನ_IP);
#ಆದರೆ_ಅಂತ್ಯ

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#ಅಥವಾಆದರೆ CONFIG_ARCH_AARCH64
    ಶಬ್ದ_t esr = getESR();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    ಆದರೆ (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
        handleUserLevelFault(esr, 0);
    }
#ಆದರೆ_ಅಂತ್ಯ
    restore_user_context();
    UNREACHABLE();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXCEPTION_FASTPATH
ಶೂನ್ಯ NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    ಗಂಟು_ಬೀಗ_SYS;
    c_ನಮೂದು_hook();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_VMFault;
    ks_Kernel_ನಮೂದು.ಶಬ್ದ = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಮುಂದಿನ_IP);
    ks_Kernel_ನಮೂದು.is_fastpath = false;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# ಇಲ್ಲದಿದ್ದರೆ
    handleVMFaultEvent(type);
    restore_user_context();
#ಆದರೆ_ಅಂತ್ಯ
    UNREACHABLE();
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_ದತ್ತಾಂಶ_fault(ಶೂನ್ಯ)
{
    c_handle_vm_fault(se_L4_ದತ್ತಾಂಶ_Fault);
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_instruction_fault(ಶೂನ್ಯ)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_ಅಡಚಣೆ(ಶೂನ್ಯ)
{
    ಗಂಟು_ಬೀಗ_IRQ_IF(IRQT_ಗಮ್ಯ_IRQ(get_ಸಕ್ರಿಯ_IRQ()) != irq_remote_call_ipi);
    c_ನಮೂದು_hook();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_ಅಡಚಣೆ;
    ks_Kernel_ನಮೂದು.ಶಬ್ದ = IRQT_ಗಮ್ಯ_IRQ(get_ಸಕ್ರಿಯ_IRQ());
    ks_Kernel_ನಮೂದು.core = CURRENT_CPU_ಸೂಚ್ಯಂಕ();
#ಆದರೆ_ಅಂತ್ಯ

    handle_ಅಡಚಣೆ_ನಮೂದು();
    restore_user_context();
}

ಶೂನ್ಯ NORETURN slowpath(syscall_t syscall)
{
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

ಶೂನ್ಯ VISIBLE c_handle_syscall(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info, syscall_t syscall)
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ಆರಂಭ(cptr, ಸಂದೇಶ_Info, syscall);
    ks_Kernel_ನಮೂದು.is_fastpath = 0;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_FASTPATH
ALIGN(L1_CACHE_ಸಾಲು_ಗಾತ್ರ)
ಶೂನ್ಯ VISIBLE c_handle_fastpath_call(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info)
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ಆರಂಭ(cptr, ಸಂದೇಶ_Info, SysCall);
    ks_Kernel_ನಮೂದು.is_fastpath = 1;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */

    fastpath_call(cptr, ಸಂದೇಶ_Info);
    UNREACHABLE();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_ಸಾಲು_ಗಾತ್ರ)
ಶೂನ್ಯ VISIBLE c_handle_fastpath_signal(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info)
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ಆರಂಭ(cptr, ಸಂದೇಶ_Info, SysCall);
    ks_Kernel_ನಮೂದು.is_fastpath = 1;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */
    fastpath_signal(cptr, ಸಂದೇಶ_Info);
    UNREACHABLE();
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_SIGNAL_FASTPATH */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_ಸಾಲು_ಗಾತ್ರ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ VISIBLE c_handle_fastpath_ಉತ್ತರ_recv(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info, ಶಬ್ದ_t ಉತ್ತರ)
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ VISIBLE c_handle_fastpath_ಉತ್ತರ_recv(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info)
#ಆದರೆ_ಅಂತ್ಯ
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ಆರಂಭ(cptr, ಸಂದೇಶ_Info, Sys_ಉತ್ತರ_Recv);
    ks_Kernel_ನಮೂದು.is_fastpath = 1;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    fastpath_ಉತ್ತರ_recv(cptr, ಸಂದೇಶ_Info, ಉತ್ತರ);
#ಇಲ್ಲದಿದ್ದರೆ
    fastpath_ಉತ್ತರ_recv(cptr, ಸಂದೇಶ_Info);
#ಆದರೆ_ಅಂತ್ಯ
    UNREACHABLE();
}

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN ಶೂನ್ಯ c_handle_vcpu_fault(ಶಬ್ದ_t hsr)
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_VCPUFault;
    ks_Kernel_ನಮೂದು.ಶಬ್ದ = hsr;
#ಆದರೆ_ಅಂತ್ಯ
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
