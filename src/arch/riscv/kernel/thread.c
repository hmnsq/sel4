/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <object.h>
#અન્તર્ભાવ <machine.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <arch/kernel/thread.h>
#અન્તર્ભાવ <linker.h>

બાહ્ય અક્ષર kernel_ઢગલો_alloc[CONFIG_MAX_અંક_NODES][BIT(CONFIG_KERNEL_ઢગલો_BITS)];

શૂન્ય Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE શૂન્ય Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, NextIP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    માટે (પૂર્ણાંક i = 0; i < CONFIG_MAX_અંક_NODES; i++) {
        જો (ગાંઠ_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_ઢગલો_alloc + (i + 1) * BIT(CONFIG_KERNEL_ઢગલો_BITS));
            તોડો;
        }
    }
#નહીંતર
    setRegister(tcb, SP, (word_t)kernel_ઢગલો_alloc + BIT(CONFIG_KERNEL_ઢગલો_BITS));
#endif
}

શૂન્ય Arch_switchToIdleThread(શૂન્ય)
{
    tcb_t *tcb = ગાંઠ_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

શૂન્ય Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

શૂન્ય Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

શૂન્ય Arch_prepareNextDomain(શૂન્ય)
{
    /* Don't need to do anything */
}

શૂન્ય Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
