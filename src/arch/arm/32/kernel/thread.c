/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <object.h>
#অন্তর্ভাব <machine.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <arch/kernel/thread.h>
#অন্তর্ভাব <linker.h>

শূন্য Arch_switchToThread(tcb_t *tcb)
{
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    setVMRoot(tcb);
    clearExMonitor();
}

BOOT_CODE শূন্য Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, CPSR, CPSR_IDLETHREAD);
    setRegister(tcb, পরবর্তী_IP, (word_t)&idle_thread);
}

শূন্য Arch_switchToIdleThread(শূন্য)
{
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ফাঁকা);
    }

    /* Force the idle thread to run on kernel page table */
    setVMRoot(গ্রন্থি_STATE(ksIdleThread));
}

শূন্য Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
