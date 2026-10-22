/*
 * Copyright 2014, General Dynamics C4 Systems
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
    /* set PD */
    ಗಣ_VMRoot(tcb);
    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ Arch_configureIdleThread(tcb_t *tcb)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, FLAGS, FLAGS_USER_DEFAULT);
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, ಮುಂದಿನ_IP, (ಶಬ್ದ_t)&idle_thread);
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, CS, SEL_CS_0);
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, SS, SEL_DS_0);
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, FS_ತಳ, 0);
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, GS_ತಳ, 0);
}

ಶೂನ್ಯ Arch_switch_ಗಮ್ಯ_Idle_Thread(ಶೂನ್ಯ)
{
    /* Force the idle thread to run on kernel page table */
    ಗಣ_VMRoot(ಗಂಟು_STATE(ksIdleThread));
}

ಶೂನ್ಯ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ಶೂನ್ಯ ಬಹುಲಕ_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
