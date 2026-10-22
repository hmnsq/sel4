/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <kernel/stack.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/fastpath/fastpath.h>
#अन्तर्भाव <arch/machine/debug.h>
#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <mode/stack.h>
#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <arch/kernel/traps.h>

#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <util.h>

#यदि_परिभाषितम् CONFIG_VTX
प्रयुक्तम् स्थिर शून्य NORETURN vmlaunch_failed(शून्य)
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();

    handle_Vm_प्रविष्टिः_Fail();
    restore_user_context();
}

स्थिर शून्य NORETURN restore_vmx(tcb_t *cur_thread, vcpu_t *vcpu)
{
    restoreVMCS();
#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    /* Do not support breakpoints in VMs, so just disable all breakpoints */
    आरोपय_All_Disabled_Breakpoint_State(cur_thread);
#यदि_अन्तः
    word_t *guest_regs_ptr = vcpu->gp_registers;
    यदि (vcpu->launched) {
        /* attempt to do a vmresume */
        asm चञ्चल(
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
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#अन्यथा
            "leal kernel_stack_alloc + %c1, %%esp\n"
#यदि_अन्तः
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(BIT(CONFIG_KERNEL_राशिः_द्वयङ्काः) - परिमाण(word_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    } अन्यथा {
        /* attempt to do a vmlaunch */
        asm चञ्चल(
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
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#अन्यथा
            "leal kernel_stack_alloc + %c1, %%esp\n"
#यदि_अन्तः
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(BIT(CONFIG_KERNEL_राशिः_द्वयङ्काः) - परिमाण(word_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    }
    UNREACHABLE();
}
#यदि_अन्तः

शून्य NORETURN VISIBLE restore_user_context(शून्य);
शून्य NORETURN VISIBLE restore_user_context(शून्य)
{
    c_exit_hook();

    ग्रन्थिः_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    यदि (ARCH_ग्रन्थिः_STATE(x86_KSPending_व्यवधानम्) != int_invalid) {
        /* put this in service */
        व्यवधानम्_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm चञ्चल(
            "mov %[stack_top], %%esp\n"
            "push %[syscall] \n"
            "push %[irq]\n"
            "call c_handle_interrupt"
            :
            : [राशिः_शिखरम्] "r"(&(kernel_राशिः_alloc[CURRENT_CPU_सूचकांक()][BIT(CONFIG_KERNEL_राशिः_द्वयङ्काः)])),
            [syscall] "r"(0), /* syscall is unused for irq path */
            [irq] "r"(irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = ग्रन्थिः_STATE(ksCurThread);
#यदि_परिभाषितम् CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    यदि (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_गन्तव्यम्_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } अन्यथा यदि (vcpu) {
        vcpu_fpu_गन्तव्यम्_host(cur_thread, vcpu);
    }
#यदि_अन्तः
    समुच्चयः_Kernel_प्रविष्टिः_राशिः_Pointer(cur_thread);

#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#यदि_अन्तः

    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    /* see if we entered via syscall */
    यदि (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1)) {
        cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
        asm चञ्चल(
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
    } अन्यथा {
        asm चञ्चल(
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
