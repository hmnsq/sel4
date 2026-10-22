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

تهی Arch_switchToThread(tcb_t *tcb)
{
    /* set PD */
    setVMRoot(tcb);
    اگر (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    اگر (config_set(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_CODE تهی Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, FLAGS, FLAGS_USER_DEFAULT);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    setRegister(tcb, FS_BASE, 0);
    setRegister(tcb, GS_BASE, 0);
}

تهی Arch_switchToIdleThread(تهی)
{
    /* Force the idle thread to run on kernel page table */
    setVMRoot(گره_STATE(ksIdleThread));
}

تهی Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

تهی Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Don't need to do anything */
}
