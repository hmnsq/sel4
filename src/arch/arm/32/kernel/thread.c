/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <linker.h>

ಶೂನ್ಯ Arch_switchToThread(tcb_t *tcb)
{
    ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    setVMRoot(tcb);
    clearExMonitor();
}

BOOT_CODE ಶೂನ್ಯ Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
}

ಶೂನ್ಯ Arch_switchToIdleThread(ಶೂನ್ಯ)
{
    ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ರಿಕ್ತ);
    }

    /* Force the idle thread to run on kernel page table */
    setVMRoot(ಗಂಟು_STATE(ksIdleThread));
}

ಶೂನ್ಯ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
