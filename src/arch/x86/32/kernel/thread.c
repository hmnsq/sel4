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
    سیٹ_VMRoot(tcb);
    إذا (config_سیٹ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    إذا (config_سیٹ(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_کوڈ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سیٹ_رجسٹر(tcb, FLAGS, FLAGS_USER_DEFAULT);
    سیٹ_رجسٹر(tcb, اگلا_IP, (لفظ_t)&idle_thread);
    سیٹ_رجسٹر(tcb, CS, SEL_CS_0);
    سیٹ_رجسٹر(tcb, SS, SEL_DS_0);
    سیٹ_رجسٹر(tcb, FS_ادھار, 0);
    سیٹ_رجسٹر(tcb, GS_ادھار, 0);
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    /* Force the idle thread to run on kernel page table */
    سیٹ_VMRoot(گرہ_STATE(ksIdleThread));
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ منوال_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
