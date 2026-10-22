/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <object.h>
#অন্তর্ভাব <machine.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <arch/kernel/thread.h>
#অন্তর্ভাব <linker.h>

শূন্য Arch_switchToThread(tcb_t *tcb)
{
    /* set PD */
    setVMRoot(tcb);
#ifdef ENABLE_SMP_SUPPORT
    asm চঞ্চল("movq %[value], %%gs:%c[offset]"
                 :
                 : [মান] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [offset] "i"(OFFSETOF(গ্রন্থি_Info_t, currentThreadUserContext)));
#endif
    যদি (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    যদি (config_set(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_CODE শূন্য Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, FLAGS, FLAGS_USER_DEFAULT);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    /* We set the RSP to 0, even though the idle thread will never touch it, as it
     * allows us to distinguish an interrupt from the idle thread from an interrupt
     * from kernel execution, just by examining the saved RSP value (since the kernel
     * thread will have a valid RSP, and never 0). See traps.S for the other side of this
     */
    setRegister(tcb, RSP, 0);
}

শূন্য Arch_switchToIdleThread(শূন্য)
{
    tcb_t *tcb = গ্রন্থি_STATE(ksIdleThread);
    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
#ifdef ENABLE_SMP_SUPPORT
    asm চঞ্চল("movq %[value], %%gs:%c[offset]"
                 :
                 : [মান] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [offset] "i"(OFFSETOF(গ্রন্থি_Info_t, currentThreadUserContext)));
#endif
}

শূন্য Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

শূন্য Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Setting Error to 0 will force a return by the interrupt path, which
     * does a full restore. Unless we're the current thread, in which case
     * we still have to go back out via a sysret */
    যদি (tptr != গ্রন্থি_STATE(ksCurThread)) {
        setRegister(tptr, Error, 0);
    }
}
