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

خارجي حرف kernel_ڍير_alloc[CONFIG_MAX_انگ_NODES][ٻٽو_انگ(CONFIG_KERNEL_ڍير_بٽ)];

فراغ Arch_switch_منزل_Thread(tcb_t *tcb)
{
    سيٽ_VMRoot(tcb);
}

BOOT_ڪوڊ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سيٽ_رجسٽر(tcb, ايندڙ_IP, (لفظ_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    سيٽ_رجسٽر(tcb, SSTATUS, (لفظ_t) SSTATUS_SPP | SSTATUS_SPIE);
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    لكل (صحيح i = 0; i < CONFIG_MAX_انگ_NODES; i++) {
        إذا (ڳنڍ_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            سيٽ_رجسٽر(tcb, SP, (لفظ_t)kernel_ڍير_alloc + (i + 1) * ٻٽو_انگ(CONFIG_KERNEL_ڍير_بٽ));
            اكسر;
        }
    }
#وإلا
    سيٽ_رجسٽر(tcb, SP, (لفظ_t)kernel_ڍير_alloc + ٻٽو_انگ(CONFIG_KERNEL_ڍير_بٽ));
#جيڪڏهن_پورو
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    tcb_t *tcb = ڳنڍ_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    سيٽ_VMRoot(tcb);
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

فراغ Arch_prepare_ايندڙ_Domain(فراغ)
{
    /* Don't need to do anything */
}

فراغ Arch_prepare_سيٽ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
