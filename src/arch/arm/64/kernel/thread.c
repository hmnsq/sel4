/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_CODE व्योम Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    setRegister(tcb, ELR_EL1, (word_t)&idle_thread);
}

व्योम Arch_switchToIdleThread(व्योम)
{
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(शून्य);
    }
    set_Current_User_VSpace_मूल(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

व्योम Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
