/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <object.h>
#അന്തര്ഭാവ <machine.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <arch/kernel/vspace.h>
#അന്തര്ഭാവ <arch/kernel/thread.h>
#അന്തര്ഭാവ <linker.h>

ബാഹ്യ അക്ഷരം kernel_അട്ടി_alloc[CONFIG_MAX_അക്കം_NODES][BIT(CONFIG_KERNEL_അട്ടി_BITS)];

ശൂന്യം Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE ശൂന്യം Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, NextIP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < CONFIG_MAX_അക്കം_NODES; i++) {
        എങ്കിൽ (കണ്ണി_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_അട്ടി_alloc + (i + 1) * BIT(CONFIG_KERNEL_അട്ടി_BITS));
            നിർത്തുക;
        }
    }
#അല്ലെങ്കിൽ
    setRegister(tcb, SP, (word_t)kernel_അട്ടി_alloc + BIT(CONFIG_KERNEL_അട്ടി_BITS));
#endif
}

ശൂന്യം Arch_switchToIdleThread(ശൂന്യം)
{
    tcb_t *tcb = കണ്ണി_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

ശൂന്യം Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ശൂന്യം Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

ശൂന്യം Arch_prepareNextDomain(ശൂന്യം)
{
    /* Don't need to do anything */
}

ശൂന്യം Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
