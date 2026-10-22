/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_సంకేతం శూన్య Arch_configureIdleThread(tcb_t *tcb)
{
    సమితి_రిజిస్టరు(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    సమితి_రిజిస్టరు(tcb, ELR_EL1, (మాట_t)&idle_thread);
}

శూన్య Arch_switch_గమ్యం_Idle_Thread(శూన్య)
{
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ఖాళీ);
    }
    సమితి_Current_User_VSpace_మూలం(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

శూన్య Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
