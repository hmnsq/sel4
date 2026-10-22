/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <object.h>
#অন্তর্ভাব <machine.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <arch/kernel/thread.h>
#অন্তর্ভাব <linker.h>

শূন্য Arch_switchToThread(tcb_t *tcb)
{
    /* set PD */
    setVMRoot(tcb);
    যদি (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    যদি (config_set(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_CODE শূন্য Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, FLAGS, FLAGS_USER_DEFAULT);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    setRegister(tcb, FS_BASE, 0);
    setRegister(tcb, GS_BASE, 0);
}

শূন্য Arch_switchToIdleThread(শূন্য)
{
    /* Force the idle thread to run on kernel page table */
    setVMRoot(গ্রন্থি_STATE(ksIdleThread));
}

শূন্য Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

শূন্য Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Don't need to do anything */
}
