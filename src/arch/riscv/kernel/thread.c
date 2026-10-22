/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <object.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/kernel/thread.h>
#अन्तर्भाव <linker.h>

बाह्य वर्ण kernel_राशिः_alloc[CONFIG_MAX_अङ्कः_NODES][BIT(CONFIG_KERNEL_राशिः_BITS)];

शून्य Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE शून्य Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, NextIP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    प्रति (पूर्णाङ्क i = 0; i < CONFIG_MAX_अङ्कः_NODES; i++) {
        जइ (ग्रन्थिः_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_राशिः_alloc + (i + 1) * BIT(CONFIG_KERNEL_राशिः_BITS));
            भङ्ग;
        }
    }
#अण्णहा
    setRegister(tcb, SP, (word_t)kernel_राशिः_alloc + BIT(CONFIG_KERNEL_राशिः_BITS));
#endif
}

शून्य Arch_switchToIdleThread(शून्य)
{
    tcb_t *tcb = ग्रन्थिः_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

शून्य Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

शून्य Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

शून्य Arch_prepareNextDomain(शून्य)
{
    /* Don't need to do anything */
}

शून्य Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
