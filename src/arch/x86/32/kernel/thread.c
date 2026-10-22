/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <object.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/kernel/thread.h>
#अन्तर्भाव <linker.h>

शून्य Arch_switch_गन्तव्यम्_Thread(tcb_t *tcb)
{
    /* set PD */
    समुच्चयः_VMRoot(tcb);
    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_संहिता शून्य Arch_configureIdleThread(tcb_t *tcb)
{
    समुच्चयः_पञ्जिकाकोशः(tcb, FLAGS, FLAGS_USER_DEFAULT);
    समुच्चयः_पञ्जिकाकोशः(tcb, अग्र_IP, (word_t)&idle_thread);
    समुच्चयः_पञ्जिकाकोशः(tcb, CS, SEL_CS_0);
    समुच्चयः_पञ्जिकाकोशः(tcb, SS, SEL_DS_0);
    समुच्चयः_पञ्जिकाकोशः(tcb, FS_आधारः, 0);
    समुच्चयः_पञ्जिकाकोशः(tcb, GS_आधारः, 0);
}

शून्य Arch_switch_गन्तव्यम्_Idle_Thread(शून्य)
{
    /* Force the idle thread to run on kernel page table */
    समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksIdleThread));
}

शून्य Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

शून्य बहुलकम्_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
