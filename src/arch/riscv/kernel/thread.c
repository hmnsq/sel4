/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <object.h>
#অন্তর্ভাব <machine.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <arch/kernel/thread.h>
#অন্তর্ভাব <linker.h>

বাহ্যিক অক্ষর kernel_স্তূপ_alloc[CONFIG_MAX_অঙ্ক_NODES][BIT(CONFIG_KERNEL_স্তূপ_BITS)];

শূন্য Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE শূন্য Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, পরবর্তী_IP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    জন্য (পূর্ণসংখ্যা i = 0; i < CONFIG_MAX_অঙ্ক_NODES; i++) {
        যদি (গ্রন্থি_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_স্তূপ_alloc + (i + 1) * BIT(CONFIG_KERNEL_স্তূপ_BITS));
            বিরতি;
        }
    }
#অন্যথা
    setRegister(tcb, SP, (word_t)kernel_স্তূপ_alloc + BIT(CONFIG_KERNEL_স্তূপ_BITS));
#endif
}

শূন্য Arch_switchToIdleThread(শূন্য)
{
    tcb_t *tcb = গ্রন্থি_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

শূন্য Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

শূন্য Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

শূন্য Arch_prepare_পরবর্তী_Domain(শূন্য)
{
    /* Don't need to do anything */
}

শূন্য Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
