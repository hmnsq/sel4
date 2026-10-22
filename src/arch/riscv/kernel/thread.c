/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <object.h>
#समावेश <machine.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <arch/kernel/thread.h>
#समावेश <linker.h>

बाह्य अक्षर kernel_ढेर_alloc[CONFIG_MAX_अंक_NODES][BIT(CONFIG_KERNEL_ढेर_BITS)];

व्योम Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE व्योम Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, अग्र_IP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#अगर_घोषित ENABLE_SMP_SUPPORT
    क्रम (पूर्णांक i = 0; i < CONFIG_MAX_अंक_NODES; i++) {
        अगर (गाँठ_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_ढेर_alloc + (i + 1) * BIT(CONFIG_KERNEL_ढेर_BITS));
            अवरोध;
        }
    }
#अन्यथा
    setRegister(tcb, SP, (word_t)kernel_ढेर_alloc + BIT(CONFIG_KERNEL_ढेर_BITS));
#पूर्ण_अगर
}

व्योम Arch_switchToIdleThread(व्योम)
{
    tcb_t *tcb = गाँठ_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

व्योम Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

व्योम Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

व्योम Arch_prepare_अग्र_Domain(व्योम)
{
    /* Don't need to do anything */
}

व्योम Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
