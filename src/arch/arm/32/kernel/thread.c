/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <object.h>
#அந்தர்பாவ <machine.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <arch/kernel/vspace.h>
#அந்தர்பாவ <arch/kernel/thread.h>
#அந்தர்பாவ <linker.h>

வெறுமை Arch_switchToThread(tcb_t *tcb)
{
    எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    setVMRoot(tcb);
    clearExMonitor();
}

BOOT_CODE வெறுமை Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
}

வெறுமை Arch_switchToIdleThread(வெறுமை)
{
    எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(வெற்று);
    }

    /* Force the idle thread to run on kernel page table */
    setVMRoot(முனை_STATE(ksIdleThread));
}

வெறுமை Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
