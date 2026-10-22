/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <object.h>
#அந்தர்பாவ <machine.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <arch/kernel/vspace.h>
#அந்தர்பாவ <arch/kernel/thread.h>
#அந்தர்பாவ <linker.h>

வெளி எழுத்து kernel_அடுக்கு_alloc[CONFIG_MAX_இலக்கம்_NODES][BIT(CONFIG_KERNEL_அடுக்கு_BITS)];

வெறுமை Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
}

BOOT_CODE வெறுமை Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, NextIP, (word_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE);
#ifdef ENABLE_SMP_SUPPORT
    க்கு (முழு i = 0; i < CONFIG_MAX_இலக்கம்_NODES; i++) {
        எனில் (முனை_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            setRegister(tcb, SP, (word_t)kernel_அடுக்கு_alloc + (i + 1) * BIT(CONFIG_KERNEL_அடுக்கு_BITS));
            நிறுத்து;
        }
    }
#இல்லையெனில்
    setRegister(tcb, SP, (word_t)kernel_அடுக்கு_alloc + BIT(CONFIG_KERNEL_அடுக்கு_BITS));
#endif
}

வெறுமை Arch_switchToIdleThread(வெறுமை)
{
    tcb_t *tcb = முனை_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

வெறுமை Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

வெறுமை Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

வெறுமை Arch_prepareNextDomain(வெறுமை)
{
    /* Don't need to do anything */
}

வெறுமை Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
