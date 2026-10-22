/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/stack.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/fastpath/fastpath.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/stack.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/traps.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
ਵਰਤਿਆ ਸਥਿਰ ਖਾਲੀ NORETURN vmlaunch_failed(ਖਾਲੀ)
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();

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
    ਸ਼ਬਦ_t *guest_regs_ptr = vcpu->gp_registers;
    ਜੇ (vcpu->launched) {
        /* attempt to do a vmresume */
        asm ਅਸਥਿਰ(
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#ਨਹੀਂਤਾਂ
            "leal kernel_stack_alloc + %c1, %%esp\n"
#ਜੇ_ਅੰਤ
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ) - ਆਕਾਰ(ਸ਼ਬਦ_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    } ਨਹੀਂਤਾਂ {
        /* attempt to do a vmlaunch */
        asm ਅਸਥਿਰ(
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#ਨਹੀਂਤਾਂ
            "leal kernel_stack_alloc + %c1, %%esp\n"
#ਜੇ_ਅੰਤ
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ) - ਆਕਾਰ(ਸ਼ਬਦ_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    }
    UNREACHABLE();
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ NORETURN VISIBLE restore_user_context(ਖਾਲੀ);
ਖਾਲੀ NORETURN VISIBLE restore_user_context(ਖਾਲੀ)
{
    c_exit_hook();

    ਗੰਢ_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    ਜੇ (ARCH_ਗੰਢ_STATE(x86_KSPending_ਰੁਕਾਵਟ) != int_invalid) {
        /* put this in service */
        ਰੁਕਾਵਟ_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm ਅਸਥਿਰ(
            "mov %[stack_top], %%esp\n"
            "push %[syscall] \n"
            "push %[irq]\n"
            "call c_handle_interrupt"
            :
            : [ਢੇਰ_ਸਿਖਰ] "r"(&(kernel_ਢੇਰ_alloc[CURRENT_CPU_ਸੂਚਕਾਂਕ()][ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ)])),
            [syscall] "r"(0), /* syscall is unused for irq path */
            [irq] "r"(irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = ਗੰਢ_STATE(ksCurThread);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    ਜੇ (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_ਮੰਜ਼ਿਲ_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } ਨਹੀਂਤਾਂ ਜੇ (vcpu) {
        vcpu_fpu_ਮੰਜ਼ਿਲ_host(cur_thread, vcpu);
    }
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_Kernel_ਇੰਦਰਾਜ_ਢੇਰ_ਸੂਚਕ(cur_thread);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#ਜੇ_ਅੰਤ

    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    /* see if we entered via syscall */
    ਜੇ (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1)) {
        cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
        asm ਅਸਥਿਰ(
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
    } ਨਹੀਂਤਾਂ {
        asm ਅਸਥਿਰ(
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
