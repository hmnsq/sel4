/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <object.h>
#అన్తర్భావ <machine.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/kernel/thread.h>
#అన్తర్భావ <linker.h>

బాహ్య అక్షరం kernel_దొంతర_alloc[CONFIG_MAX_అంకె_NODES][బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు)];

శూన్య Arch_switch_గమ్యం_Thread(tcb_t *tcb)
{
    సమితి_VMRoot(tcb);
}

BOOT_సంకేతం శూన్య Arch_configureIdleThread(tcb_t *tcb)
{
    సమితి_రిజిస్టరు(tcb, తదుపరి_IP, (మాట_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    సమితి_రిజిస్టరు(tcb, SSTATUS, (మాట_t) SSTATUS_SPP | SSTATUS_SPIE);
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    కొరకు (పూర్ణాంకం i = 0; i < CONFIG_MAX_అంకె_NODES; i++) {
        ఒకవేళ (కణుపు_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            సమితి_రిజిస్టరు(tcb, SP, (మాట_t)kernel_దొంతర_alloc + (i + 1) * బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు));
            ఆపు;
        }
    }
#లేకపోతే
    సమితి_రిజిస్టరు(tcb, SP, (మాట_t)kernel_దొంతర_alloc + బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు));
#అయితే_ముగింపు
}

శూన్య Arch_switch_గమ్యం_Idle_Thread(శూన్య)
{
    tcb_t *tcb = కణుపు_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    సమితి_VMRoot(tcb);
}

శూన్య Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

శూన్య Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

శూన్య Arch_prepare_తదుపరి_Domain(శూన్య)
{
    /* Don't need to do anything */
}

శూన్య Arch_prepare_సమితి_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
