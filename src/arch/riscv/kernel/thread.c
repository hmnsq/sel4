/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <linker.h>

ಬಾಹ್ಯ ವರ್ಣ kernel_ರಾಶಿ_alloc[CONFIG_MAX_ಅಂಕೆ_NODES][BIT(CONFIG_KERNEL_ರಾಶಿ_BITS)];

ಶೂನ್ಯ Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE ಶೂನ್ಯ Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, NextIP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < CONFIG_MAX_ಅಂಕೆ_NODES; i++) {
        ಯದಿ (ಗಂಟು_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_ರಾಶಿ_alloc + (i + 1) * BIT(CONFIG_KERNEL_ರಾಶಿ_BITS));
            ಭಙ್ಗ;
        }
    }
#ಅನ್ಯಥಾ
    setRegister(tcb, SP, (word_t)kernel_ರಾಶಿ_alloc + BIT(CONFIG_KERNEL_ರಾಶಿ_BITS));
#endif
}

ಶೂನ್ಯ Arch_switchToIdleThread(ಶೂನ್ಯ)
{
    tcb_t *tcb = ಗಂಟು_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

ಶೂನ್ಯ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ಶೂನ್ಯ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

ಶೂನ್ಯ Arch_prepareNextDomain(ಶೂನ್ಯ)
{
    /* Don't need to do anything */
}

ಶೂನ್ಯ Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
