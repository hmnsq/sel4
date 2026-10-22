/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_CODE ശൂന്യം Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    setRegister(tcb, ELR_EL1, (word_t)&idle_thread);
}

ശൂന്യം Arch_switchToIdleThread(ശൂന്യം)
{
    എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ശൂന്യ);
    }
    set_Current_User_VSpace_വേര്(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

ശൂന്യം Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
