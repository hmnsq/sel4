/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>

ਬਾਹਰੀ ਅੱਖਰ kernel_ਢੇਰ_alloc[CONFIG_MAX_ਅੰਕ_NODES][ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ)];

ਖਾਲੀ Arch_switch_ਮੰਜ਼ਿਲ_Thread(tcb_t *tcb)
{
    ਸੈੱਟ_VMRoot(tcb);
}

BOOT_ਕੋਡ ਖਾਲੀ Arch_configureIdleThread(tcb_t *tcb)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, ਅਗਲਾ_IP, (ਸ਼ਬਦ_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, SSTATUS, (ਸ਼ਬਦ_t) SSTATUS_SPP | SSTATUS_SPIE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਲਈ (ਪੂਰਨ i = 0; i < CONFIG_MAX_ਅੰਕ_NODES; i++) {
        ਜੇ (ਗੰਢ_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, SP, (ਸ਼ਬਦ_t)kernel_ਢੇਰ_alloc + (i + 1) * ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ));
            ਰੋਕੋ;
        }
    }
#ਨਹੀਂਤਾਂ
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, SP, (ਸ਼ਬਦ_t)kernel_ਢੇਰ_alloc + ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ));
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Arch_switch_ਮੰਜ਼ਿਲ_Idle_Thread(ਖਾਲੀ)
{
    tcb_t *tcb = ਗੰਢ_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    ਸੈੱਟ_VMRoot(tcb);
}

ਖਾਲੀ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ਖਾਲੀ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

ਖਾਲੀ Arch_prepare_ਅਗਲਾ_Domain(ਖਾਲੀ)
{
    /* Don't need to do anything */
}

ਖਾਲੀ Arch_prepare_ਸੈੱਟ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
