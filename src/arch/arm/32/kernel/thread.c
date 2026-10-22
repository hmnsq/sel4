/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <object.h>
#समावेश <machine.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <arch/kernel/thread.h>
#समावेश <linker.h>

व्योम Arch_switchToThread(tcb_t *tcb)
{
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    setVMRoot(tcb);
    clearExMonitor();
}

BOOT_CODE व्योम Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, अग्र_IP, (word_t)&idle_thread);
}

व्योम Arch_switchToIdleThread(व्योम)
{
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(शून्य);
    }

    /* Force the idle thread to run on kernel page table */
    setVMRoot(गाँठ_STATE(ksIdleThread));
}

व्योम Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
