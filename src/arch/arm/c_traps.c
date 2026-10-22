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
    ग्रन्थिः_तालकम्_SYS;
    c_प्रविष्टिः_hook();

#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_User_Level_Fault;
    ks_Kernel_प्रविष्टिः.word = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), अग्र_IP);
#यदि_अन्तः

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#यदि_परिभाषितम् CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#नोचेद्यदि CONFIG_ARCH_AARCH64
    word_t esr = getESR();
#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    यदि (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } अन्यथा
#यदि_अन्तः
    {
        handleUserLevelFault(esr, 0);
    }
#यदि_अन्तः
    restore_user_context();
    UNREACHABLE();
}

#यदि_परिभाषितम् CONFIG_EXCEPTION_FASTPATH
शून्य NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#यदि_अन्तः

स्थिर अन्तःस्थ शून्य NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    ग्रन्थिः_तालकम्_SYS;
    c_प्रविष्टिः_hook();

#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_VMFault;
    ks_Kernel_प्रविष्टिः.word = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), अग्र_IP);
    ks_Kernel_प्रविष्टिः.is_fastpath = false;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# अन्यथा
    handleVMFaultEvent(type);
    restore_user_context();
#यदि_अन्तः
    UNREACHABLE();
}

शून्य VISIBLE NORETURN c_handle_दत्त_fault(शून्य)
{
    c_handle_vm_fault(se_L4_दत्त_Fault);
}

शून्य VISIBLE NORETURN c_handle_instruction_fault(शून्य)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

शून्य VISIBLE NORETURN c_handle_व्यवधानम्(शून्य)
{
    ग्रन्थिः_तालकम्_IRQ_IF(IRQT_गन्तव्यम्_IRQ(get_सक्रिय_IRQ()) != irq_remote_call_ipi);
    c_प्रविष्टिः_hook();

#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_व्यवधानम्;
    ks_Kernel_प्रविष्टिः.word = IRQT_गन्तव्यम्_IRQ(get_सक्रिय_IRQ());
    ks_Kernel_प्रविष्टिः.core = CURRENT_CPU_सूचकांक();
#यदि_अन्तः

    handle_व्यवधानम्_प्रविष्टिः();
    restore_user_context();
}

शून्य NORETURN slowpath(syscall_t syscall)
{
    यदि (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#यदि_अन्तः /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } अन्यथा {
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.is_fastpath = 0;
#यदि_अन्तः /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

शून्य VISIBLE c_handle_syscall(word_t cptr, word_t सन्देशः_Info, syscall_t syscall)
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, syscall);
    ks_Kernel_प्रविष्टिः.is_fastpath = 0;
#यदि_अन्तः /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#यदि_परिभाषितम् CONFIG_FASTPATH
ALIGN(L1_CACHE_पङ्क्तिः_आकार)
शून्य VISIBLE c_handle_fastpath_call(word_t cptr, word_t सन्देशः_Info)
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, SysCall);
    ks_Kernel_प्रविष्टिः.is_fastpath = 1;
#यदि_अन्तः /* DEBUG */

    fastpath_call(cptr, सन्देशः_Info);
    UNREACHABLE();
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#यदि_परिभाषितम् CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_पङ्क्तिः_आकार)
शून्य VISIBLE c_handle_fastpath_signal(word_t cptr, word_t सन्देशः_Info)
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, SysCall);
    ks_Kernel_प्रविष्टिः.is_fastpath = 1;
#यदि_अन्तः /* DEBUG */
    fastpath_signal(cptr, सन्देशः_Info);
    UNREACHABLE();
}
#यदि_अन्तः /* CONFIG_SIGNAL_FASTPATH */
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_पङ्क्तिः_आकार)
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य VISIBLE c_handle_fastpath_प्रत्युत्तरम्_recv(word_t cptr, word_t सन्देशः_Info, word_t प्रत्युत्तरम्)
#अन्यथा
शून्य VISIBLE c_handle_fastpath_प्रत्युत्तरम्_recv(word_t cptr, word_t सन्देशः_Info)
#यदि_अन्तः
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, Sys_प्रत्युत्तरम्_Recv);
    ks_Kernel_प्रविष्टिः.is_fastpath = 1;
#यदि_अन्तः /* DEBUG */

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    fastpath_प्रत्युत्तरम्_recv(cptr, सन्देशः_Info, प्रत्युत्तरम्);
#अन्यथा
    fastpath_प्रत्युत्तरम्_recv(cptr, सन्देशः_Info);
#यदि_अन्तः
    UNREACHABLE();
}

#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN शून्य c_handle_vcpu_fault(word_t hsr)
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();

#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_VCPUFault;
    ks_Kernel_प्रविष्टिः.word = hsr;
#यदि_अन्तः
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
