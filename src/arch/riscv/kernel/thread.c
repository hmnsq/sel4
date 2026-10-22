/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <object.h>
#شامل <machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/kernel/thread.h>
#شامل <linker.h>

بیرونی نویسه kernel_پشته_alloc[CONFIG_MAX_رقم_NODES][BIT(CONFIG_KERNEL_پشته_BITS)];

تهی Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE تهی Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, NextIP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    برای (صحیح i = 0; i < CONFIG_MAX_رقم_NODES; i++) {
        اگر (گره_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_پشته_alloc + (i + 1) * BIT(CONFIG_KERNEL_پشته_BITS));
            بشکن;
        }
    }
#وگرنه
    setRegister(tcb, SP, (word_t)kernel_پشته_alloc + BIT(CONFIG_KERNEL_پشته_BITS));
#endif
}

تهی Arch_switchToIdleThread(تهی)
{
    tcb_t *tcb = گره_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

تهی Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

تهی Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

تهی Arch_prepareNextDomain(تهی)
{
    /* Don't need to do anything */
}

تهی Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
