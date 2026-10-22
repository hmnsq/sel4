/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/traps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/stack.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
ਸਥਿਰ ਖਾਲੀ NORETURN ਵਰਤਿਆ vmlaunch_failed(ਸ਼ਬਦ_t failInvalid, ਸ਼ਬਦ_t failValid)
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();

    ਜੇ (failInvalid) {
        userError("current VMCS pointer is not valid");
    }
    ਜੇ (failValid) {
        userError("vmlaunch/vmresume error %d", (ਪੂਰਨ)vmread(VMX_ਅੰਕੜੇ_INSTRUCTION_ERROR));
    }

    handle_Vm_ਇੰਦਰਾਜ_Fail();
    restore_user_context();
}

ਸਥਿਰ ਖਾਲੀ NORETURN restore_vmx(tcb_t *cur_thread, vcpu_t *vcpu)
{
    restoreVMCS();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    /* Do not support breakpoints in VMs, so just disable all breakpoints */
    ਲੱਦੋ_All_Disabled_Breakpoint_State(cur_thread);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    vcpu_restore_guest_msrs(vcpu);
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* attempt to do a vmlaunch/vmresume */
    asm ਅਸਥਿਰ(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
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
#ਨਹੀਂਤਾਂ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Set our stack pointer to the top of the tcb so we can efficiently pop
        "movq %[guest_regs], %%rsp\n"
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "cmpq $0x1, (%[launched])\n" // is the VM launched already?
        "jne launch\n"
        "popq %%rax\n"
        "popq %%rbx\n"
        "popq %%rcx\n"
        "popq %%rdx\n"
        "popq %%rsi\n"
        "popq %%rdi\n"
        "popq %%rbp\n"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#ਨਹੀਂਤਾਂ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
        "swapgs\n"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#ਨਹੀਂਤਾਂ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
        "swapgs\n"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "vmlaunch\n" // no, do the vmlaunch

        "done:\n"
        "setb %%al\n"
        "sete %%bl\n"
        "movzx %%al, %%rdi\n"
        "movzx %%bl, %%rsi\n"
        // if we get here we failed

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
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

#ਨਹੀਂਤਾਂ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
        "swapgs\n"
        "movq %%gs:%c[stack_offset], %%rsp\n"
#ਨਹੀਂਤਾਂ
        "leaq kernel_stack_alloc + %c[stack_size], %%rsp\n"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "leaq vmlaunch_failed(%%rip), %%rax\n"
        "jmp *%%rax\n"
        :
        : [guest_regs]"r"(vcpu->gp_registers),
        [launched]"r"(&vcpu->launched),
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
        [ਢੇਰ_ਮਾਪ]"i"(ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ)),
        [guest_msr]"r"(vcpu->guest_msr_registers),
        [host_msr]"r"(&vcpu->host_msr_registers[n_vcpu_msr_ਰਜਿਸਟਰ])
#ਨਹੀਂਤਾਂ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        [ਢੇਰ_ਮਾਪ]"i"(ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
        , [ਢੇਰ_ਵਿੱਥ]"i"(OFFSETOF(ਗੰਢ_Info_t, ਢੇਰ_ਸਿਖਰ_2))
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Clobber memory so the compiler is forced to complete all stores
        // before running this assembler. Leave rbx, r8, r9 and r10 not clobbered
        // so that the inputs get pinned to those general purpose registers.
        : "memory", "cc", "rax", "rcx", "rdx", "r11", "r12", "r13", "r14", "r15"
    );
    UNREACHABLE();
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ VISIBLE NORETURN restore_user_context(ਖਾਲੀ)
{
    c_exit_hook();

    ਗੰਢ_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    ਜੇ (ARCH_ਗੰਢ_STATE(x86_KSPending_ਰੁਕਾਵਟ) != int_invalid) {
        ਰੁਕਾਵਟ_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm ਅਸਥਿਰ(
            "movq %[stack_top], %%rsp\n"
            "movq %[syscall], %%rsi\n"
            "movq %[irq], %%rdi\n"
            "call c_handle_interrupt"
            :
            : [ਢੇਰ_ਸਿਖਰ] "r"(&(kernel_ਢੇਰ_alloc[CURRENT_CPU_ਸੂਚਕਾਂਕ()][ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ)])),
            [syscall] "i"(0), /* syscall is unused for irq path */
            [irq] "r"((se_L4_ਸ਼ਬਦ)irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = ਗੰਢ_STATE(ksCurThread);
    ਸ਼ਬਦ_t *irqstack = x64KSIRQStack[CURRENT_CPU_ਸੂਚਕਾਂਕ()];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    ਜੇ (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_ਮੰਜ਼ਿਲ_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } ਨਹੀਂਤਾਂ ਜੇ (vcpu) {
        vcpu_fpu_ਮੰਜ਼ਿਲ_host(cur_thread, vcpu);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
    ਸ਼ਬਦ_t user_cr3 = ਬਹੁਲਕ_ਗੰਢ_STATE(x64KSCurrentUserCR3);
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_SKIM_WINDOW */
    swapgs();
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    // Check if we are returning from a syscall/sysenter or from an interrupt
    // There is a special case where if we would be returning from a sysenter,
    // but are current singlestepping, do a full return like an interrupt
    ਜੇ (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1) &&
        (!config_ਸੈੱਟ(CONFIG_SYSENTER) || !config_ਸੈੱਟ(CONFIG_HARDWARE_DEBUG_API)
         || ((cur_thread->tcbArch.tcbContext.registers[FLAGS] & FLAGS_TF) == 0))) {
        ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)) {
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
        ਜੇ (config_ਸੈੱਟ(CONFIG_SYSENTER)) {
            cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)
            ਰਜਿਸਟਰ ਸ਼ਬਦ_t user_cr3_r11 asm("r11") = user_cr3;
#ਜੇ_ਅੰਤ
            asm ਅਸਥਿਰ(
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
#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)
                "popq %%rsp\n"
                "movq %%r11, %%cr3\n"
                "movq %%rsp, %%r11\n"
#ਨਹੀਂਤਾਂ
                "popq %%r11\n"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_SKIM_WINDOW */
#ਜੇ_ਅੰਤ /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sti\n"
                SYSEXITQ "\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI]),
#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)
                "r"(user_cr3_r11),
#ਜੇ_ਅੰਤ
                [IF] "i"(FLAGS_IF)
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        } ਨਹੀਂਤਾਂ {
            asm ਅਸਥਿਰ(
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
#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)
                "popq %%rsp\n"
                "movq %%rcx, %%cr3\n"
                "movq %%rsp, %%rcx\n"
#ਨਹੀਂਤਾਂ
                "popq %%rcx\n"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_SKIM_WINDOW */
#ਜੇ_ਅੰਤ /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // clear RSP to not leak information to the user
                "xor %%rsp, %%rsp\n"
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sysretq\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)
                , "c"(user_cr3)
#ਜੇ_ਅੰਤ
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        }
    } ਨਹੀਂਤਾਂ {
        /* construct our return from interrupt frame */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
        /* Have to zero this to prevent storage channel */
        irqstack[0] = 0;
#ਜੇ_ਅੰਤ
        irqstack[1] = get_ਰਜਿਸਟਰ(cur_thread, ਅਗਲਾ_IP);
        irqstack[2] = get_ਰਜਿਸਟਰ(cur_thread, CS);
        irqstack[3] = get_ਰਜਿਸਟਰ(cur_thread, FLAGS);
        irqstack[4] = get_ਰਜਿਸਟਰ(cur_thread, RSP);
        irqstack[5] = get_ਰਜਿਸਟਰ(cur_thread, SS);
        asm ਅਸਥਿਰ(
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

#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)
            /* pop into rsp as we're done with the stack for now and we need to
             * preserve our rcx value as it has our next cr3 value */
            "popq %%rsp\n"
#ਨਹੀਂਤਾਂ
            "popq %%rcx\n"
#ਜੇ_ਅੰਤ /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
            // Swapping gs twice here is worth it as it allows us to efficiently
            // set the user gs base previously
            "swapgs\n"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
            /* now we stash rcx in the scratch space that we can access once we've
             * loaded the user cr3 */
            "movq %%rsp, %%gs:%c[scratch_offset]\n"
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_SKIM_WINDOW */
            "movq %%gs:8, %%rsp\n"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
            /* change to the user address space and then load the value of rcx that
             * we stashed */
            "movq %%rcx, %%cr3\n"
            "movq %%gs:%c[scratch_offset], %%rcx\n"
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_SKIM_WINDOW */
            "addq $8, %%rsp\n"
            // Switch to the user GS value
            "swapgs\n"
#ਨਹੀਂਤਾਂ /* !ENABLE_SMP_SUPPORT */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
            "movq (x64KSCurrentUserCR3), %%rsp\n"
            "movq %%rsp, %%cr3\n"
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_SKIM_WINDOW */
            "leaq x64KSIRQStack + 8, %%rsp\n"
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
            "iretq\n"
            :
            : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)
            , "c"(user_cr3)
            , [scratch_ਵਿੱਥ] "i"(ਗੰਢ_Skim_Scratch_ਵਿੱਥ)
#ਜੇ_ਅੰਤ
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "memory"
        );
    }
    UNREACHABLE();
}

ਖਾਲੀ VISIBLE NORETURN c_x64_handle_ਰੁਕਾਵਟ(ਪੂਰਨ irq, ਪੂਰਨ syscall);
ਖਾਲੀ VISIBLE NORETURN c_x64_handle_ਰੁਕਾਵਟ(ਪੂਰਨ irq, ਪੂਰਨ syscall)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }
    ਸ਼ਬਦ_t *irq_ਢੇਰ = x64KSIRQStack[CURRENT_CPU_ਸੂਚਕਾਂਕ()];
    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), Error, irq_ਢੇਰ[0]);
    /* In the case of an interrupt the NextIP and the FaultIP should be the same value,
     * i.e. the address of the instruction the CPU was about to execute before the
     * interrupt. This is the 5th value pushed on by the hardware, so indexing from
     * the bottom is x64KSIRQStack[1] */
    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਅਗਲਾ_IP, irq_ਢੇਰ[1]);
    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), FaultIP, irq_ਢੇਰ[1]);
    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), FLAGS, irq_ਢੇਰ[3]);
    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), RSP, irq_ਢੇਰ[4]);
    c_handle_ਰੁਕਾਵਟ(irq, syscall);
    UNREACHABLE();
}
