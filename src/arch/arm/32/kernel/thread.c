/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <object.h>
#ଅନ୍ତର୍ଭାଵ <machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>

ଶୂନ୍ୟ Arch_switchToThread(tcb_t *tcb)
{
    ଯଦି (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    setVMRoot(tcb);
    clearExMonitor();
}

BOOT_CODE ଶୂନ୍ୟ Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
}

ଶୂନ୍ୟ Arch_switchToIdleThread(ଶୂନ୍ୟ)
{
    ଯଦି (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ଖାଲି);
    }

    /* Force the idle thread to run on kernel page table */
    setVMRoot(ଗଣ୍ଠି_STATE(ksIdleThread));
}

ଶୂନ୍ୟ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
