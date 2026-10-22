/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <kernel/traps.h>
#అన్తర్భావ <arch/machine/debug.h>
#అన్తర్భావ <kernel/stack.h>

#అన్తర్భావ <api/syscall.h>

#నిర్వచితమైతే CONFIG_VTX
స్థిర శూన్య NORETURN వాడిన vmlaunch_failed(మాట_t failInvalid, మాట_t failValid)
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();

    ఒకవేళ (failInvalid) {
        userError("current VMCS pointer is not valid");
    }
    ఒకవేళ (failValid) {
        userError("vmlaunch/vmresume error %d", (పూర్ణాంకం)vmread(VMX_దత్తాంశం_INSTRUCTION_ERROR));
    }

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
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    vcpu_restore_guest_msrs(vcpu);
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* attempt to do a vmlaunch/vmresume */
    asm అస్థిర(
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
        // Save host's GS, Shadow GS, and FS
        "mov $0xC0000101, %%ecx\n"
        "rdmsr\n"
        "shl $0x20,%%rdx\n"
        "or %%rdx, %%rax\n"
        "mov %%rax, %%r11\n" // R11 has GS
        "swapgs\n"
        "rdmsr\n"
        "shl $0x20,%%rdx\n"
        "or %%rdx,%%rax\n"
        "mov %%rax, %%r12\n" // R12 has Shadow GS
        "mov $0xC0000100, %%ecx\n"
        "rdmsr\n"
        "shl $0x20,%%rdx\n"
        "or %%rdx, %%rax\n"
        "mov %%rax, %%r13\n" // R13 has FS
        "movq %[host_msr], %%rsp\n" // host_gs
        "pushq %%r13\n"
        "pushq %%r12\n"
        "pushq %%r11\n"

        // Restore guest's GS and Shadow GS
        "mov $0xC0000101, %%ecx\n"
        "swapgs\n"
        "movq %[guest_msr], %%rsp\n" // guest_gs
        "popq %%rax\n"       // GS
        "mov %%rax,%%rdx\n"
        "shr $0x20,%%rdx\n"
        "wrmsr\n"
        "swapgs\n"
        "popq %%rax\n"       // Shadow GS
        "mov %%rax,%%rdx\n"
        "shr $0x20,%%rdx\n"
        "wrmsr\n"
        "swapgs\n"
        "mov $0xC0000100, %%ecx\n"
        "popq %%rax\n"       // FS
        "mov %%rax,%%rdx\n"
        "shr $0x20,%%rdx\n"
        "wrmsr\n"
        "movq %[guest_regs], %%rsp\n"
#లేకపోతే /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Set our stack pointer to the top of the tcb so we can efficiently pop
        "movq %[guest_regs], %%rsp\n"
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "cmpq $0x1, (%[launched])\n" // is the VM launched already?
        "jne launch\n"
        "popq %%rax\n"
        "popq %%rbx\n"
        "popq %%rcx\n"
        "popq %%rdx\n"
        "popq %%rsi\n"
        "popq %%rdi\n"
        "popq %%rbp\n"
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#లేకపోతే /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
        "swapgs\n"
#అయితే_ముగింపు
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "vmresume\n" // yes, do the vmresume
        "jmp done\n"

        "launch:\n"
        "popq %%rax\n"
        "popq %%rbx\n"
        "popq %%rcx\n"
        "popq %%rdx\n"
        "popq %%rsi\n"
        "popq %%rdi\n"
        "popq %%rbp\n"
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#లేకపోతే /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
        "swapgs\n"
#అయితే_ముగింపు
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "vmlaunch\n" // no, do the vmlaunch

        "done:\n"
        "setb %%al\n"
        "sete %%bl\n"
        "movzx %%al, %%rdi\n"
        "movzx %%bl, %%rsi\n"
        // if we get here we failed

#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
        // Restore host's GS, Shadow GS, and FS
        "sub $0xA8, %%rsp\n" // 15 * 8 (regs) + 3 * 8 (guest_msr) + 3 * 8 (host_msr)
        "mov $0xC0000101, %%ecx\n"
        "pop %%rax\n"
        "movq %%rax, %%rdx\n"
        "shr $0x20, %%rdx\n"
        "wrmsr\n" // GS
        "swapgs\n"
        "pop %%rax\n"
        "movq %%rax, %%rdx\n"
        "shr $0x20, %%rdx\n"
        "wrmsr\n" // Shadow GS
        "mov $0xC0000100, %%ecx\n"
        "pop %%rax\n"
        "movq %%rax, %%rdx\n"
        "shr $0x20, %%rdx\n"
        "wrmsr\n" // FS

#లేకపోతే /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
        "swapgs\n"
        "movq %%gs:%c[stack_offset], %%rsp\n"
#లేకపోతే
        "leaq kernel_stack_alloc + %c[stack_size], %%rsp\n"
#అయితే_ముగింపు
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "leaq vmlaunch_failed(%%rip), %%rax\n"
        "jmp *%%rax\n"
        :
        : [guest_regs]"r"(vcpu->gp_registers),
        [launched]"r"(&vcpu->launched),
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
        [దొంతర_కొలత]"i"(బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు)),
        [guest_msr]"r"(vcpu->guest_msr_registers),
        [host_msr]"r"(&vcpu->host_msr_registers[n_vcpu_msr_రిజిస్టరు])
#లేకపోతే /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        [దొంతర_కొలత]"i"(బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు))
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
        , [దొంతర_స్థానభ్రంశం]"i"(OFFSETOF(కణుపు_Info_t, దొంతర_శిఖరం_2))
#అయితే_ముగింపు
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Clobber memory so the compiler is forced to complete all stores
        // before running this assembler. Leave rbx, r8, r9 and r10 not clobbered
        // so that the inputs get pinned to those general purpose registers.
        : "memory", "cc", "rax", "rcx", "rdx", "r11", "r12", "r13", "r14", "r15"
    );
    UNREACHABLE();
}
#అయితే_ముగింపు

శూన్య VISIBLE NORETURN restore_user_context(శూన్య)
{
    c_exit_hook();

    కణుపు_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    ఒకవేళ (ARCH_కణుపు_STATE(x86_KSPending_అంతరాయం) != int_invalid) {
        అంతరాయం_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm అస్థిర(
            "movq %[stack_top], %%rsp\n"
            "movq %[syscall], %%rsi\n"
            "movq %[irq], %%rdi\n"
            "call c_handle_interrupt"
            :
            : [దొంతర_శిఖరం] "r"(&(kernel_దొంతర_alloc[CURRENT_CPU_సూచీ()][బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు)])),
            [syscall] "i"(0), /* syscall is unused for irq path */
            [irq] "r"((se_L4_మాట)irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = కణుపు_STATE(ksCurThread);
    మాట_t *irqstack = x64KSIRQStack[CURRENT_CPU_సూచీ()];
#నిర్వచితమైతే CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    ఒకవేళ (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_గమ్యం_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } లేకపోతే ఒకవేళ (vcpu) {
        vcpu_fpu_గమ్యం_host(cur_thread, vcpu);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#అయితే_ముగింపు

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
    మాట_t user_cr3 = బహుళకం_కణుపు_STATE(x64KSCurrentUserCR3);
#అయితే_ముగింపు /* CONFIG_KERNEL_SKIM_WINDOW */
    swapgs();
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    // Check if we are returning from a syscall/sysenter or from an interrupt
    // There is a special case where if we would be returning from a sysenter,
    // but are current singlestepping, do a full return like an interrupt
    ఒకవేళ (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1) &&
        (!config_సమితి(CONFIG_SYSENTER) || !config_సమితి(CONFIG_HARDWARE_DEBUG_API)
         || ((cur_thread->tcbArch.tcbContext.registers[FLAGS] & FLAGS_TF) == 0))) {
        ఒకవేళ (config_సమితి(CONFIG_KERNEL_SKIM_కిటికీ)) {
            /* if we are using the SKIM window then we are trying to hide kernel state from
             * the user in the case of Meltdown where the kernel region is effectively readable
             * by the user. To prevent a storage channel across threads through the irq stack,
             * which is indirectly controlled by the user, we need to clear the stack. We perform
             * this here since when we return *from* an interrupt we must use this stack and
             * cannot clear it. This means if we restore from interrupt, then enter from a syscall
             * and switch to a different thread we must either on syscall entry, or before leaving
             * the kernel, clear the irq stack. */
            irqstack[0] = 0;
            irqstack[1] = 0;
            irqstack[2] = 0;
            irqstack[3] = 0;
            irqstack[4] = 0;
            irqstack[5] = 0;
        }
        ఒకవేళ (config_సమితి(CONFIG_SYSENTER)) {
            cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_కిటికీ)
            నమోదు మాట_t user_cr3_r11 asm("r11") = user_cr3;
#అయితే_ముగింపు
            asm అస్థిర(
                // Set our stack pointer to the top of the tcb so we can efficiently pop
                "movq %0, %%rsp\n"
                "popq %%rdi\n"
                "popq %%rsi\n"
                "popq %%rax\n"
                "popq %%rbx\n"
                "popq %%rbp\n"
                "popq %%r12\n"
                "popq %%r13\n"
                "popq %%r14\n"
                // skip RDX
                "addq $8, %%rsp\n"
                "popq %%r10\n"
                "popq %%r8\n"
                "popq %%r9\n"
                "popq %%r15\n"
                //restore RFLAGS
                "popfq\n"
                // reset interrupt bit
                "orq %[IF], -8(%%rsp)\n"
                // Restore NextIP
                "popq %%rdx\n"
                // Skip ERROR
                "addq $8, %%rsp\n"
                // Restore RSP
                "popq %%rcx\n"
                // Skip FaultIP
                "addq $8, %%rsp\n"
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_కిటికీ)
                "popq %%rsp\n"
                "movq %%r11, %%cr3\n"
                "movq %%rsp, %%r11\n"
#లేకపోతే
                "popq %%r11\n"
#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#అయితే_ముగింపు /* CONFIG_KERNEL_SKIM_WINDOW */
#అయితే_ముగింపు /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sti\n"
                SYSEXITQ "\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI]),
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_కిటికీ)
                "r"(user_cr3_r11),
#అయితే_ముగింపు
                [IF] "i"(FLAGS_IF)
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        } లేకపోతే {
            asm అస్థిర(
                // Set our stack pointer to the top of the tcb so we can efficiently pop
                "movq %0, %%rsp\n"
                "popq %%rdi\n"
                "popq %%rsi\n"
                "popq %%rax\n"
                "popq %%rbx\n"
                "popq %%rbp\n"
                "popq %%r12\n"
                "popq %%r13\n"
                "popq %%r14\n"
                "popq %%rdx\n"
                "popq %%r10\n"
                "popq %%r8\n"
                "popq %%r9\n"
                "popq %%r15\n"
                //restore RFLAGS
                "popq %%r11\n"
                // Restore NextIP
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_కిటికీ)
                "popq %%rsp\n"
                "movq %%rcx, %%cr3\n"
                "movq %%rsp, %%rcx\n"
#లేకపోతే
                "popq %%rcx\n"
#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#అయితే_ముగింపు /* CONFIG_KERNEL_SKIM_WINDOW */
#అయితే_ముగింపు /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // clear RSP to not leak information to the user
                "xor %%rsp, %%rsp\n"
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sysretq\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_కిటికీ)
                , "c"(user_cr3)
#అయితే_ముగింపు
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        }
    } లేకపోతే {
        /* construct our return from interrupt frame */
#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
        /* Have to zero this to prevent storage channel */
        irqstack[0] = 0;
#అయితే_ముగింపు
        irqstack[1] = get_రిజిస్టరు(cur_thread, తదుపరి_IP);
        irqstack[2] = get_రిజిస్టరు(cur_thread, CS);
        irqstack[3] = get_రిజిస్టరు(cur_thread, FLAGS);
        irqstack[4] = get_రిజిస్టరు(cur_thread, RSP);
        irqstack[5] = get_రిజిస్టరు(cur_thread, SS);
        asm అస్థిర(
            // Set our stack pointer to the top of the tcb so we can efficiently pop
            "movq %0, %%rsp\n"
            "popq %%rdi\n"
            "popq %%rsi\n"
            "popq %%rax\n"
            "popq %%rbx\n"
            "popq %%rbp\n"
            "popq %%r12\n"
            "popq %%r13\n"
            "popq %%r14\n"
            "popq %%rdx\n"
            "popq %%r10\n"
            "popq %%r8\n"
            "popq %%r9\n"
            "popq %%r15\n"
            /* skip RFLAGS, Error, NextIP, RSP, and FaultIP */
            "addq $40, %%rsp\n"
            "popq %%r11\n"

#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_కిటికీ)
            /* pop into rsp as we're done with the stack for now and we need to
             * preserve our rcx value as it has our next cr3 value */
            "popq %%rsp\n"
#లేకపోతే
            "popq %%rcx\n"
#అయితే_ముగింపు /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
            // Swapping gs twice here is worth it as it allows us to efficiently
            // set the user gs base previously
            "swapgs\n"
#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
            /* now we stash rcx in the scratch space that we can access once we've
             * loaded the user cr3 */
            "movq %%rsp, %%gs:%c[scratch_offset]\n"
#అయితే_ముగింపు /* CONFIG_KERNEL_SKIM_WINDOW */
            "movq %%gs:8, %%rsp\n"
#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
            /* change to the user address space and then load the value of rcx that
             * we stashed */
            "movq %%rcx, %%cr3\n"
            "movq %%gs:%c[scratch_offset], %%rcx\n"
#అయితే_ముగింపు /* CONFIG_KERNEL_SKIM_WINDOW */
            "addq $8, %%rsp\n"
            // Switch to the user GS value
            "swapgs\n"
#లేకపోతే /* !ENABLE_SMP_SUPPORT */
#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
            "movq (x64KSCurrentUserCR3), %%rsp\n"
            "movq %%rsp, %%cr3\n"
#అయితే_ముగింపు /* CONFIG_KERNEL_SKIM_WINDOW */
            "leaq x64KSIRQStack + 8, %%rsp\n"
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
            "iretq\n"
            :
            : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_కిటికీ)
            , "c"(user_cr3)
            , [scratch_స్థానభ్రంశం] "i"(కణుపు_Skim_Scratch_స్థానభ్రంశం)
#అయితే_ముగింపు
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "memory"
        );
    }
    UNREACHABLE();
}

శూన్య VISIBLE NORETURN c_x64_handle_అంతరాయం(పూర్ణాంకం irq, పూర్ణాంకం syscall);
శూన్య VISIBLE NORETURN c_x64_handle_అంతరాయం(పూర్ణాంకం irq, పూర్ణాంకం syscall)
{
    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }
    మాట_t *irq_దొంతర = x64KSIRQStack[CURRENT_CPU_సూచీ()];
    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), Error, irq_దొంతర[0]);
    /* In the case of an interrupt the NextIP and the FaultIP should be the same value,
     * i.e. the address of the instruction the CPU was about to execute before the
     * interrupt. This is the 5th value pushed on by the hardware, so indexing from
     * the bottom is x64KSIRQStack[1] */
    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), తదుపరి_IP, irq_దొంతర[1]);
    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), FaultIP, irq_దొంతర[1]);
    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), FLAGS, irq_దొంతర[3]);
    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), RSP, irq_దొంతర[4]);
    c_handle_అంతరాయం(irq, syscall);
    UNREACHABLE();
}
