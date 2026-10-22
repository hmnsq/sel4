/*
 * Copyright 2014, General Dynamics C4 Systems
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
    setRegister(tcb, FS_BASE, 0);
    setRegister(tcb, GS_BASE, 0);
}

व्योम Arch_switchToIdleThread(व्योम)
{
    /* Force the idle thread to run on kernel page table */
    setVMRoot(गाँठ_STATE(ksIdleThread));
}

व्योम Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

व्योम Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Don't need to do anything */
}
