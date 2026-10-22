/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/kernel/traps.h>
#ضمن <arch/object/vcpu.h>
#ضمن <arch/machine/registerset.h>
#ضمن <api/syscall.h>
#ضمن <machine/fpu.h>
#ضمن <mode/machine/debug.h>

#ضمن <sel4/benchmark_track_types.h>
#ضمن <benchmark/benchmark_track.h>
#ضمن <benchmark/benchmark_utilisation.h>
#ضمن <arch/machine.h>

فراغ VISIBLE NORETURN c_handle_undefined_instruction(فراغ)
{
    غوټه_قلف_SYS;
    c_ثبت_hook();

#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.لار = ثبت_User_Level_Fault;
    ks_Kernel_ثبت.کلمه = get_ثبتوونکی(غوټه_STATE(ksCurThread), بل_IP);
#د_که_پای

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#که_تعریف_شوی CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#وإلاإذا CONFIG_ARCH_AARCH64
    کلمه_t esr = getESR();
#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
    إذا (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } وإلا
#د_که_پای
    {
        handleUserLevelFault(esr, 0);
    }
#د_که_پای
    restore_user_context();
    UNREACHABLE();
}

#که_تعریف_شوی CONFIG_EXCEPTION_FASTPATH
فراغ NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#د_که_پای

ساكن مضمن فراغ NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    غوټه_قلف_SYS;
    c_ثبت_hook();

#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.لار = ثبت_VMFault;
    ks_Kernel_ثبت.کلمه = get_ثبتوونکی(غوټه_STATE(ksCurThread), بل_IP);
    ks_Kernel_ثبت.is_fastpath = false;
#د_که_پای

#که_تعریف_شوی CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# وإلا
    handleVMFaultEvent(type);
    restore_user_context();
#د_که_پای
    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_معلومات_fault(فراغ)
{
    c_handle_vm_fault(se_L4_معلومات_Fault);
}

فراغ VISIBLE NORETURN c_handle_instruction_fault(فراغ)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

فراغ VISIBLE NORETURN c_handle_مداخله(فراغ)
{
    غوټه_قلف_IRQ_IF(IRQT_منزل_IRQ(get_فعال_IRQ()) != irq_remote_call_ipi);
    c_ثبت_hook();

#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.لار = ثبت_مداخله;
    ks_Kernel_ثبت.کلمه = IRQT_منزل_IRQ(get_فعال_IRQ());
    ks_Kernel_ثبت.core = CURRENT_CPU_شاخص();
#د_که_پای

    handle_مداخله_ثبت();
    restore_user_context();
}

فراغ NORETURN slowpath(syscall_t syscall)
{
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.لار = ثبت_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#د_که_پای /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.is_fastpath = 0;
#د_که_پای /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

فراغ VISIBLE c_handle_syscall(کلمه_t cptr, کلمه_t پیغام_Info, syscall_t syscall)
{
    غوټه_قلف_SYS;

    c_ثبت_hook();
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_پیل(cptr, پیغام_Info, syscall);
    ks_Kernel_ثبت.is_fastpath = 0;
#د_که_پای /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#که_تعریف_شوی CONFIG_FASTPATH
ALIGN(L1_CACHE_کرښه_اندازه)
فراغ VISIBLE c_handle_fastpath_call(کلمه_t cptr, کلمه_t پیغام_Info)
{
    غوټه_قلف_SYS;

    c_ثبت_hook();
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_پیل(cptr, پیغام_Info, SysCall);
    ks_Kernel_ثبت.is_fastpath = 1;
#د_که_پای /* DEBUG */

    fastpath_call(cptr, پیغام_Info);
    UNREACHABLE();
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
#که_تعریف_شوی CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_کرښه_اندازه)
فراغ VISIBLE c_handle_fastpath_signal(کلمه_t cptr, کلمه_t پیغام_Info)
{
    غوټه_قلف_SYS;

    c_ثبت_hook();
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_پیل(cptr, پیغام_Info, SysCall);
    ks_Kernel_ثبت.is_fastpath = 1;
#د_که_پای /* DEBUG */
    fastpath_signal(cptr, پیغام_Info);
    UNREACHABLE();
}
#د_که_پای /* CONFIG_SIGNAL_FASTPATH */
#د_که_پای /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_کرښه_اندازه)
#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ VISIBLE c_handle_fastpath_ځواب_recv(کلمه_t cptr, کلمه_t پیغام_Info, کلمه_t ځواب)
#وإلا
فراغ VISIBLE c_handle_fastpath_ځواب_recv(کلمه_t cptr, کلمه_t پیغام_Info)
#د_که_پای
{
    غوټه_قلف_SYS;

    c_ثبت_hook();
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_پیل(cptr, پیغام_Info, Sys_ځواب_Recv);
    ks_Kernel_ثبت.is_fastpath = 1;
#د_که_پای /* DEBUG */

#که_تعریف_شوی CONFIG_KERNEL_MCS
    fastpath_ځواب_recv(cptr, پیغام_Info, ځواب);
#وإلا
    fastpath_ځواب_recv(cptr, پیغام_Info);
#د_که_پای
    UNREACHABLE();
}

#د_که_پای

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN فراغ c_handle_vcpu_fault(کلمه_t hsr)
{
    غوټه_قلف_SYS;

    c_ثبت_hook();

#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.لار = ثبت_VCPUFault;
    ks_Kernel_ثبت.کلمه = hsr;
#د_که_پای
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */
