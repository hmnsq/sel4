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

خالی VISIBLE NORETURN c_handle_undefined_instruction(خالی)
{
    گرہ_تالا_SYS;
    c_اندراج_hook();

#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.راستہ = اندراج_User_Level_Fault;
    ks_Kernel_اندراج.لفظ = get_رجسٹر(گرہ_STATE(ksCurThread), اگلا_IP);
#اگر_ختم

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#اگر_تعریف_شدہ CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#وگرنہ_اگر CONFIG_ARCH_AARCH64
    لفظ_t esr = getESR();
#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    اگر (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } ورنہ
#اگر_ختم
    {
        handleUserLevelFault(esr, 0);
    }
#اگر_ختم
    restore_user_context();
    UNREACHABLE();
}

#اگر_تعریف_شدہ CONFIG_EXCEPTION_FASTPATH
خالی NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#اگر_ختم

ساکن اندرونی خالی NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    گرہ_تالا_SYS;
    c_اندراج_hook();

#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.راستہ = اندراج_VMFault;
    ks_Kernel_اندراج.لفظ = get_رجسٹر(گرہ_STATE(ksCurThread), اگلا_IP);
    ks_Kernel_اندراج.is_fastpath = false;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# ورنہ
    handleVMFaultEvent(type);
    restore_user_context();
#اگر_ختم
    UNREACHABLE();
}

خالی VISIBLE NORETURN c_handle_ڈیٹا_fault(خالی)
{
    c_handle_vm_fault(se_L4_ڈیٹا_Fault);
}

خالی VISIBLE NORETURN c_handle_instruction_fault(خالی)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

خالی VISIBLE NORETURN c_handle_مداخلت(خالی)
{
    گرہ_تالا_IRQ_IF(IRQT_مقصد_IRQ(get_فعال_IRQ()) != irq_remote_call_ipi);
    c_اندراج_hook();

#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.راستہ = اندراج_مداخلت;
    ks_Kernel_اندراج.لفظ = IRQT_مقصد_IRQ(get_فعال_IRQ());
    ks_Kernel_اندراج.core = CURRENT_CPU_اشاریہ();
#اگر_ختم

    handle_مداخلت_اندراج();
    restore_user_context();
}

خالی NORETURN slowpath(syscall_t syscall)
{
    اگر (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.راستہ = اندراج_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#اگر_ختم /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ورنہ {
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.is_fastpath = 0;
#اگر_ختم /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

خالی VISIBLE c_handle_syscall(لفظ_t cptr, لفظ_t پیغام_Info, syscall_t syscall)
{
    گرہ_تالا_SYS;

    c_اندراج_hook();
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_آغاز(cptr, پیغام_Info, syscall);
    ks_Kernel_اندراج.is_fastpath = 0;
#اگر_ختم /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#اگر_تعریف_شدہ CONFIG_FASTPATH
ALIGN(L1_CACHE_سطر_سائز)
خالی VISIBLE c_handle_fastpath_call(لفظ_t cptr, لفظ_t پیغام_Info)
{
    گرہ_تالا_SYS;

    c_اندراج_hook();
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_آغاز(cptr, پیغام_Info, SysCall);
    ks_Kernel_اندراج.is_fastpath = 1;
#اگر_ختم /* DEBUG */

    fastpath_call(cptr, پیغام_Info);
    UNREACHABLE();
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#اگر_تعریف_شدہ CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_سطر_سائز)
خالی VISIBLE c_handle_fastpath_signal(لفظ_t cptr, لفظ_t پیغام_Info)
{
    گرہ_تالا_SYS;

    c_اندراج_hook();
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_آغاز(cptr, پیغام_Info, SysCall);
    ks_Kernel_اندراج.is_fastpath = 1;
#اگر_ختم /* DEBUG */
    fastpath_signal(cptr, پیغام_Info);
    UNREACHABLE();
}
#اگر_ختم /* CONFIG_SIGNAL_FASTPATH */
#اگر_ختم /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_سطر_سائز)
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info, لفظ_t جواب)
#ورنہ
خالی VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info)
#اگر_ختم
{
    گرہ_تالا_SYS;

    c_اندراج_hook();
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_آغاز(cptr, پیغام_Info, Sys_جواب_Recv);
    ks_Kernel_اندراج.is_fastpath = 1;
#اگر_ختم /* DEBUG */

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    fastpath_جواب_recv(cptr, پیغام_Info, جواب);
#ورنہ
    fastpath_جواب_recv(cptr, پیغام_Info);
#اگر_ختم
    UNREACHABLE();
}

#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN خالی c_handle_vcpu_fault(لفظ_t hsr)
{
    گرہ_تالا_SYS;

    c_اندراج_hook();

#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.راستہ = اندراج_VCPUFault;
    ks_Kernel_اندراج.لفظ = hsr;
#اگر_ختم
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
