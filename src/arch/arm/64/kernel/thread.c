/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <linker.h>

ಶೂನ್ಯ Arch_switch_ಗಮ್ಯ_Thread(tcb_t *tcb)
{
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }
    ಗಣ_VMRoot(tcb);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ Arch_configureIdleThread(tcb_t *tcb)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, ELR_EL1, (ಶಬ್ದ_t)&idle_thread);
}

ಶೂನ್ಯ Arch_switch_ಗಮ್ಯ_Idle_Thread(ಶೂನ್ಯ)
{
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ಖಾಲಿ);
    }
    ಗಣ_Current_User_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

ಶೂನ್ಯ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
