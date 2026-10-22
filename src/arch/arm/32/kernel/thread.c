/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <object.h>
#ضمن <machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/kernel/thread.h>
#ضمن <linker.h>

فراغ Arch_switch_منزل_Thread(tcb_t *tcb)
{
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    سېټ_VMRoot(tcb);
    clearExMonitor();
}

BOOT_کوډ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سېټ_ثبتوونکی(tcb, CPSR, CPSR_IDLETHREAD);
    سېټ_ثبتوونکی(tcb, بل_IP, (کلمه_t)&idle_thread);
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(عدم);
    }

    /* Force the idle thread to run on kernel page table */
    سېټ_VMRoot(غوټه_STATE(ksIdleThread));
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
