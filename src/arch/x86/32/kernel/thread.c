/*
 * Copyright 2014, General Dynamics C4 Systems
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
    /* set PD */
    سېټ_VMRoot(tcb);
    إذا (config_سېټ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    إذا (config_سېټ(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_کوډ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سېټ_ثبتوونکی(tcb, FLAGS, FLAGS_USER_DEFAULT);
    سېټ_ثبتوونکی(tcb, بل_IP, (کلمه_t)&idle_thread);
    سېټ_ثبتوونکی(tcb, CS, SEL_CS_0);
    سېټ_ثبتوونکی(tcb, SS, SEL_DS_0);
    سېټ_ثبتوونکی(tcb, FS_بنسټ, 0);
    سېټ_ثبتوونکی(tcb, GS_بنسټ, 0);
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    /* Force the idle thread to run on kernel page table */
    سېټ_VMRoot(غوټه_STATE(ksIdleThread));
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ ډېرپېښ_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
