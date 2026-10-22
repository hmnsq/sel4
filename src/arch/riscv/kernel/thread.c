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

خارجي حرف kernel_پشته_alloc[CONFIG_MAX_رقم_NODES][بیت(CONFIG_KERNEL_پشته_بیتها)];

فراغ Arch_switch_مقصد_Thread(tcb_t *tcb)
{
    مجموعه_VMRoot(tcb);
}

BOOT_کد فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    مجموعه_ثبات(tcb, بعدی_IP, (کلمه_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    مجموعه_ثبات(tcb, SSTATUS, (کلمه_t) SSTATUS_SPP | SSTATUS_SPIE);
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    لكل (صحيح i = 0; i < CONFIG_MAX_رقم_NODES; i++) {
        إذا (گره_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            مجموعه_ثبات(tcb, SP, (کلمه_t)kernel_پشته_alloc + (i + 1) * بیت(CONFIG_KERNEL_پشته_بیتها));
            اكسر;
        }
    }
#وإلا
    مجموعه_ثبات(tcb, SP, (کلمه_t)kernel_پشته_alloc + بیت(CONFIG_KERNEL_پشته_بیتها));
#پایان_اگر
}

فراغ Arch_switch_مقصد_Idle_Thread(فراغ)
{
    tcb_t *tcb = گره_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    مجموعه_VMRoot(tcb);
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

فراغ Arch_prepare_بعدی_Domain(فراغ)
{
    /* Don't need to do anything */
}

فراغ Arch_prepare_مجموعه_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
