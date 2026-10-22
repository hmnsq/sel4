/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>

ਖਾਲੀ Arch_switch_ਮੰਜ਼ਿਲ_Thread(tcb_t *tcb)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(tcb->tcbArch.tcbVCPU);
    }

    ਸੈੱਟ_VMRoot(tcb);
    clearExMonitor();
}

BOOT_ਕੋਡ ਖਾਲੀ Arch_configureIdleThread(tcb_t *tcb)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, CPSR, CPSR_IDLETHREAD);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, ਅਗਲਾ_IP, (ਸ਼ਬਦ_t)&idle_thread);
}

ਖਾਲੀ Arch_switch_ਮੰਜ਼ਿਲ_Idle_Thread(ਖਾਲੀ)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(ਨਿਰਾ);
    }

    /* Force the idle thread to run on kernel page table */
    ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksIdleThread));
}

ਖਾਲੀ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
