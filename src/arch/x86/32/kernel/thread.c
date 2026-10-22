/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <object.h>
#അന്തര്ഭാവ <machine.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <arch/kernel/vspace.h>
#അന്തര്ഭാവ <arch/kernel/thread.h>
#അന്തര്ഭാവ <linker.h>

ശൂന്യം Arch_switchToThread(tcb_t *tcb)
{
    /* set PD */
    setVMRoot(tcb);
    എങ്കിൽ (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    എങ്കിൽ (config_set(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_CODE ശൂന്യം Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, FLAGS, FLAGS_USER_DEFAULT);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    setRegister(tcb, FS_BASE, 0);
    setRegister(tcb, GS_BASE, 0);
}

ശൂന്യം Arch_switchToIdleThread(ശൂന്യം)
{
    /* Force the idle thread to run on kernel page table */
    setVMRoot(കണ്ണി_STATE(ksIdleThread));
}

ശൂന്യം Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ശൂന്യം Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Don't need to do anything */
}
