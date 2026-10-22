/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <object.h>
#અન્તર્ભાવ <machine.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <arch/kernel/thread.h>
#અન્તર્ભાવ <linker.h>

શૂન્ય Arch_switchToThread(tcb_t *tcb)
{
    /* set PD */
    setVMRoot(tcb);
    જો (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    જો (config_set(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_CODE શૂન્ય Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, FLAGS, FLAGS_USER_DEFAULT);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    setRegister(tcb, FS_BASE, 0);
    setRegister(tcb, GS_BASE, 0);
}

શૂન્ય Arch_switchToIdleThread(શૂન્ય)
{
    /* Force the idle thread to run on kernel page table */
    setVMRoot(ગાંઠ_STATE(ksIdleThread));
}

શૂન્ય Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

શૂન્ય Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Don't need to do anything */
}
