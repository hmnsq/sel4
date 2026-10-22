/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <object.h>
#समावेश <machine.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <arch/kernel/thread.h>
#समावेश <linker.h>

व्योम Arch_switchToThread(tcb_t *tcb)
{
    /* set PD */
    setVMRoot(tcb);
#अगर_घोषित ENABLE_SMP_SUPPORT
    यंत्र अस्थिर("movq %[value], %%gs:%c[offset]"
                 :
                 : [मूल्य] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [offset] "i"(OFFSETOF(गाँठ_Info_t, currentThreadUserContext)));
#पूर्ण_अगर
    अगर (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    अगर (config_set(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_CODE व्योम Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, FLAGS, FLAGS_USER_DEFAULT);
    setRegister(tcb, अग्र_IP, (word_t)&idle_thread);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    /* We set the RSP to 0, even though the idle thread will never touch it, as it
     * allows us to distinguish an interrupt from the idle thread from an interrupt
     * from kernel execution, just by examining the saved RSP value (since the kernel
     * thread will have a valid RSP, and never 0). See traps.S for the other side of this
     */
    setRegister(tcb, RSP, 0);
}

व्योम Arch_switchToIdleThread(व्योम)
{
    tcb_t *tcb = गाँठ_STATE(ksIdleThread);
    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
#अगर_घोषित ENABLE_SMP_SUPPORT
    यंत्र अस्थिर("movq %[value], %%gs:%c[offset]"
                 :
                 : [मूल्य] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [offset] "i"(OFFSETOF(गाँठ_Info_t, currentThreadUserContext)));
#पूर्ण_अगर
}

व्योम Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

व्योम Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Setting Error to 0 will force a return by the interrupt path, which
     * does a full restore. Unless we're the current thread, in which case
     * we still have to go back out via a sysret */
    अगर (tptr != गाँठ_STATE(ksCurThread)) {
        setRegister(tptr, Error, 0);
    }
}
