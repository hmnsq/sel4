/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/statedata.h>
#ضمن <machine/fpu.h>
#ضمن <kernel/traps.h>
#ضمن <arch/machine/debug.h>
#ضمن <kernel/stack.h>

#ضمن <api/syscall.h>

#اگر_تعریف_شده CONFIG_VTX
ساكن فراغ NORETURN استفاده_شده vmlaunch_failed(کلمه_t failInvalid, کلمه_t failValid)
{
    گره_قفل_SYS;

    c_مدخل_hook();

    إذا (failInvalid) {
        userError("current VMCS pointer is not valid");
    }
    إذا (failValid) {
        userError("vmlaunch/vmresume error %d", (صحيح)vmread(VMX_داده_INSTRUCTION_ERROR));
    }

    handle_Vm_مدخل_Fail();
    restore_user_context();
}

ساكن فراغ NORETURN restore_vmx(tcb_t *cur_thread, vcpu_t *vcpu)
{
    restoreVMCS();
#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    /* Do not support breakpoints in VMs, so just disable all breakpoints */
    بارگیری_All_Disabled_Breakpoint_State(cur_thread);
#پایان_اگر
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
    vcpu_restore_guest_msrs(vcpu);
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* attempt to do a vmlaunch/vmresume */
    asm متطاير(
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
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
#وإلا /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Set our stack pointer to the top of the tcb so we can efficiently pop
        "movq %[guest_regs], %%rsp\n"
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "cmpq $0x1, (%[launched])\n" // is the VM launched already?
        "jne launch\n"
        "popq %%rax\n"
        "popq %%rbx\n"
        "popq %%rcx\n"
        "popq %%rdx\n"
        "popq %%rsi\n"
        "popq %%rdi\n"
        "popq %%rbp\n"
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#وإلا /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
        "swapgs\n"
#پایان_اگر
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
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
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
        "popq %%r8\n"
        "popq %%r9\n"
        "popq %%r10\n"
        "popq %%r11\n"
        "popq %%r12\n"
        "popq %%r13\n"
        "popq %%r14\n"
        "popq %%r15\n"
#وإلا /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
        "swapgs\n"
#پایان_اگر
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "vmlaunch\n" // no, do the vmlaunch

        "done:\n"
        "setb %%al\n"
        "sete %%bl\n"
        "movzx %%al, %%rdi\n"
        "movzx %%bl, %%rsi\n"
        // if we get here we failed

#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
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

#وإلا /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
        "swapgs\n"
        "movq %%gs:%c[stack_offset], %%rsp\n"
#وإلا
        "leaq kernel_stack_alloc + %c[stack_size], %%rsp\n"
#پایان_اگر
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        "leaq vmlaunch_failed(%%rip), %%rax\n"
        "jmp *%%rax\n"
        :
        : [guest_regs]"r"(vcpu->gp_registers),
        [launched]"r"(&vcpu->launched),
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
        [پشته_اندازه]"i"(بیت(CONFIG_KERNEL_پشته_بیتها)),
        [guest_msr]"r"(vcpu->guest_msr_registers),
        [host_msr]"r"(&vcpu->host_msr_registers[n_vcpu_msr_ثبات])
#وإلا /* not CONFIG_X86_64_VTX_64BIT_GUESTS */
        [پشته_اندازه]"i"(بیت(CONFIG_KERNEL_پشته_بیتها))
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
        , [پشته_جابه_جایی]"i"(OFFSETOF(گره_Info_t, پشته_بالاترین_2))
#پایان_اگر
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        // Clobber memory so the compiler is forced to complete all stores
        // before running this assembler. Leave rbx, r8, r9 and r10 not clobbered
        // so that the inputs get pinned to those general purpose registers.
        : "memory", "cc", "rax", "rcx", "rdx", "r11", "r12", "r13", "r14", "r15"
    );
    UNREACHABLE();
}
#پایان_اگر

فراغ VISIBLE NORETURN restore_user_context(فراغ)
{
    c_exit_hook();

    گره_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    إذا (ARCH_گره_STATE(x86_KSPending_وقفه) != int_invalid) {
        وقفه_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm متطاير(
            "movq %[stack_top], %%rsp\n"
            "movq %[syscall], %%rsi\n"
            "movq %[irq], %%rdi\n"
            "call c_handle_interrupt"
            :
            : [پشته_بالاترین] "r"(&(kernel_پشته_alloc[CURRENT_CPU_شاخص()][بیت(CONFIG_KERNEL_پشته_بیتها)])),
            [syscall] "i"(0), /* syscall is unused for irq path */
            [irq] "r"((se_L4_کلمه)irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = گره_STATE(ksCurThread);
    کلمه_t *irqstack = x64KSIRQStack[CURRENT_CPU_شاخص()];
#اگر_تعریف_شده CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    إذا (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_مقصد_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } وإلا إذا (vcpu) {
        vcpu_fpu_مقصد_host(cur_thread, vcpu);
    }
#پایان_اگر

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#پایان_اگر

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
    کلمه_t user_cr3 = نما_گره_STATE(x64KSCurrentUserCR3);
#پایان_اگر /* CONFIG_KERNEL_SKIM_WINDOW */
    swapgs();
#پایان_اگر /* ENABLE_SMP_SUPPORT */

    إذا (config_مجموعه(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    // Check if we are returning from a syscall/sysenter or from an interrupt
    // There is a special case where if we would be returning from a sysenter,
    // but are current singlestepping, do a full return like an interrupt
    إذا (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1) &&
        (!config_مجموعه(CONFIG_SYSENTER) || !config_مجموعه(CONFIG_HARDWARE_DEBUG_API)
         || ((cur_thread->tcbArch.tcbContext.registers[FLAGS] & FLAGS_TF) == 0))) {
        إذا (config_مجموعه(CONFIG_KERNEL_SKIM_پنجره)) {
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
        إذا (config_مجموعه(CONFIG_SYSENTER)) {
            cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_پنجره)
            سجل کلمه_t user_cr3_r11 asm("r11") = user_cr3;
#پایان_اگر
            asm متطاير(
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
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_پنجره)
                "popq %%rsp\n"
                "movq %%r11, %%cr3\n"
                "movq %%rsp, %%r11\n"
#وإلا
                "popq %%r11\n"
#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#پایان_اگر /* CONFIG_KERNEL_SKIM_WINDOW */
#پایان_اگر /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sti\n"
                SYSEXITQ "\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI]),
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_پنجره)
                "r"(user_cr3_r11),
#پایان_اگر
                [IF] "i"(FLAGS_IF)
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        } وإلا {
            asm متطاير(
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
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_پنجره)
                "popq %%rsp\n"
                "movq %%rcx, %%cr3\n"
                "movq %%rsp, %%rcx\n"
#وإلا
                "popq %%rcx\n"
#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
                "movq (x64KSCurrentUserCR3), %%rsp\n"
                "movq %%rsp, %%cr3\n"
#پایان_اگر /* CONFIG_KERNEL_SKIM_WINDOW */
#پایان_اگر /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */
                // clear RSP to not leak information to the user
                "xor %%rsp, %%rsp\n"
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sysretq\n"
                :
                : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_پنجره)
                , "c"(user_cr3)
#پایان_اگر
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        }
    } وإلا {
        /* construct our return from interrupt frame */
#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
        /* Have to zero this to prevent storage channel */
        irqstack[0] = 0;
#پایان_اگر
        irqstack[1] = get_ثبات(cur_thread, بعدی_IP);
        irqstack[2] = get_ثبات(cur_thread, CS);
        irqstack[3] = get_ثبات(cur_thread, FLAGS);
        irqstack[4] = get_ثبات(cur_thread, RSP);
        irqstack[5] = get_ثبات(cur_thread, SS);
        asm متطاير(
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

#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_پنجره)
            /* pop into rsp as we're done with the stack for now and we need to
             * preserve our rcx value as it has our next cr3 value */
            "popq %%rsp\n"
#وإلا
            "popq %%rcx\n"
#پایان_اگر /* defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_WINDOW) */

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
            // Swapping gs twice here is worth it as it allows us to efficiently
            // set the user gs base previously
            "swapgs\n"
#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
            /* now we stash rcx in the scratch space that we can access once we've
             * loaded the user cr3 */
            "movq %%rsp, %%gs:%c[scratch_offset]\n"
#پایان_اگر /* CONFIG_KERNEL_SKIM_WINDOW */
            "movq %%gs:8, %%rsp\n"
#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
            /* change to the user address space and then load the value of rcx that
             * we stashed */
            "movq %%rcx, %%cr3\n"
            "movq %%gs:%c[scratch_offset], %%rcx\n"
#پایان_اگر /* CONFIG_KERNEL_SKIM_WINDOW */
            "addq $8, %%rsp\n"
            // Switch to the user GS value
            "swapgs\n"
#وإلا /* !ENABLE_SMP_SUPPORT */
#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
            "movq (x64KSCurrentUserCR3), %%rsp\n"
            "movq %%rsp, %%cr3\n"
#پایان_اگر /* CONFIG_KERNEL_SKIM_WINDOW */
            "leaq x64KSIRQStack + 8, %%rsp\n"
#پایان_اگر /* ENABLE_SMP_SUPPORT */
            "iretq\n"
            :
            : "r"(&cur_thread->tcbArch.tcbContext.registers[RDI])
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_KERNEL_SKIM_پنجره)
            , "c"(user_cr3)
            , [scratch_جابه_جایی] "i"(گره_Skim_Scratch_جابه_جایی)
#پایان_اگر
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "memory"
        );
    }
    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_x64_handle_وقفه(صحيح irq, صحيح syscall);
فراغ VISIBLE NORETURN c_x64_handle_وقفه(صحيح irq, صحيح syscall)
{
    إذا (config_مجموعه(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_enable_ibrs();
    }
    کلمه_t *irq_پشته = x64KSIRQStack[CURRENT_CPU_شاخص()];
    مجموعه_ثبات(گره_STATE(ksCurThread), Error, irq_پشته[0]);
    /* In the case of an interrupt the NextIP and the FaultIP should be the same value,
     * i.e. the address of the instruction the CPU was about to execute before the
     * interrupt. This is the 5th value pushed on by the hardware, so indexing from
     * the bottom is x64KSIRQStack[1] */
    مجموعه_ثبات(گره_STATE(ksCurThread), بعدی_IP, irq_پشته[1]);
    مجموعه_ثبات(گره_STATE(ksCurThread), FaultIP, irq_پشته[1]);
    مجموعه_ثبات(گره_STATE(ksCurThread), FLAGS, irq_پشته[3]);
    مجموعه_ثبات(گره_STATE(ksCurThread), RSP, irq_پشته[4]);
    c_handle_وقفه(irq, syscall);
    UNREACHABLE();
}
