/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    asm అస్థిర("movq %[value], %%gs:%c[offset]"
                 :
                 : [విలువ] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [స్థానభ్రంశం] "i"(OFFSETOF(కణుపు_Info_t, currentThreadUserContext)));
#అయితే_ముగింపు
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
    /* We set the RSP to 0, even though the idle thread will never touch it, as it
     * allows us to distinguish an interrupt from the idle thread from an interrupt
     * from kernel execution, just by examining the saved RSP value (since the kernel
     * thread will have a valid RSP, and never 0). See traps.S for the other side of this
     */
    సమితి_రిజిస్టరు(tcb, RSP, 0);
}

శూన్య Arch_switch_గమ్యం_Idle_Thread(శూన్య)
{
    tcb_t *tcb = కణుపు_STATE(ksIdleThread);
    /* Force the idle thread to run on kernel page table */
    సమితి_VMRoot(tcb);
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    asm అస్థిర("movq %[value], %%gs:%c[offset]"
                 :
                 : [విలువ] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [స్థానభ్రంశం] "i"(OFFSETOF(కణుపు_Info_t, currentThreadUserContext)));
#అయితే_ముగింపు
}

శూన్య Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

శూన్య బహుళకం_post_Modify_Registers(tcb_t *tptr)
{
    /* Setting Error to 0 will force a return by the interrupt path, which
     * does a full restore. Unless we're the current thread, in which case
     * we still have to go back out via a sysret */
    ఒకవేళ (tptr != కణుపు_STATE(ksCurThread)) {
        సమితి_రిజిస్టరు(tptr, Error, 0);
    }
}
