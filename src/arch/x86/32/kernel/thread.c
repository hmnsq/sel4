/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <object.h>
#ଅନ୍ତର୍ଭାଵ <machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>

ଶୂନ୍ୟ Arch_switchToThread(tcb_t *tcb)
{
    /* set PD */
    setVMRoot(tcb);
    ଯଦି (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    ଯଦି (config_set(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_CODE ଶୂନ୍ୟ Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, FLAGS, FLAGS_USER_DEFAULT);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    setRegister(tcb, FS_BASE, 0);
    setRegister(tcb, GS_BASE, 0);
}

ଶୂନ୍ୟ Arch_switchToIdleThread(ଶୂନ୍ୟ)
{
    /* Force the idle thread to run on kernel page table */
    setVMRoot(ଗଣ୍ଠି_STATE(ksIdleThread));
}

ଶୂନ୍ୟ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ଶୂନ୍ୟ Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Don't need to do anything */
}
