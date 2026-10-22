/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_CODE تهی Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    setRegister(tcb, ELR_EL1, (word_t)&idle_thread);
}

تهی Arch_switchToIdleThread(تهی)
{
    اگر (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(پوچ);
    }
    set_Current_User_VSpace_ریشه(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

تهی Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
