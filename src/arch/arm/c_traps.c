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
    گرہ_جندرا_SYS;
    c_اندراج_hook();

#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.رستہ = اندراج_User_Level_Fault;
    ks_Kernel_اندراج.لفظ = get_رجسٹر(گرہ_STATE(ksCurThread), اگلا_IP);
#جے_انت

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#جے_تعریف_شدہ CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#وإلاإذا CONFIG_ARCH_AARCH64
    لفظ_t esr = getESR();
#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    إذا (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } وإلا
#جے_انت
    {
        handleUserLevelFault(esr, 0);
    }
#جے_انت
    restore_user_context();
    UNREACHABLE();
}

#جے_تعریف_شدہ CONFIG_EXCEPTION_FASTPATH
فراغ NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#جے_انت

ساكن مضمن فراغ NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    گرہ_جندرا_SYS;
    c_اندراج_hook();

#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.رستہ = اندراج_VMFault;
    ks_Kernel_اندراج.لفظ = get_رجسٹر(گرہ_STATE(ksCurThread), اگلا_IP);
    ks_Kernel_اندراج.is_fastpath = false;
#جے_انت

#جے_تعریف_شدہ CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# وإلا
    handleVMFaultEvent(type);
    restore_user_context();
#جے_انت
    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_انکڑے_fault(فراغ)
{
    c_handle_vm_fault(se_L4_انکڑے_Fault);
}

فراغ VISIBLE NORETURN c_handle_instruction_fault(فراغ)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

فراغ VISIBLE NORETURN c_handle_رکاوٹ(فراغ)
{
    گرہ_جندرا_IRQ_IF(IRQT_منزل_IRQ(get_سرگرم_IRQ()) != irq_remote_call_ipi);
    c_اندراج_hook();

#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.رستہ = اندراج_رکاوٹ;
    ks_Kernel_اندراج.لفظ = IRQT_منزل_IRQ(get_سرگرم_IRQ());
    ks_Kernel_اندراج.core = CURRENT_CPU_اشاریہ();
#جے_انت

    handle_رکاوٹ_اندراج();
    restore_user_context();
}

فراغ NORETURN slowpath(syscall_t syscall)
{
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.رستہ = اندراج_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#جے_انت /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.is_fastpath = 0;
#جے_انت /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

فراغ VISIBLE c_handle_syscall(لفظ_t cptr, لفظ_t پیغام_Info, syscall_t syscall)
{
    گرہ_جندرا_SYS;

    c_اندراج_hook();
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیغام_Info, syscall);
    ks_Kernel_اندراج.is_fastpath = 0;
#جے_انت /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#جے_تعریف_شدہ CONFIG_FASTPATH
ALIGN(L1_CACHE_سطر_ناپ)
فراغ VISIBLE c_handle_fastpath_call(لفظ_t cptr, لفظ_t پیغام_Info)
{
    گرہ_جندرا_SYS;

    c_اندراج_hook();
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیغام_Info, SysCall);
    ks_Kernel_اندراج.is_fastpath = 1;
#جے_انت /* DEBUG */

    fastpath_call(cptr, پیغام_Info);
    UNREACHABLE();
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#جے_تعریف_شدہ CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_سطر_ناپ)
فراغ VISIBLE c_handle_fastpath_signal(لفظ_t cptr, لفظ_t پیغام_Info)
{
    گرہ_جندرا_SYS;

    c_اندراج_hook();
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیغام_Info, SysCall);
    ks_Kernel_اندراج.is_fastpath = 1;
#جے_انت /* DEBUG */
    fastpath_signal(cptr, پیغام_Info);
    UNREACHABLE();
}
#جے_انت /* CONFIG_SIGNAL_FASTPATH */
#جے_انت /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_سطر_ناپ)
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info, لفظ_t جواب)
#وإلا
فراغ VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info)
#جے_انت
{
    گرہ_جندرا_SYS;

    c_اندراج_hook();
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیغام_Info, Sys_جواب_Recv);
    ks_Kernel_اندراج.is_fastpath = 1;
#جے_انت /* DEBUG */

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    fastpath_جواب_recv(cptr, پیغام_Info, جواب);
#وإلا
    fastpath_جواب_recv(cptr, پیغام_Info);
#جے_انت
    UNREACHABLE();
}

#جے_انت

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN فراغ c_handle_vcpu_fault(لفظ_t hsr)
{
    گرہ_جندرا_SYS;

    c_اندراج_hook();

#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.رستہ = اندراج_VCPUFault;
    ks_Kernel_اندراج.لفظ = hsr;
#جے_انت
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
