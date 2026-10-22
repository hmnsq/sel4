/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <object.h>
#அந்தர்பாவ <machine.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <arch/kernel/vspace.h>
#அந்தர்பாவ <arch/kernel/thread.h>
#அந்தர்பாவ <linker.h>

வெறுமை Arch_switchToThread(tcb_t *tcb)
{
    /* set PD */
    setVMRoot(tcb);
    எனில் (config_set(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    எனில் (config_set(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_CODE வெறுமை Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, FLAGS, FLAGS_USER_DEFAULT);
    setRegister(tcb, NextIP, (word_t)&idle_thread);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    setRegister(tcb, FS_BASE, 0);
    setRegister(tcb, GS_BASE, 0);
}

வெறுமை Arch_switchToIdleThread(வெறுமை)
{
    /* Force the idle thread to run on kernel page table */
    setVMRoot(முனை_STATE(ksIdleThread));
}

வெறுமை Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

வெறுமை Mode_postModifyRegisters(tcb_t *tptr)
{
    /* Don't need to do anything */
}
