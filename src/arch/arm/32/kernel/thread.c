/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <object.h>
#અન્તર્ભાવ <machine.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <arch/kernel/thread.h>
#અન્તર્ભાવ <linker.h>

શૂન્ય Arch_switchToThread(tcb_t *tcb)
{
    જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    setVMRoot(tcb);
    clearExMonitor();
}

BOOT_CODE શૂન્ય Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
}

શૂન્ય Arch_switchToIdleThread(શૂન્ય)
{
    જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ખાલી);
    }

    /* Force the idle thread to run on kernel page table */
    setVMRoot(ગાંઠ_STATE(ksIdleThread));
}

શૂન્ય Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
