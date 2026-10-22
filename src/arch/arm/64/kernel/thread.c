/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

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
}

BOOT_کوڈ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سیٹ_رجسٹر(tcb, SPSR_EL1, PSTATE_IDLETHREAD);
    سیٹ_رجسٹر(tcb, ELR_EL1, (لفظ_t)&idle_thread);
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_switch(عدم);
    }
    سیٹ_Current_User_VSpace_جڑ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
