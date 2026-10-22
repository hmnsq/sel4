/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <object.h>
#شامل <machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/kernel/thread.h>
#شامل <linker.h>

خالی Arch_switch_مقصد_Thread(tcb_t *tcb)
{
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    سیٹ_VMRoot(tcb);
    clearExMonitor();
}

BOOT_رمز خالی Arch_configureIdleThread(tcb_t *tcb)
{
    سیٹ_رجسٹر(tcb, CPSR, CPSR_IDLETHREAD);
    سیٹ_رجسٹر(tcb, اگلا_IP, (لفظ_t)&idle_thread);
}

خالی Arch_switch_مقصد_Idle_Thread(خالی)
{
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(تہی);
    }

    /* Force the idle thread to run on kernel page table */
    سیٹ_VMRoot(گرہ_STATE(ksIdleThread));
}

خالی Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
