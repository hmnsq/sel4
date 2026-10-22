/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <kernel/stack.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <arch/fastpath/fastpath.h>
#అన్తర్భావ <arch/machine/debug.h>
#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <mode/stack.h>
#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <arch/kernel/traps.h>

#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <util.h>

#నిర్వచితమైతే CONFIG_VTX
వాడిన స్థిర శూన్య NORETURN vmlaunch_failed(శూన్య)
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();

    handle_Vm_నమోదు_Fail();
    restore_user_context();
}

స్థిర శూన్య NORETURN restore_vmx(tcb_t *cur_thread, vcpu_t *vcpu)
{
    restoreVMCS();
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    /* Do not support breakpoints in VMs, so just disable all breakpoints */
    ఎక్కించు_All_Disabled_Breakpoint_State(cur_thread);
#అయితే_ముగింపు
    మాట_t *guest_regs_ptr = vcpu->gp_registers;
    ఒకవేళ (vcpu->launched) {
        /* attempt to do a vmresume */
        asm అస్థిర(
            // Set our stack pointer to the top of the tcb so we can efficiently pop
            "movl %[guest_regs], %%esp\n"
            "popl %%eax\n"
            "popl %%ebx\n"
            "popl %%ecx\n"
            "popl %%edx\n"
            "popl %%esi\n"
            "popl %%edi\n"
            "popl %%ebp\n"
            // Now do the vmresume
            "vmresume\n"
            // if we get here we failed
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#లేకపోతే
            "leal kernel_stack_alloc + %c1, %%esp\n"
#అయితే_ముగింపు
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు) - పరిమాణం(మాట_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    } లేకపోతే {
        /* attempt to do a vmlaunch */
        asm అస్థిర(
            // Set our stack pointer to the top of the tcb so we can efficiently pop
            "movl %[guest_regs], %%esp\n"
            "popl %%eax\n"
            "popl %%ebx\n"
            "popl %%ecx\n"
            "popl %%edx\n"
            "popl %%esi\n"
            "popl %%edi\n"
            "popl %%ebp\n"
            // Now do the vmresume
            "vmlaunch\n"
            // if we get here we failed
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#లేకపోతే
            "leal kernel_stack_alloc + %c1, %%esp\n"
#అయితే_ముగింపు
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు) - పరిమాణం(మాట_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    }
    UNREACHABLE();
}
#అయితే_ముగింపు

శూన్య NORETURN VISIBLE restore_user_context(శూన్య);
శూన్య NORETURN VISIBLE restore_user_context(శూన్య)
{
    c_exit_hook();

    కణుపు_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    ఒకవేళ (ARCH_కణుపు_STATE(x86_KSPending_అంతరాయం) != int_invalid) {
        /* put this in service */
        అంతరాయం_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm అస్థిర(
            "mov %[stack_top], %%esp\n"
            "push %[syscall] \n"
            "push %[irq]\n"
            "call c_handle_interrupt"
            :
            : [దొంతర_శిఖరం] "r"(&(kernel_దొంతర_alloc[CURRENT_CPU_సూచీ()][బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు)])),
            [syscall] "r"(0), /* syscall is unused for irq path */
            [irq] "r"(irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = కణుపు_STATE(ksCurThread);
#నిర్వచితమైతే CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    ఒకవేళ (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_గమ్యం_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } లేకపోతే ఒకవేళ (vcpu) {
        vcpu_fpu_గమ్యం_host(cur_thread, vcpu);
    }
#అయితే_ముగింపు
    సమితి_Kernel_నమోదు_దొంతర_సూచిక(cur_thread);

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#అయితే_ముగింపు

    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    /* see if we entered via syscall */
    ఒకవేళ (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1)) {
        cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
        asm అస్థిర(
            // Set our stack pointer to the top of the tcb so we can efficiently pop
            "movl %0, %%esp\n"
            // restore syscall number
            "popl %%eax\n"
            // cap/badge register
            "popl %%ebx\n"
            // skip ecx and edx, these will contain esp and NextIP due to sysenter/sysexit convention
            "addl $8, %%esp\n"
            // message info register
            "popl %%esi\n"
            // message register
            "popl %%edi\n"
            // message register
            "popl %%ebp\n"
            // skip FaultIP and Error (these are fake registers)
            "addl $8, %%esp\n"
            // restore NextIP
            "popl %%edx\n"
            // skip cs
            "addl $4,  %%esp\n"
            "movl 4(%%esp), %%ecx\n"
            "popfl\n"
            "orl %[IFMASK], -4(%%esp)\n"
            "sti\n"
            "sysexit\n"
            :
            : "r"(&cur_thread->tcbArch.tcbContext.registers[EAX]),
            [IFMASK]"i"(FLAGS_IF)
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "memory"
        );
    } లేకపోతే {
        asm అస్థిర(
            // Set our stack pointer to the top of the tcb so we can efficiently pop
            "movl %0, %%esp\n"
            "popl %%eax\n"
            "popl %%ebx\n"
            "popl %%ecx\n"
            "popl %%edx\n"
            "popl %%esi\n"
            "popl %%edi\n"
            "popl %%ebp\n"
            // skip FaultIP and Error
            "addl $8, %%esp\n"
            "iret\n"
            :
            : "r"(&cur_thread->tcbArch.tcbContext.registers[EAX])
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "memory"
        );
    }

    UNREACHABLE();
}
