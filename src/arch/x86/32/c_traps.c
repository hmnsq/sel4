/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <kernel/stack.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/fastpath/fastpath.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <mode/stack.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/traps.h>

#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <util.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
ಬಳಸಿದ ಸ್ಥಿರ ಶೂನ್ಯ NORETURN vmlaunch_failed(ಶೂನ್ಯ)
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();

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
    ಶಬ್ದ_t *guest_regs_ptr = vcpu->gp_registers;
    ಆದರೆ (vcpu->launched) {
        /* attempt to do a vmresume */
        asm ಅಸ್ಥಿರ(
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#ಇಲ್ಲದಿದ್ದರೆ
            "leal kernel_stack_alloc + %c1, %%esp\n"
#ಆದರೆ_ಅಂತ್ಯ
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು) - ಗಾತ್ರ(ಶಬ್ದ_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* attempt to do a vmlaunch */
        asm ಅಸ್ಥಿರ(
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#ಇಲ್ಲದಿದ್ದರೆ
            "leal kernel_stack_alloc + %c1, %%esp\n"
#ಆದರೆ_ಅಂತ್ಯ
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು) - ಗಾತ್ರ(ಶಬ್ದ_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    }
    UNREACHABLE();
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ NORETURN VISIBLE restore_user_context(ಶೂನ್ಯ);
ಶೂನ್ಯ NORETURN VISIBLE restore_user_context(ಶೂನ್ಯ)
{
    c_exit_hook();

    ಗಂಟು_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    ಆದರೆ (ARCH_ಗಂಟು_STATE(x86_KSPending_ಅಡಚಣೆ) != int_invalid) {
        /* put this in service */
        ಅಡಚಣೆ_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm ಅಸ್ಥಿರ(
            "mov %[stack_top], %%esp\n"
            "push %[syscall] \n"
            "push %[irq]\n"
            "call c_handle_interrupt"
            :
            : [ರಾಶಿ_ತುದಿ] "r"(&(kernel_ರಾಶಿ_alloc[CURRENT_CPU_ಸೂಚ್ಯಂಕ()][ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು)])),
            [syscall] "r"(0), /* syscall is unused for irq path */
            [irq] "r"(irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = ಗಂಟು_STATE(ksCurThread);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    ಆದರೆ (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_ಗಮ್ಯ_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vcpu) {
        vcpu_fpu_ಗಮ್ಯ_host(cur_thread, vcpu);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_Kernel_ನಮೂದು_ರಾಶಿ_ಸೂಚಕ(cur_thread);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    /* see if we entered via syscall */
    ಆದರೆ (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1)) {
        cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
        asm ಅಸ್ಥಿರ(
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
    } ಇಲ್ಲದಿದ್ದರೆ {
        asm ಅಸ್ಥಿರ(
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
