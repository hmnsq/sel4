/*
 * Copyright 2014, General Dynamics C4 Systems
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
    /* set PD */
    سیٹ_VMRoot(tcb);
    اگر (config_سیٹ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    اگر (config_سیٹ(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_رمز خالی Arch_configureIdleThread(tcb_t *tcb)
{
    سیٹ_رجسٹر(tcb, FLAGS, FLAGS_USER_DEFAULT);
    سیٹ_رجسٹر(tcb, اگلا_IP, (لفظ_t)&idle_thread);
    سیٹ_رجسٹر(tcb, CS, SEL_CS_0);
    سیٹ_رجسٹر(tcb, SS, SEL_DS_0);
    سیٹ_رجسٹر(tcb, FS_بنیاد, 0);
    سیٹ_رجسٹر(tcb, GS_بنیاد, 0);
}

خالی Arch_switch_مقصد_Idle_Thread(خالی)
{
    /* Force the idle thread to run on kernel page table */
    سیٹ_VMRoot(گرہ_STATE(ksIdleThread));
}

خالی Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

خالی منوال_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
