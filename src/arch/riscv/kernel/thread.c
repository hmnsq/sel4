/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <object.h>
#ضمن <machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/kernel/thread.h>
#ضمن <linker.h>

خارجي حرف kernel_تر_alloc[CONFIG_MAX_لمب_NODES][BIT(CONFIG_KERNEL_تر_BITS)];

فراغ Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, NextIP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    لكل (صحيح i = 0; i < CONFIG_MAX_لمب_NODES; i++) {
        إذا (كلي_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_تر_alloc + (i + 1) * BIT(CONFIG_KERNEL_تر_BITS));
            اكسر;
        }
    }
#وإلا
    setRegister(tcb, SP, (word_t)kernel_تر_alloc + BIT(CONFIG_KERNEL_تر_BITS));
#endif
}

فراغ Arch_switchToIdleThread(فراغ)
{
    tcb_t *tcb = كلي_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

فراغ Arch_prepareNextDomain(فراغ)
{
    /* Don't need to do anything */
}

فراغ Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
