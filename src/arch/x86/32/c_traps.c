/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <kernel/stack.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/fastpath/fastpath.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_track.h>
#ଅନ୍ତର୍ଭାଵ <mode/stack.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/vcpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/traps.h>

#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <util.h>

#ifdef CONFIG_VTX
USED ସ୍ଥିର ଶୂନ୍ୟ NORETURN vmlaunch_failed(ଶୂନ୍ୟ)
{
    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();

    handleVmEntryFail();
    restore_user_context();
}

ସ୍ଥିର ଶୂନ୍ୟ NORETURN restore_vmx(tcb_t *cur_thread, vcpu_t *vcpu)
{
    restoreVMCS();
#ifdef CONFIG_HARDWARE_DEBUG_API
    /* Do not support breakpoints in VMs, so just disable all breakpoints */
    loadAllDisabledBreakpointState(cur_thread);
#endif
    word_t *guest_regs_ptr = vcpu->gp_registers;
    ଯଦି (vcpu->launched) {
        /* attempt to do a vmresume */
        asm ଅସ୍ଥିର(
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
#ifdef ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#ଅନ୍ୟଥା
            "leal kernel_stack_alloc + %c1, %%esp\n"
#endif
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(BIT(CONFIG_KERNEL_ଗଦା_BITS) - ଆକାର(word_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    } ଅନ୍ୟଥା {
        /* attempt to do a vmlaunch */
        asm ଅସ୍ଥିର(
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
#ifdef ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#ଅନ୍ୟଥା
            "leal kernel_stack_alloc + %c1, %%esp\n"
#endif
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(BIT(CONFIG_KERNEL_ଗଦା_BITS) - ଆକାର(word_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    }
    UNREACHABLE();
}
#endif

ଶୂନ୍ୟ NORETURN VISIBLE restore_user_context(ଶୂନ୍ୟ);
ଶୂନ୍ୟ NORETURN VISIBLE restore_user_context(ଶୂନ୍ୟ)
{
    c_exit_hook();

    ଗଣ୍ଠି_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(x86KSPendingInterrupt) != int_invalid) {
        /* put this in service */
        interrupt_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm ଅସ୍ଥିର(
            "mov %[stack_top], %%esp\n"
            "push %[syscall] \n"
            "push %[irq]\n"
            "call c_handle_interrupt"
            :
            : [ଗଦା_ଶୀର୍ଷ] "r"(&(kernel_ଗଦା_alloc[CURRENT_CPU_INDEX()][BIT(CONFIG_KERNEL_ଗଦା_BITS)])),
            [syscall] "r"(0), /* syscall is unused for irq path */
            [irq] "r"(irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = ଗଣ୍ଠି_STATE(ksCurThread);
#ifdef CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    ଯଦି (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_to_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } ଅନ୍ୟଥା ଯଦି (vcpu) {
        vcpu_fpu_to_host(cur_thread, vcpu);
    }
#endif
    set_Kernel_Entry_ଗଦା_Pointer(cur_thread);

#ifdef CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#endif

    ଯଦି (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    /* see if we entered via syscall */
    ଯଦି (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1)) {
        cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
        asm ଅସ୍ଥିର(
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
    } ଅନ୍ୟଥା {
        asm ଅସ୍ଥିର(
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
