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
    سيٽ_VMRoot(tcb);
    إذا (config_سيٽ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    إذا (config_سيٽ(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_ڪوڊ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سيٽ_رجسٽر(tcb, FLAGS, FLAGS_USER_DEFAULT);
    سيٽ_رجسٽر(tcb, ايندڙ_IP, (لفظ_t)&idle_thread);
    سيٽ_رجسٽر(tcb, CS, SEL_CS_0);
    سيٽ_رجسٽر(tcb, SS, SEL_DS_0);
    سيٽ_رجسٽر(tcb, FS_بنياد, 0);
    سيٽ_رجسٽر(tcb, GS_بنياد, 0);
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    /* Force the idle thread to run on kernel page table */
    سيٽ_VMRoot(ڳنڍ_STATE(ksIdleThread));
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ گهڻائي_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
