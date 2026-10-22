/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <object.h>
#ଅନ୍ତର୍ଭାଵ <machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>

ବାହ୍ୟ ଅକ୍ଷର kernel_ଗଦା_alloc[CONFIG_MAX_ଅଙ୍କ_NODES][BIT(CONFIG_KERNEL_ଗଦା_BITS)];

ଶୂନ୍ୟ Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE ଶୂନ୍ୟ Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, NextIP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < CONFIG_MAX_ଅଙ୍କ_NODES; i++) {
        ଯଦି (ଗଣ୍ଠି_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_ଗଦା_alloc + (i + 1) * BIT(CONFIG_KERNEL_ଗଦା_BITS));
            ଭାଙ୍ଗ;
        }
    }
#ଅନ୍ୟଥା
    setRegister(tcb, SP, (word_t)kernel_ଗଦା_alloc + BIT(CONFIG_KERNEL_ଗଦା_BITS));
#endif
}

ଶୂନ୍ୟ Arch_switchToIdleThread(ଶୂନ୍ୟ)
{
    tcb_t *tcb = ଗଣ୍ଠି_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

ଶୂନ୍ୟ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ଶୂନ୍ୟ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

ଶୂନ୍ୟ Arch_prepareNextDomain(ଶୂନ୍ୟ)
{
    /* Don't need to do anything */
}

ଶୂନ୍ୟ Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
