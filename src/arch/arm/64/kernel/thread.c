/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_CODE শূন্য Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    setRegister(tcb, ELR_EL1, (word_t)&idle_thread);
}

শূন্য Arch_switchToIdleThread(শূন্য)
{
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(রিক্ত);
    }
    set_Current_User_VSpace_মূল(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

শূন্য Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
