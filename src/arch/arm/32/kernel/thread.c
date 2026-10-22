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
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    سیٹ_VMRoot(tcb);
    clearExMonitor();
}

BOOT_کوڈ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سیٹ_رجسٹر(tcb, CPSR, CPSR_IDLETHREAD);
    سیٹ_رجسٹر(tcb, اگلا_IP, (لفظ_t)&idle_thread);
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(عدم);
    }

    /* Force the idle thread to run on kernel page table */
    سیٹ_VMRoot(گرہ_STATE(ksIdleThread));
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
