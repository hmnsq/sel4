/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <object.h>
#അന്തര്ഭാവ <machine.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <arch/kernel/vspace.h>
#അന്തര്ഭാവ <arch/kernel/thread.h>
#അന്തര്ഭാവ <linker.h>

ശൂന്യം Arch_switchToThread(tcb_t *tcb)
{
    എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    setVMRoot(tcb);
    clearExMonitor();
}

BOOT_CODE ശൂന്യം Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
}

ശൂന്യം Arch_switchToIdleThread(ശൂന്യം)
{
    എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ശൂന്യ);
    }

    /* Force the idle thread to run on kernel page table */
    setVMRoot(കണ്ണി_STATE(ksIdleThread));
}

ശൂന്യം Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
