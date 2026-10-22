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
    ڳنڍ_تالو_SYS;
    c_داخلا_hook();

#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.رستو = داخلا_User_Level_Fault;
    ks_Kernel_داخلا.لفظ_2 = get_رجسٽر(ڳنڍ_STATE(ksCurThread), ايندڙ_IP);
#جيڪڏهن_پورو

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#وإلاإذا CONFIG_ARCH_AARCH64
    لفظ_t esr = getESR();
#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
    إذا (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } وإلا
#جيڪڏهن_پورو
    {
        handleUserLevelFault(esr, 0);
    }
#جيڪڏهن_پورو
    restore_user_context();
    UNREACHABLE();
}

#جيڪڏهن_وصف_ٿيل CONFIG_EXCEPTION_FASTPATH
فراغ NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#جيڪڏهن_پورو

ساكن مضمن فراغ NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    ڳنڍ_تالو_SYS;
    c_داخلا_hook();

#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.رستو = داخلا_VMFault;
    ks_Kernel_داخلا.لفظ_2 = get_رجسٽر(ڳنڍ_STATE(ksCurThread), ايندڙ_IP);
    ks_Kernel_داخلا.is_fastpath = false;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# وإلا
    handleVMFaultEvent(type);
    restore_user_context();
#جيڪڏهن_پورو
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

فراغ VISIBLE NORETURN c_handle_مداخلت(فراغ)
{
    ڳنڍ_تالو_IRQ_IF(IRQT_منزل_IRQ(get_سرگرم_IRQ()) != irq_remote_call_ipi);
    c_داخلا_hook();

#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.رستو = داخلا_مداخلت;
    ks_Kernel_داخلا.لفظ_2 = IRQT_منزل_IRQ(get_سرگرم_IRQ());
    ks_Kernel_داخلا.core = CURRENT_CPU_اشاريو();
#جيڪڏهن_پورو

    handle_مداخلت_داخلا();
    restore_user_context();
}

فراغ NORETURN slowpath(syscall_t syscall)
{
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.رستو = داخلا_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#جيڪڏهن_پورو /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.is_fastpath = 0;
#جيڪڏهن_پورو /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

فراغ VISIBLE c_handle_syscall(لفظ_t cptr, لفظ_t نياپو_Info, syscall_t syscall)
{
    ڳنڍ_تالو_SYS;

    c_داخلا_hook();
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروعات(cptr, نياپو_Info, syscall);
    ks_Kernel_داخلا.is_fastpath = 0;
#جيڪڏهن_پورو /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#جيڪڏهن_وصف_ٿيل CONFIG_FASTPATH
ALIGN(L1_CACHE_سٽ_سائيز)
فراغ VISIBLE c_handle_fastpath_call(لفظ_t cptr, لفظ_t نياپو_Info)
{
    ڳنڍ_تالو_SYS;

    c_داخلا_hook();
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروعات(cptr, نياپو_Info, SysCall);
    ks_Kernel_داخلا.is_fastpath = 1;
#جيڪڏهن_پورو /* DEBUG */

    fastpath_call(cptr, نياپو_Info);
    UNREACHABLE();
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#جيڪڏهن_وصف_ٿيل CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_سٽ_سائيز)
فراغ VISIBLE c_handle_fastpath_signal(لفظ_t cptr, لفظ_t نياپو_Info)
{
    ڳنڍ_تالو_SYS;

    c_داخلا_hook();
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروعات(cptr, نياپو_Info, SysCall);
    ks_Kernel_داخلا.is_fastpath = 1;
#جيڪڏهن_پورو /* DEBUG */
    fastpath_signal(cptr, نياپو_Info);
    UNREACHABLE();
}
#جيڪڏهن_پورو /* CONFIG_SIGNAL_FASTPATH */
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_سٽ_سائيز)
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
فراغ VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t نياپو_Info, لفظ_t جواب)
#وإلا
فراغ VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t نياپو_Info)
#جيڪڏهن_پورو
{
    ڳنڍ_تالو_SYS;

    c_داخلا_hook();
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروعات(cptr, نياپو_Info, Sys_جواب_Recv);
    ks_Kernel_داخلا.is_fastpath = 1;
#جيڪڏهن_پورو /* DEBUG */

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    fastpath_جواب_recv(cptr, نياپو_Info, جواب);
#وإلا
    fastpath_جواب_recv(cptr, نياپو_Info);
#جيڪڏهن_پورو
    UNREACHABLE();
}

#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN فراغ c_handle_vcpu_fault(لفظ_t hsr)
{
    ڳنڍ_تالو_SYS;

    c_داخلا_hook();

#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.رستو = داخلا_VCPUFault;
    ks_Kernel_داخلا.لفظ_2 = hsr;
#جيڪڏهن_پورو
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
