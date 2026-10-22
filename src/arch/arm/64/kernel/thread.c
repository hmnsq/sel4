/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_CODE வெறுமை Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    setRegister(tcb, ELR_EL1, (word_t)&idle_thread);
}

வெறுமை Arch_switchToIdleThread(வெறுமை)
{
    எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(வெற்று);
    }
    set_Current_User_VSpace_வேர்(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

வெறுமை Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
