/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/statedata.h>
#ضمن <kernel/stack.h>
#ضمن <machine/fpu.h>
#ضمن <arch/fastpath/fastpath.h>
#ضمن <arch/machine/debug.h>
#ضمن <benchmark/benchmark_track.h>
#ضمن <mode/stack.h>
#ضمن <arch/object/vcpu.h>
#ضمن <arch/kernel/traps.h>

#ضمن <api/syscall.h>
#ضمن <util.h>

#اگر_تعریف_شده CONFIG_VTX
استفاده_شده ساكن فراغ NORETURN vmlaunch_failed(فراغ)
{
    گره_قفل_SYS;

    c_مدخل_hook();

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
    کلمه_t *guest_regs_ptr = vcpu->gp_registers;
    إذا (vcpu->launched) {
        /* attempt to do a vmresume */
        asm متطاير(
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
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#وإلا
            "leal kernel_stack_alloc + %c1, %%esp\n"
#پایان_اگر
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(بیت(CONFIG_KERNEL_پشته_بیتها) - حجم(کلمه_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    } وإلا {
        /* attempt to do a vmlaunch */
        asm متطاير(
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
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
            "movl (%%esp), %%esp\n"
#وإلا
            "leal kernel_stack_alloc + %c1, %%esp\n"
#پایان_اگر
            "call vmlaunch_failed\n"
            : [guest_regs] "+a"(guest_regs_ptr)
            : "i"(بیت(CONFIG_KERNEL_پشته_بیتها) - حجم(کلمه_t))
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "ebx", "ecx", "edx", "esi", "edi", "ebp", "memory"
        );
    }
    UNREACHABLE();
}
#پایان_اگر

فراغ NORETURN VISIBLE restore_user_context(فراغ);
فراغ NORETURN VISIBLE restore_user_context(فراغ)
{
    c_exit_hook();

    گره_UNLOCK_IF_HELD;

    /* we've now 'exited' the kernel. If we have a pending interrupt
     * we should 'enter' it again */
    إذا (ARCH_گره_STATE(x86_KSPending_وقفه) != int_invalid) {
        /* put this in service */
        وقفه_t irq = servicePendingIRQ();
        /* reset our stack and jmp to the IRQ entry point */
        asm متطاير(
            "mov %[stack_top], %%esp\n"
            "push %[syscall] \n"
            "push %[irq]\n"
            "call c_handle_interrupt"
            :
            : [پشته_بالاترین] "r"(&(kernel_پشته_alloc[CURRENT_CPU_شاخص()][بیت(CONFIG_KERNEL_پشته_بیتها)])),
            [syscall] "r"(0), /* syscall is unused for irq path */
            [irq] "r"(irq)
            : "memory");
        UNREACHABLE();
    }

    tcb_t *cur_thread = گره_STATE(ksCurThread);
#اگر_تعریف_شده CONFIG_VTX
    vcpu_t *vcpu = cur_thread->tcbArch.tcbVCPU;
    إذا (thread_state_ptr_get_tsType(&cur_thread->tcbState) == ThreadState_RunningVM) {
        vcpu_fpu_مقصد_guest(cur_thread, vcpu);
        restore_vmx(cur_thread, vcpu);
    } وإلا إذا (vcpu) {
        vcpu_fpu_مقصد_host(cur_thread, vcpu);
    }
#پایان_اگر
    مجموعه_Kernel_مدخل_پشته_اشاره_گر(cur_thread);

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    restore_user_debug_context(cur_thread);
#پایان_اگر

    إذا (config_مجموعه(CONFIG_KERNEL_X86_IBRS_BASIC)) {
        x86_disable_ibrs();
    }

    /* see if we entered via syscall */
    إذا (likely(cur_thread->tcbArch.tcbContext.registers[Error] == -1)) {
        cur_thread->tcbArch.tcbContext.registers[FLAGS] &= ~FLAGS_IF;
        asm متطاير(
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
    } وإلا {
        asm متطاير(
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
