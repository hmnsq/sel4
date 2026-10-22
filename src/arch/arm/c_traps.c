/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/traps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine/debug.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/benchmark_track_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_utilisation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>

ਖਾਲੀ VISIBLE NORETURN c_handle_undefined_instruction(ਖਾਲੀ)
{
    ਗੰਢ_ਤਾਲਾ_SYS;
    c_ਇੰਦਰਾਜ_hook();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_User_Level_Fault;
    ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਅਗਲਾ_IP);
#ਜੇ_ਅੰਤ

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#ਜਾਂਜੇ CONFIG_ARCH_AARCH64
    ਸ਼ਬਦ_t esr = getESR();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    ਜੇ (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
        handleUserLevelFault(esr, 0);
    }
#ਜੇ_ਅੰਤ
    restore_user_context();
    UNREACHABLE();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXCEPTION_FASTPATH
ਖਾਲੀ NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    ਗੰਢ_ਤਾਲਾ_SYS;
    c_ਇੰਦਰਾਜ_hook();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_VMFault;
    ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਅਗਲਾ_IP);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = false;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# ਨਹੀਂਤਾਂ
    handleVMFaultEvent(type);
    restore_user_context();
#ਜੇ_ਅੰਤ
    UNREACHABLE();
}

ਖਾਲੀ VISIBLE NORETURN c_handle_ਅੰਕੜੇ_fault(ਖਾਲੀ)
{
    c_handle_vm_fault(se_L4_ਅੰਕੜੇ_Fault);
}

ਖਾਲੀ VISIBLE NORETURN c_handle_instruction_fault(ਖਾਲੀ)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

ਖਾਲੀ VISIBLE NORETURN c_handle_ਰੁਕਾਵਟ(ਖਾਲੀ)
{
    ਗੰਢ_ਤਾਲਾ_IRQ_IF(IRQT_ਮੰਜ਼ਿਲ_IRQ(get_ਸਰਗਰਮ_IRQ()) != irq_remote_call_ipi);
    c_ਇੰਦਰਾਜ_hook();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_ਰੁਕਾਵਟ;
    ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = IRQT_ਮੰਜ਼ਿਲ_IRQ(get_ਸਰਗਰਮ_IRQ());
    ks_Kernel_ਇੰਦਰਾਜ.core = CURRENT_CPU_ਸੂਚਕਾਂਕ();
#ਜੇ_ਅੰਤ

    handle_ਰੁਕਾਵਟ_ਇੰਦਰਾਜ();
    restore_user_context();
}

ਖਾਲੀ NORETURN slowpath(syscall_t syscall)
{
    ਜੇ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#ਜੇ_ਅੰਤ /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 0;
#ਜੇ_ਅੰਤ /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

ਖਾਲੀ VISIBLE c_handle_syscall(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info, syscall_t syscall)
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ਸ਼ੁਰੂ(cptr, ਸੁਨੇਹਾ_Info, syscall);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 0;
#ਜੇ_ਅੰਤ /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_FASTPATH
ALIGN(L1_CACHE_ਸਤਰ_ਮਾਪ)
ਖਾਲੀ VISIBLE c_handle_fastpath_call(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info)
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ਸ਼ੁਰੂ(cptr, ਸੁਨੇਹਾ_Info, SysCall);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 1;
#ਜੇ_ਅੰਤ /* DEBUG */

    fastpath_call(cptr, ਸੁਨੇਹਾ_Info);
    UNREACHABLE();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_ਸਤਰ_ਮਾਪ)
ਖਾਲੀ VISIBLE c_handle_fastpath_signal(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info)
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ਸ਼ੁਰੂ(cptr, ਸੁਨੇਹਾ_Info, SysCall);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 1;
#ਜੇ_ਅੰਤ /* DEBUG */
    fastpath_signal(cptr, ਸੁਨੇਹਾ_Info);
    UNREACHABLE();
}
#ਜੇ_ਅੰਤ /* CONFIG_SIGNAL_FASTPATH */
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_ਸਤਰ_ਮਾਪ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ VISIBLE c_handle_fastpath_ਜਵਾਬ_recv(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info, ਸ਼ਬਦ_t ਜਵਾਬ)
#ਨਹੀਂਤਾਂ
ਖਾਲੀ VISIBLE c_handle_fastpath_ਜਵਾਬ_recv(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info)
#ਜੇ_ਅੰਤ
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ਸ਼ੁਰੂ(cptr, ਸੁਨੇਹਾ_Info, Sys_ਜਵਾਬ_Recv);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 1;
#ਜੇ_ਅੰਤ /* DEBUG */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    fastpath_ਜਵਾਬ_recv(cptr, ਸੁਨੇਹਾ_Info, ਜਵਾਬ);
#ਨਹੀਂਤਾਂ
    fastpath_ਜਵਾਬ_recv(cptr, ਸੁਨੇਹਾ_Info);
#ਜੇ_ਅੰਤ
    UNREACHABLE();
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN ਖਾਲੀ c_handle_vcpu_fault(ਸ਼ਬਦ_t hsr)
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_VCPUFault;
    ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = hsr;
#ਜੇ_ਅੰਤ
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
