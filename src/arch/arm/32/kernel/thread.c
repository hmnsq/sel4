/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <object.h>
#شامل <machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/kernel/thread.h>
#شامل <linker.h>

تهی Arch_switchToThread(tcb_t *tcb)
{
    اگر (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    setVMRoot(tcb);
    clearExMonitor();
}

BOOT_CODE تهی Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
}

تهی Arch_switchToIdleThread(تهی)
{
    اگر (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(پوچ);
    }

    /* Force the idle thread to run on kernel page table */
    setVMRoot(گره_STATE(ksIdleThread));
}

تهی Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
