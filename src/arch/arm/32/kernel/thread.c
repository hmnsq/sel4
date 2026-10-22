/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <object.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/kernel/thread.h>
#अन्तर्भाव <linker.h>

शून्य Arch_switch_गन्तव्यम्_Thread(tcb_t *tcb)
{
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    समुच्चयः_VMRoot(tcb);
    clearExMonitor();
}

BOOT_संहिता शून्य Arch_configureIdleThread(tcb_t *tcb)
{
    समुच्चयः_पञ्जिकाकोशः(tcb, CPSR, CPSR_IDLETHREAD);
    समुच्चयः_पञ्जिकाकोशः(tcb, अग्र_IP, (word_t)&idle_thread);
}

शून्य Arch_switch_गन्तव्यम्_Idle_Thread(शून्य)
{
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(रिक्त);
    }

    /* Force the idle thread to run on kernel page table */
    समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksIdleThread));
}

शून्य Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
