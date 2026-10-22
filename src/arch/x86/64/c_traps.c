/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <kernel/stack.h>

#ಅನ್ತರ್ಭಾವ <api/syscall.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
ಸ್ಥಿರ ಶೂನ್ಯ NORETURN ಬಳಸಿದ vmlaunch_failed(ಶಬ್ದ_t failInvalid, ಶಬ್ದ_t failValid)
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();

    ಆದರೆ (failInvalid) {
        userError("current VMCS pointer is not valid");
    }
    ಆದರೆ (failValid) {
        userError("vmlaunch/vmresume error %d", (ಪೂರ್ಣಾಂಕ)vmread(VMX_ದತ್ತಾಂಶ_INSTRUCTION_ERROR));
    }

    handle_Vm_ನಮೂದು_Fail();
    restore_user_context();
}

ಸ್ಥಿರ ಶೂನ್ಯ NORETURN restore_vmx(tcb_t *cur_thread, vcpu_t *vcpu)
{
    restoreVMCS();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    /* Do not support breakpoints in VMs, so just disable all breakpoints */
    ತುಂಬು_All_Disabled_Breakpoint_State(cur_thread);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    vcpu_restore_guest_msrs(vcpu);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* attempt to do a vmlaunch/vmresume */
    asm ಅಸ್ಥಿರ(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
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
#ಇಲ್ಲದಿದ್ದರೆ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Set our stack pointer to the top of the tcb so we can efficiently pop
        "movq %[guest_regs], %%rsp\n"
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "cmpq $0x1, (%[launched])\n" // is the VM launched already?
        "jne launch\n"
        "popq %%rax\n"
        "popq %%rbx\n"
        "popq %%rcx\n"
        "popq %%rdx\n"
        "popq %%rsi\n"
        "popq %%rdi\n"
        "popq %%rbp\n"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#ಇಲ್ಲದಿದ್ದರೆ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
        "swapgs\n"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#ಇಲ್ಲದಿದ್ದರೆ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
        "swapgs\n"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "vmlaunch\n" // no, do the vmlaunch

        "done:\n"
        "setb %%al\n"
        "sete %%bl\n"
        "movzx %%al, %%rdi\n"
        "movzx %%bl, %%rsi\n"
        // if we get here we failed

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
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

#ಇಲ್ಲದಿದ್ದರೆ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
        "swapgs\n"
        "movq %%gs:%c[stack_offset], %%rsp\n"
#ಇಲ್ಲದಿದ್ದರೆ
        "leaq kernel_stack_alloc + %c[stack_size], %%rsp\n"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "leaq vmlaunch_failed(%%rip), %%rax\n"
        "jmp *%%rax\n"
        :
        : [guest_regs]"r"(vcpu->gp_registers),
        [launched]"r"(&vcpu->launched),
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
        [ರಾಶಿ_ಗಾತ್ರ]"i"(ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು)),
        [guest_msr]"r"(vcpu->guest_msr_registers),
        [host_msr]"r"(&vcpu->host_msr_registers[n_vcpu_msr_ರಿಜಿಸ್ಟರ್])
#ಇಲ್ಲದಿದ್ದರೆ /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        [ರಾಶಿ_ಗಾತ್ರ]"i"(ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
        , [ರಾಶಿ_ಸ್ಥಾನಾಂತರ]"i"(OFFSETOF(ಗಂಟು_Info_t, ರಾಶಿ_ತುದಿ_2))
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Clobber memory so the compiler is forced to complete all stores
        // before running this assembler. Leave rbx, r8, r9 and r10 not clobbered
        // so that the inputs get pinned to those general purpose registers.
        : "memory", "cc", "rax", "rcx", "rdx", "r11", "r12", "r13", "r14", "r15"
    );
    UNREACHABLE();
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ VISIBLE NORETURN restore_user_context(ಶೂನ್ಯ)
{
    c_exit_hook();

    ಗಂಟು_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    ಆದರೆ (ARCH_ಗಂಟು_STATE(x86_KSPending_ಅಡಚಣೆ) != int_invalid) {
        ಅಡಚಣೆ_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm ಅಸ್ಥಿರ(
            "movq %[stack_top], %%rsp\n"
            "movq %[syscall], %%rsi\n"
            "movq %[irq], %%rdi\n"
            "call c_handle_interrupt"
            :
            : [ರಾಶಿ_ತುದಿ] "r"(&(kernel_ರಾಶಿ_alloc[CURRENT_CPU_ಸೂಚ್ಯಂಕ()][ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು)])),
            [syscall] "i"(0), /* syscall is unused for irq path */
            [irq] "r"((se_L4_ಶಬ್ದ)irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = ಗಂಟು_STATE(ksCurThread);
    ಶಬ್ದ_t *irqstack = x64KSIRQStack[CURRENT_CPU_ಸೂಚ್ಯಂಕ()];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    ಆದರೆ (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_ಗಮ್ಯ_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vcpu) {
        vcpu_fpu_ಗಮ್ಯ_host(cur_thread, vcpu);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
    ಶಬ್ದ_t user_cr3 = ಬಹುಲಕ_ಗಂಟು_STATE(x64KSCurrentUserCR3);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_SKIM_WINDOW */
    swapgs();
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    // Check if we are returning from a syscall/sysenter or from an interrupt
    // There is a special case where if we would be returning from a sysenter,
    // but are current singlestepping, do a full return like an interrupt
    ಆದರೆ (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1) &&
        (!config_ಗಣ(CONFIG_SYSENTER) || !config_ಗಣ(CONFIG_HARDWARE_DEBUG_API)
         || ((cur_thread->tcbArch.tcbContext.registers[FLAGS] & FLAGS_TF) == 0))) {
        ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)) {
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
        ಆದರೆ (config_ಗಣ(CONFIG_SYSENTER)) {
            cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)
            ನೋಂದಣಿ ಶಬ್ದ_t user_cr3_r11 asm("r11") = user_cr3;
#ಆದರೆ_ಅಂತ್ಯ
            asm ಅಸ್ಥಿರ(
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
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)
                "popq %%rsp\n"
                "movq %%r11, %%cr3\n"
                "movq %%rsp, %%r11\n"
#ಇಲ್ಲದಿದ್ದರೆ
                "popq %%r11\n"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_SKIM_WINDOW */
#ಆದರೆ_ಅಂತ್ಯ /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sti\n"
                SYSEXITQ "\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI]),
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)
                "r"(user_cr3_r11),
#ಆದರೆ_ಅಂತ್ಯ
                [IF] "i"(FLAGS_IF)
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        } ಇಲ್ಲದಿದ್ದರೆ {
            asm ಅಸ್ಥಿರ(
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
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)
                "popq %%rsp\n"
                "movq %%rcx, %%cr3\n"
                "movq %%rsp, %%rcx\n"
#ಇಲ್ಲದಿದ್ದರೆ
                "popq %%rcx\n"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_SKIM_WINDOW */
#ಆದರೆ_ಅಂತ್ಯ /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // clear RSP to not leak information to the user
                "xor %%rsp, %%rsp\n"
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sysretq\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)
                , "c"(user_cr3)
#ಆದರೆ_ಅಂತ್ಯ
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* construct our return from interrupt frame */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
        /* Have to zero this to prevent storage channel */
        irqstack[0] = 0;
#ಆದರೆ_ಅಂತ್ಯ
        irqstack[1] = get_ರಿಜಿಸ್ಟರ್(cur_thread, ಮುಂದಿನ_IP);
        irqstack[2] = get_ರಿಜಿಸ್ಟರ್(cur_thread, CS);
        irqstack[3] = get_ರಿಜಿಸ್ಟರ್(cur_thread, FLAGS);
        irqstack[4] = get_ರಿಜಿಸ್ಟರ್(cur_thread, RSP);
        irqstack[5] = get_ರಿಜಿಸ್ಟರ್(cur_thread, SS);
        asm ಅಸ್ಥಿರ(
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

#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)
            /* pop into rsp as we're done with the stack for now and we need to
             * preserve our rcx value as it has our next cr3 value */
            "popq %%rsp\n"
#ಇಲ್ಲದಿದ್ದರೆ
            "popq %%rcx\n"
#ಆದರೆ_ಅಂತ್ಯ /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
            // Swapping gs twice here is worth it as it allows us to efficiently
            // set the user gs base previously
            "swapgs\n"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
            /* now we stash rcx in the scratch space that we can access once we've
             * loaded the user cr3 */
            "movq %%rsp, %%gs:%c[scratch_offset]\n"
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_SKIM_WINDOW */
            "movq %%gs:8, %%rsp\n"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
            /* change to the user address space and then load the value of rcx that
             * we stashed */
            "movq %%rcx, %%cr3\n"
            "movq %%gs:%c[scratch_offset], %%rcx\n"
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_SKIM_WINDOW */
            "addq $8, %%rsp\n"
            // Switch to the user GS value
            "swapgs\n"
#ಇಲ್ಲದಿದ್ದರೆ /* !ENABLE_SMP_SUPPORT */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
            "movq (x64KSCurrentUserCR3), %%rsp\n"
            "movq %%rsp, %%cr3\n"
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_SKIM_WINDOW */
            "leaq x64KSIRQStack + 8, %%rsp\n"
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
            "iretq\n"
            :
            : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)
            , "c"(user_cr3)
            , [scratch_ಸ್ಥಾನಾಂತರ] "i"(ಗಂಟು_Skim_Scratch_ಸ್ಥಾನಾಂತರ)
#ಆದರೆ_ಅಂತ್ಯ
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "memory"
        );
    }
    UNREACHABLE();
}

ಶೂನ್ಯ VISIBLE NORETURN c_x64_handle_ಅಡಚಣೆ(ಪೂರ್ಣಾಂಕ irq, ಪೂರ್ಣಾಂಕ syscall);
ಶೂನ್ಯ VISIBLE NORETURN c_x64_handle_ಅಡಚಣೆ(ಪೂರ್ಣಾಂಕ irq, ಪೂರ್ಣಾಂಕ syscall)
{
    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }
    ಶಬ್ದ_t *irq_ರಾಶಿ = x64KSIRQStack[CURRENT_CPU_ಸೂಚ್ಯಂಕ()];
    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), Error, irq_ರಾಶಿ[0]);
    /* In the case of an interrupt the NextIP and the FaultIP should be the same value,
     * i.e. the address of the instruction the CPU was about to execute before the
     * interrupt. This is the 5th value pushed on by the hardware, so indexing from
     * the bottom is x64KSIRQStack[1] */
    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಮುಂದಿನ_IP, irq_ರಾಶಿ[1]);
    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), FaultIP, irq_ರಾಶಿ[1]);
    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), FLAGS, irq_ರಾಶಿ[3]);
    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), RSP, irq_ರಾಶಿ[4]);
    c_handle_ಅಡಚಣೆ(irq, syscall);
    UNREACHABLE();
}
