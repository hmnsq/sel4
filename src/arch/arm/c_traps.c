/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <arch/kernel/traps.h>
#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <arch/machine/registerset.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <mode/machine/debug.h>

#అన్తర్భావ <sel4/benchmark_track_types.h>
#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <benchmark/benchmark_utilisation.h>
#అన్తర్భావ <arch/machine.h>

శూన్య VISIBLE NORETURN c_handle_undefined_instruction(శూన్య)
{
    కణుపు_తాళం_SYS;
    c_నమోదు_hook();

#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.మార్గం = నమోదు_User_Level_Fault;
    ks_Kernel_నమోదు.మాట = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), తదుపరి_IP);
#అయితే_ముగింపు

    /* There's only one user-level fault on ARM, and the code is (0,0) */
#నిర్వచితమైతే CONFIG_ARCH_AARCH32
    handleUserLevelFault(0, 0);
#లేదావేళ CONFIG_ARCH_AARCH64
    మాట_t esr = getESR();
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    ఒకవేళ (isDebugFault(esr)) {
        handleDebugFaultEvent(esr);
    } లేకపోతే
#అయితే_ముగింపు
    {
        handleUserLevelFault(esr, 0);
    }
#అయితే_ముగింపు
    restore_user_context();
    UNREACHABLE();
}

#నిర్వచితమైతే CONFIG_EXCEPTION_FASTPATH
శూన్య NORETURN vm_fault_slowpath(vm_fault_type_t type)
{
    handleVMFaultEvent(type);
    restore_user_context();
    UNREACHABLE();
}
#అయితే_ముగింపు

స్థిర అంతఃవరుస శూన్య NORETURN c_handle_vm_fault(vm_fault_type_t type)
{
    కణుపు_తాళం_SYS;
    c_నమోదు_hook();

#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.మార్గం = నమోదు_VMFault;
    ks_Kernel_నమోదు.మాట = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), తదుపరి_IP);
    ks_Kernel_నమోదు.is_fastpath = false;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_EXCEPTION_FASTPATH
    fastpath_vm_fault(type);
# లేకపోతే
    handleVMFaultEvent(type);
    restore_user_context();
#అయితే_ముగింపు
    UNREACHABLE();
}

శూన్య VISIBLE NORETURN c_handle_దత్తాంశం_fault(శూన్య)
{
    c_handle_vm_fault(se_L4_దత్తాంశం_Fault);
}

శూన్య VISIBLE NORETURN c_handle_instruction_fault(శూన్య)
{
    c_handle_vm_fault(seL4_InstructionFault);
}

శూన్య VISIBLE NORETURN c_handle_అంతరాయం(శూన్య)
{
    కణుపు_తాళం_IRQ_IF(IRQT_గమ్యం_IRQ(get_చురుకైన_IRQ()) != irq_remote_call_ipi);
    c_నమోదు_hook();

#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.మార్గం = నమోదు_అంతరాయం;
    ks_Kernel_నమోదు.మాట = IRQT_గమ్యం_IRQ(get_చురుకైన_IRQ());
    ks_Kernel_నమోదు.core = CURRENT_CPU_సూచీ();
#అయితే_ముగింపు

    handle_అంతరాయం_నమోదు();
    restore_user_context();
}

శూన్య NORETURN slowpath(syscall_t syscall)
{
    ఒకవేళ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.మార్గం = నమోదు_Unknown_Syscall;
        /* ksKernelEntry.word word is already set to syscall */
#అయితే_ముగింపు /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } లేకపోతే {
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.is_fastpath = 0;
#అయితే_ముగింపు /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

శూన్య VISIBLE c_handle_syscall(మాట_t cptr, మాట_t సందేశం_Info, syscall_t syscall)
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ప్రారంభం(cptr, సందేశం_Info, syscall);
    ks_Kernel_నమోదు.is_fastpath = 0;
#అయితే_ముగింపు /* DEBUG */

    slowpath(syscall);
    UNREACHABLE();
}

#నిర్వచితమైతే CONFIG_FASTPATH
ALIGN(L1_CACHE_పంక్తి_కొలత)
శూన్య VISIBLE c_handle_fastpath_call(మాట_t cptr, మాట_t సందేశం_Info)
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ప్రారంభం(cptr, సందేశం_Info, SysCall);
    ks_Kernel_నమోదు.is_fastpath = 1;
#అయితే_ముగింపు /* DEBUG */

    fastpath_call(cptr, సందేశం_Info);
    UNREACHABLE();
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
#నిర్వచితమైతే CONFIG_SIGNAL_FASTPATH
ALIGN(L1_CACHE_పంక్తి_కొలత)
శూన్య VISIBLE c_handle_fastpath_signal(మాట_t cptr, మాట_t సందేశం_Info)
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ప్రారంభం(cptr, సందేశం_Info, SysCall);
    ks_Kernel_నమోదు.is_fastpath = 1;
#అయితే_ముగింపు /* DEBUG */
    fastpath_signal(cptr, సందేశం_Info);
    UNREACHABLE();
}
#అయితే_ముగింపు /* CONFIG_SIGNAL_FASTPATH */
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

ALIGN(L1_CACHE_పంక్తి_కొలత)
#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య VISIBLE c_handle_fastpath_జవాబు_recv(మాట_t cptr, మాట_t సందేశం_Info, మాట_t జవాబు)
#లేకపోతే
శూన్య VISIBLE c_handle_fastpath_జవాబు_recv(మాట_t cptr, మాట_t సందేశం_Info)
#అయితే_ముగింపు
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ప్రారంభం(cptr, సందేశం_Info, Sys_జవాబు_Recv);
    ks_Kernel_నమోదు.is_fastpath = 1;
#అయితే_ముగింపు /* DEBUG */

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    fastpath_జవాబు_recv(cptr, సందేశం_Info, జవాబు);
#లేకపోతే
    fastpath_జవాబు_recv(cptr, సందేశం_Info);
#అయితే_ముగింపు
    UNREACHABLE();
}

#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
VISIBLE NORETURN శూన్య c_handle_vcpu_fault(మాట_t hsr)
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();

#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.మార్గం = నమోదు_VCPUFault;
    ks_Kernel_నమోదు.మాట = hsr;
#అయితే_ముగింపు
    handleVCPUFault(hsr);
    restore_user_context();
    UNREACHABLE();
}
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
