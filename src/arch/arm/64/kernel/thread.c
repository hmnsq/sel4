/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_CODE શૂન્ય Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    setRegister(tcb, ELR_EL1, (word_t)&idle_thread);
}

શૂન્ય Arch_switchToIdleThread(શૂન્ય)
{
    જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ખાલી);
    }
    set_Current_User_VSpace_મૂળ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

શૂન્ય Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
