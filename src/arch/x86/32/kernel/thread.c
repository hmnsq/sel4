/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <object.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>

ਖਾਲੀ Arch_switch_ਮੰਜ਼ਿਲ_Thread(tcb_t *tcb)
{
    /* set PD */
    ਸੈੱਟ_VMRoot(tcb);
    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_ਕੋਡ ਖਾਲੀ Arch_configureIdleThread(tcb_t *tcb)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, FLAGS, FLAGS_USER_DEFAULT);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, ਅਗਲਾ_IP, (ਸ਼ਬਦ_t)&idle_thread);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, CS, SEL_CS_0);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, SS, SEL_DS_0);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, FS_ਅਧਾਰ, 0);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, GS_ਅਧਾਰ, 0);
}

ਖਾਲੀ Arch_switch_ਮੰਜ਼ਿਲ_Idle_Thread(ਖਾਲੀ)
{
    /* Force the idle thread to run on kernel page table */
    ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksIdleThread));
}

ਖਾਲੀ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ਖਾਲੀ ਬਹੁਲਕ_post_Modify_Registers(tcb_t *tptr)
{
    /* Don't need to do anything */
}
