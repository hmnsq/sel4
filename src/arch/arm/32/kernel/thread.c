/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
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
    clearExMonitor();
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ Arch_configureIdleThread(tcb_t *tcb)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, CPSR, CPSR_IDLETHREAD);
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, ಮುಂದಿನ_IP, (ಶಬ್ದ_t)&idle_thread);
}

ಶೂನ್ಯ Arch_switch_ಗಮ್ಯ_Idle_Thread(ಶೂನ್ಯ)
{
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ಖಾಲಿ);
    }

    /* Force the idle thread to run on kernel page table */
    ಗಣ_VMRoot(ಗಂಟು_STATE(ksIdleThread));
}

ಶೂನ್ಯ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
