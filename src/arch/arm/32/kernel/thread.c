/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <object.h>
#అన్తర్భావ <machine.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/kernel/thread.h>
#అన్తర్భావ <linker.h>

శూన్య Arch_switch_గమ్యం_Thread(tcb_t *tcb)
{
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    సమితి_VMRoot(tcb);
    clearExMonitor();
}

BOOT_సంకేతం శూన్య Arch_configureIdleThread(tcb_t *tcb)
{
    సమితి_రిజిస్టరు(tcb, CPSR, CPSR_IDLETHREAD);
    సమితి_రిజిస్టరు(tcb, తదుపరి_IP, (మాట_t)&idle_thread);
}

శూన్య Arch_switch_గమ్యం_Idle_Thread(శూన్య)
{
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ఖాళీ);
    }

    /* Force the idle thread to run on kernel page table */
    సమితి_VMRoot(కణుపు_STATE(ksIdleThread));
}

శూన్య Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
