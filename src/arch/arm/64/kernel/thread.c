/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <object.h>
#ضمن <machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/kernel/thread.h>
#ضمن <linker.h>

فراغ Arch_switchToThread(tcb_t *tcb)
{
    𐤀𐤌 (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }
    setVMRoot(tcb);
}

BOOT_CODE فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    setRegister(tcb, ELR_EL1, (word_t)&idle_thread);
}

فراغ Arch_switchToIdleThread(فراغ)
{
    𐤀𐤌 (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(عدم);
    }
    setCurrentUserVSpaceRoot(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
