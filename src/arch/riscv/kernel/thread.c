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

خارجي حرف kernel_ډېری_alloc[CONFIG_MAX_عدد_NODES][بټ(CONFIG_KERNEL_ډېری_بټونه)];

فراغ Arch_switch_منزل_Thread(tcb_t *tcb)
{
    سېټ_VMRoot(tcb);
}

BOOT_کوډ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سېټ_ثبتوونکی(tcb, بل_IP, (کلمه_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    سېټ_ثبتوونکی(tcb, SSTATUS, (کلمه_t) SSTATUS_SPP | SSTATUS_SPIE);
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    لكل (صحيح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
        إذا (غوټه_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            سېټ_ثبتوونکی(tcb, SP, (کلمه_t)kernel_ډېری_alloc + (i + 1) * بټ(CONFIG_KERNEL_ډېری_بټونه));
            اكسر;
        }
    }
#وإلا
    سېټ_ثبتوونکی(tcb, SP, (کلمه_t)kernel_ډېری_alloc + بټ(CONFIG_KERNEL_ډېری_بټونه));
#د_که_پای
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    tcb_t *tcb = غوټه_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    سېټ_VMRoot(tcb);
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

فراغ Arch_prepare_بل_Domain(فراغ)
{
    /* Don't need to do anything */
}

فراغ Arch_prepare_سېټ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
