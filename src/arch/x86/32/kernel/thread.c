/*
 * Copyright 2014, General Dynamics C4 Systems
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
    /* set PD */
    సమితి_VMRoot(tcb);
    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    ఒకవేళ (config_సమితి(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_సంకేతం శూన్య Arch_configureIdleThread(tcb_t *tcb)
{
    సమితి_రిజిస్టరు(tcb, FLAGS, FLAGS_USER_DEFAULT);
    సమితి_రిజిస్టరు(tcb, తదుపరి_IP, (మాట_t)&idle_thread);
    సమితి_రిజిస్టరు(tcb, CS, SEL_CS_0);
    సమితి_రిజిస్టరు(tcb, SS, SEL_DS_0);
    సమితి_రిజిస్టరు(tcb, FS_పునాది, 0);
    సమితి_రిజిస్టరు(tcb, GS_పునాది, 0);
}

శూన్య Arch_switch_గమ్యం_Idle_Thread(శూన్య)
{
    /* Force the idle thread to run on kernel page table */
    సమితి_VMRoot(కణుపు_STATE(ksIdleThread));
}

శూన్య Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

శూన్య బహుళకం_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
