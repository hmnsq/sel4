/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <kernel/traps.h>
#अन्तर्भाव <arch/machine/debug.h>
#अन्तर्भाव <kernel/stack.h>

#अन्तर्भाव <api/syscall.h>

#ifdef CONFIG_VTX
स्थिर शून्य NORETURN USED vmlaunch_failed(word_t failInvalid, word_t failValid)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();

    यदि (failInvalid) {
        userError("current VMCS pointer is not valid");
    }
    यदि (failValid) {
        userError("vmlaunch/vmresume error %d", (पूर्णाङ्क)vmread(VMX_DATA_INSTRUCTION_ERROR));
    }

    handleVmEntryFail();
    restore_user_context();
}

स्थिर शून्य NORETURN restore_vmx(tcb_t *cur_thread, vcpu_t *vcpu)
{
    restoreVMCS();
#ifdef CONFIG_HARDWARE_DEBUG_API
    /* Do not support breakpoints in VMs, so just disable all breakpoints */
    loadAllDisabledBreakpointState(cur_thread);
#endif
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    vcpu_restore_guest_msrs(vcpu);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* attempt to do a vmlaunch/vmresume */
    asm चञ्चल(
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
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
#अन्यथा /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Set our stack pointer to the top of the tcb so we can efficiently pop
        "movq %[guest_regs], %%rsp\n"
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "cmpq $0x1, (%[launched])\n" // is the VM launched already?
        "jne launch\n"
        "popq %%rax\n"
        "popq %%rbx\n"
        "popq %%rcx\n"
        "popq %%rdx\n"
        "popq %%rsi\n"
        "popq %%rdi\n"
        "popq %%rbp\n"
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#अन्यथा /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ifdef ENABLE_SMP_SUPPORT
        "swapgs\n"
#endif
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
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
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#अन्यथा /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ifdef ENABLE_SMP_SUPPORT
        "swapgs\n"
#endif
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "vmlaunch\n" // no, do the vmlaunch

        "done:\n"
        "setb %%al\n"
        "sete %%bl\n"
        "movzx %%al, %%rdi\n"
        "movzx %%bl, %%rsi\n"
        // if we get here we failed

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
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

#अन्यथा /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ifdef ENABLE_SMP_SUPPORT
        "swapgs\n"
        "movq %%gs:%c[stack_offset], %%rsp\n"
#अन्यथा
        "leaq kernel_stack_alloc + %c[stack_size], %%rsp\n"
#endif
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "leaq vmlaunch_failed(%%rip), %%rax\n"
        "jmp *%%rax\n"
        :
        : [guest_regs]"r"(vcpu->gp_registers),
        [launched]"r"(&vcpu->launched),
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
        [ढेर_size]"i"(BIT(CONFIG_KERNEL_ढेर_BITS)),
        [guest_msr]"r"(vcpu->guest_msr_registers),
        [host_msr]"r"(&vcpu->host_msr_registers[n_vcpu_msr_register])
#अन्यथा /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        [ढेर_size]"i"(BIT(CONFIG_KERNEL_ढेर_BITS))
#ifdef ENABLE_SMP_SUPPORT
        , [ढेर_offset]"i"(OFFSETOF(गाँठ_Info_t, ढेर_शीर्ष_2))
#endif
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Clobber memory so the compiler is forced to complete all stores
        // before running this assembler. Leave rbx, r8, r9 and r10 not clobbered
        // so that the inputs get pinned to those general purpose registers.
        : "memory", "cc", "rax", "rcx", "rdx", "r11", "r12", "r13", "r14", "r15"
    );
    UNREACHABLE();
}
#endif

शून्य VISIBLE NORETURN restore_user_context(शून्य)
{
    c_exit_hook();

    गाँठ_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    यदि (ARCH_गाँठ_STATE(x86KSPendingInterrupt) != int_invalid) {
        interrupt_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm चञ्चल(
            "movq %[stack_top], %%rsp\n"
            "movq %[syscall], %%rsi\n"
            "movq %[irq], %%rdi\n"
            "call c_handle_interrupt"
            :
            : [ढेर_शीर्ष] "r"(&(kernel_ढेर_alloc[CURRENT_CPU_INDEX()][BIT(CONFIG_KERNEL_ढेर_BITS)])),
            [syscall] "i"(0), /* syscall is unused for irq path */
            [irq] "r"((seL4_Word)irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = गाँठ_STATE(ksCurThread);
    word_t *irqstack = x64KSIRQStack[CURRENT_CPU_INDEX()];
#ifdef CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    यदि (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_to_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } अन्यथा यदि (vcpu) {
        vcpu_fpu_to_host(cur_thread, vcpu);
    }
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#endif

#ifdef ENABLE_SMP_SUPPORT
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    word_t user_cr3 = MODE_गाँठ_STATE(x64KSCurrentUserCR3);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */
    swapgs();
#endif /* ENABLE_SMP_SUPPORT */

    यदि (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    // Check if we are returning from a syscall/sysenter or from an interrupt
    // There is a special case where if we would be returning from a sysenter,
    // but are current singlestepping, do a full return like an interrupt
    यदि (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1) &&
        (!config_set(CONFIG_SYSENTER) || !config_set(CONFIG_HARDWARE_DEBUG_API)
         || ((cur_thread->tcbArch.tcbContext.registers[FLAGS] & FLAGS_TF) == 0))) {
        यदि (config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
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
        यदि (config_set(CONFIG_SYSENTER)) {
            cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW)
            पञ्जिका word_t user_cr3_r11 asm("r11") = user_cr3;
#endif
            asm चञ्चल(
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
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW)
                "popq %%rsp\n"
                "movq %%r11, %%cr3\n"
                "movq %%rsp, %%r11\n"
#अन्यथा
                "popq %%r11\n"
#ifdef CONFIG_KERNEL_SKIM_WINDOW
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#endif /* CONFIG_KERNEL_SKIM_WINDOW */
#endif /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sti\n"
                SYSEXITQ "\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI]),
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW)
                "r"(user_cr3_r11),
#endif
                [IF] "i"(FLAGS_IF)
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        } अन्यथा {
            asm चञ्चल(
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
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW)
                "popq %%rsp\n"
                "movq %%rcx, %%cr3\n"
                "movq %%rsp, %%rcx\n"
#अन्यथा
                "popq %%rcx\n"
#ifdef CONFIG_KERNEL_SKIM_WINDOW
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#endif /* CONFIG_KERNEL_SKIM_WINDOW */
#endif /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // clear RSP to not leak information to the user
                "xor %%rsp, %%rsp\n"
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sysretq\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW)
                , "c"(user_cr3)
#endif
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        }
    } अन्यथा {
        /* construct our return from interrupt frame */
#ifdef CONFIG_KERNEL_SKIM_WINDOW
        /* Have to zero this to prevent storage channel */
        irqstack[0] = 0;
#endif
        irqstack[1] = getRegister(cur_thread, NextIP);
        irqstack[2] = getRegister(cur_thread, CS);
        irqstack[3] = getRegister(cur_thread, FLAGS);
        irqstack[4] = getRegister(cur_thread, RSP);
        irqstack[5] = getRegister(cur_thread, SS);
        asm चञ्चल(
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

#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW)
            /* pop into rsp as we're done with the stack for now and we need to
             * preserve our rcx value as it has our next cr3 value */
            "popq %%rsp\n"
#अन्यथा
            "popq %%rcx\n"
#endif /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */

#ifdef ENABLE_SMP_SUPPORT
            // Swapping gs twice here is worth it as it allows us to efficiently
            // set the user gs base previously
            "swapgs\n"
#ifdef CONFIG_KERNEL_SKIM_WINDOW
            /* now we stash rcx in the scratch space that we can access once we've
             * loaded the user cr3 */
            "movq %%rsp, %%gs:%c[scratch_offset]\n"
#endif /* CONFIG_KERNEL_SKIM_WINDOW */
            "movq %%gs:8, %%rsp\n"
#ifdef CONFIG_KERNEL_SKIM_WINDOW
            /* change to the user address space and then load the value of rcx that
             * we stashed */
            "movq %%rcx, %%cr3\n"
            "movq %%gs:%c[scratch_offset], %%rcx\n"
#endif /* CONFIG_KERNEL_SKIM_WINDOW */
            "addq $8, %%rsp\n"
            // Switch to the user GS value
            "swapgs\n"
#अन्यथा /* !ENABLE_SMP_SUPPORT */
#ifdef CONFIG_KERNEL_SKIM_WINDOW
            "movq (x64KSCurrentUserCR3), %%rsp\n"
            "movq %%rsp, %%cr3\n"
#endif /* CONFIG_KERNEL_SKIM_WINDOW */
            "leaq x64KSIRQStack + 8, %%rsp\n"
#endif /* ENABLE_SMP_SUPPORT */
            "iretq\n"
            :
            : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW)
            , "c"(user_cr3)
            , [scratch_offset] "i"(गाँठ_Skim_Scratch_Offset)
#endif
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "memory"
        );
    }
    UNREACHABLE();
}

शून्य VISIBLE NORETURN c_x64_handle_interrupt(पूर्णाङ्क irq, पूर्णाङ्क syscall);
शून्य VISIBLE NORETURN c_x64_handle_interrupt(पूर्णाङ्क irq, पूर्णाङ्क syscall)
{
    यदि (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }
    word_t *irq_ढेर = x64KSIRQStack[CURRENT_CPU_INDEX()];
    setRegister(गाँठ_STATE(ksCurThread), Error, irq_ढेर[0]);
    /* In the case of an interrupt the NextIP and the FaultIP should be the same value,
     * i.e. the address of the instruction the CPU was about to execute before the
     * interrupt. This is the 5th value pushed on by the hardware, so indexing from
     * the bottom is x64KSIRQStack[1] */
    setRegister(गाँठ_STATE(ksCurThread), NextIP, irq_ढेर[1]);
    setRegister(गाँठ_STATE(ksCurThread), FaultIP, irq_ढेर[1]);
    setRegister(गाँठ_STATE(ksCurThread), FLAGS, irq_ढेर[3]);
    setRegister(गाँठ_STATE(ksCurThread), RSP, irq_ढेर[4]);
    c_handle_interrupt(irq, syscall);
    UNREACHABLE();
}
