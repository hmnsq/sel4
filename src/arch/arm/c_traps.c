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
    گره_قفل_SYS;
    c_مدخل_hook();

#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    ks_Kernel_مدخل.مسیر = مدخل_User_Level_Fault;
    ks_Kernel_مدخل.کلمه = get_ثبات(گره_STATE(ksCurThread), بعدی_IP);
#پایان_اگر

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#اگر_تعریف_شده CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#وإلاإذا CONFIG_ARCH_AARCH64
    کلمه_t esr = getESR();
#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    إذا (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } وإلا
#پایان_اگر
    {
        handleUserLevelFault(esr, 0);
    }
#پایان_اگر
    restore_user_context();
    UNREACHABLE();
}

#اگر_تعریف_شده CONFIG_EXCEPTION_FASTPATH
فراغ NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#پایان_اگر

ساكن مضمن فراغ NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    گره_قفل_SYS;
    c_مدخل_hook();

#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    ks_Kernel_مدخل.مسیر = مدخل_VMFault;
    ks_Kernel_مدخل.کلمه = get_ثبات(گره_STATE(ksCurThread), بعدی_IP);
    ks_Kernel_مدخل.is_fastpath = false;
#پایان_اگر

#اگر_تعریف_شده CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# وإلا
    handleVMFaultEvent(type);
    restore_user_context();
#پایان_اگر
    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_داده_fault(فراغ)
{
    c_handle_vm_fault(se_L4_داده_Fault);
}

فراغ VISIBLE NORETURN c_handle_instruction_fault(فراغ)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

فراغ VISIBLE NORETURN c_handle_وقفه(فراغ)
{
    گره_قفل_IRQ_IF(IRQT_مقصد_IRQ(get_فعال_IRQ()) != irq_remote_call_ipi);
    c_مدخل_hook();

#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    ks_Kernel_مدخل.مسیر = مدخل_وقفه;
    ks_Kernel_مدخل.کلمه = IRQT_مقصد_IRQ(get_فعال_IRQ());
    ks_Kernel_مدخل.core = CURRENT_CPU_شاخص();
#پایان_اگر

    handle_وقفه_مدخل();
    restore_user_context();
}

فراغ NORETURN slowpath(syscall_t syscall)
{
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.مسیر = مدخل_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#پایان_اگر /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.is_fastpath = 0;
#پایان_اگر /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

فراغ VISIBLE c_handle_syscall(کلمه_t cptr, کلمه_t پیام_Info, syscall_t syscall)
{
    گره_قفل_SYS;

    c_مدخل_hook();
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیام_Info, syscall);
    ks_Kernel_مدخل.is_fastpath = 0;
#پایان_اگر /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#اگر_تعریف_شده CONFIG_FASTPATH
ALIGN(L1_CACHE_سطر_اندازه)
فراغ VISIBLE c_handle_fastpath_call(کلمه_t cptr, کلمه_t پیام_Info)
{
    گره_قفل_SYS;

    c_مدخل_hook();
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیام_Info, SysCall);
    ks_Kernel_مدخل.is_fastpath = 1;
#پایان_اگر /* DEBUG */

    fastpath_call(cptr, پیام_Info);
    UNREACHABLE();
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
#اگر_تعریف_شده CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_سطر_اندازه)
فراغ VISIBLE c_handle_fastpath_signal(کلمه_t cptr, کلمه_t پیام_Info)
{
    گره_قفل_SYS;

    c_مدخل_hook();
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیام_Info, SysCall);
    ks_Kernel_مدخل.is_fastpath = 1;
#پایان_اگر /* DEBUG */
    fastpath_signal(cptr, پیام_Info);
    UNREACHABLE();
}
#پایان_اگر /* CONFIG_SIGNAL_FASTPATH */
#پایان_اگر /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_سطر_اندازه)
#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ VISIBLE c_handle_fastpath_پاسخ_recv(کلمه_t cptr, کلمه_t پیام_Info, کلمه_t پاسخ)
#وإلا
فراغ VISIBLE c_handle_fastpath_پاسخ_recv(کلمه_t cptr, کلمه_t پیام_Info)
#پایان_اگر
{
    گره_قفل_SYS;

    c_مدخل_hook();
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیام_Info, Sys_پاسخ_Recv);
    ks_Kernel_مدخل.is_fastpath = 1;
#پایان_اگر /* DEBUG */

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    fastpath_پاسخ_recv(cptr, پیام_Info, پاسخ);
#وإلا
    fastpath_پاسخ_recv(cptr, پیام_Info);
#پایان_اگر
    UNREACHABLE();
}

#پایان_اگر

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN فراغ c_handle_vcpu_fault(کلمه_t hsr)
{
    گره_قفل_SYS;

    c_مدخل_hook();

#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    ks_Kernel_مدخل.مسیر = مدخل_VCPUFault;
    ks_Kernel_مدخل.کلمه = hsr;
#پایان_اگر
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
