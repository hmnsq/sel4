/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_رمز خالی Arch_configureIdleThread(tcb_t *tcb)
{
    سیٹ_رجسٹر(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    سیٹ_رجسٹر(tcb, ELR_EL1, (لفظ_t)&idle_thread);
}

خالی Arch_switch_مقصد_Idle_Thread(خالی)
{
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(تہی);
    }
    سیٹ_Current_User_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

خالی Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
