/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <kernel/stack.h>
#অন্তর্ভাব <machine/fpu.h>
#অন্তর্ভাব <arch/fastpath/fastpath.h>
#অন্তর্ভাব <arch/machine/debug.h>
#অন্তর্ভাব <benchmark/benchmark_track.h>
#অন্তর্ভাব <mode/stack.h>
#অন্তর্ভাব <arch/object/vcpu.h>
#অন্তর্ভাব <arch/kernel/traps.h>

#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <util.h>

#ifdef CONFIG_VTX
USED স্থির শূন্য NORETURN vmlaunch_failed(শূন্য)
{
    গ্রন্থি_LOCK_SYS;

    c_entry_hook();

    handleVmEntryFail();
    restore_user_context();
}

স্থির শূন্য NORETURN restore_vmx(tcb_t *cur_thread, vcpu_t *vcpu)
{
    restoreVMCS();
#ifdef CONFIG_HARDWARE_DEBUG_API
    /* Do not support breakpoints in VMs, so just disable all breakpoints */
    loadAllDisabledBreakpointState(cur_thread);
#endif
    word_t *guest_regs_ptr = vcpu->gp_registers;
    যদি (vcpu->launched) {
        /* attempt to do a vmresume */
        asm চঞ্চল(
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
#অন্যথা
            "leal kernel_stack_alloc + %c1, %%esp\n"
#endif
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(BIT(CONFIG_KERNEL_স্তূপ_BITS) - পরিমাণ(word_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    } অন্যথা {
        /* attempt to do a vmlaunch */
        asm চঞ্চল(
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
#অন্যথা
            "leal kernel_stack_alloc + %c1, %%esp\n"
#endif
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(BIT(CONFIG_KERNEL_স্তূপ_BITS) - পরিমাণ(word_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    }
    UNREACHABLE();
}
#endif

শূন্য NORETURN VISIBLE restore_user_context(শূন্য);
শূন্য NORETURN VISIBLE restore_user_context(শূন্য)
{
    c_exit_hook();

    গ্রন্থি_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    যদি (ARCH_গ্রন্থি_STATE(x86KSPendingInterrupt) != int_invalid) {
        /* put this in service */
        interrupt_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm চঞ্চল(
            "mov %[stack_top], %%esp\n"
            "push %[syscall] \n"
            "push %[irq]\n"
            "call c_handle_interrupt"
            :
            : [স্তূপ_শীর্ষ] "r"(&(kernel_স্তূপ_alloc[CURRENT_CPU_INDEX()][BIT(CONFIG_KERNEL_স্তূপ_BITS)])),
            [syscall] "r"(0), /* syscall is unused for irq path */
            [irq] "r"(irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = গ্রন্থি_STATE(ksCurThread);
#ifdef CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    যদি (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_to_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } অন্যথা যদি (vcpu) {
        vcpu_fpu_to_host(cur_thread, vcpu);
    }
#endif
    set_Kernel_Entry_স্তূপ_Pointer(cur_thread);

#ifdef CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#endif

    যদি (config_set(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    /* see if we entered via syscall */
    যদি (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1)) {
        cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
        asm চঞ্চল(
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
    } অন্যথা {
        asm চঞ্চল(
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
