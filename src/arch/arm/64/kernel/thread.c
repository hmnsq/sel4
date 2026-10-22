/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <object.h>
#ଅନ୍ତର୍ଭାଵ <machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>

ଶୂନ୍ୟ Arch_switchToThread(tcb_t *tcb)
{
    ଯଦି (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }
    setVMRoot(tcb);
}

BOOT_CODE ଶୂନ୍ୟ Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    setRegister(tcb, ELR_EL1, (word_t)&idle_thread);
}

ଶୂନ୍ୟ Arch_switchToIdleThread(ଶୂନ୍ୟ)
{
    ଯଦି (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ଖାଲି);
    }
    set_Current_User_VSpace_ମୂଳ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

ଶୂନ୍ୟ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
