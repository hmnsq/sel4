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

فراغ Arch_switch_مقصد_Thread(tcb_t *tcb)
{
    /* set PD */
    مجموعه_VMRoot(tcb);
    إذا (config_مجموعه(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    إذا (config_مجموعه(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_کد فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    مجموعه_ثبات(tcb, FLAGS, FLAGS_USER_DEFAULT);
    مجموعه_ثبات(tcb, بعدی_IP, (کلمه_t)&idle_thread);
    مجموعه_ثبات(tcb, CS, SEL_CS_0);
    مجموعه_ثبات(tcb, SS, SEL_DS_0);
    مجموعه_ثبات(tcb, FS_پایه, 0);
    مجموعه_ثبات(tcb, GS_پایه, 0);
}

فراغ Arch_switch_مقصد_Idle_Thread(فراغ)
{
    /* Force the idle thread to run on kernel page table */
    مجموعه_VMRoot(گره_STATE(ksIdleThread));
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ نما_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
