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

خارجي حرف kernel_ڈھیر_alloc[CONFIG_MAX_عدد_NODES][ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ)];

فراغ Arch_switch_منزل_Thread(tcb_t *tcb)
{
    سیٹ_VMRoot(tcb);
}

BOOT_کوڈ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سیٹ_رجسٹر(tcb, اگلا_IP, (لفظ_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    سیٹ_رجسٹر(tcb, SSTATUS, (لفظ_t) SSTATUS_SPP | SSTATUS_SPIE);
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    لكل (صحيح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
        إذا (گرہ_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            سیٹ_رجسٹر(tcb, SP, (لفظ_t)kernel_ڈھیر_alloc + (i + 1) * ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ));
            اكسر;
        }
    }
#وإلا
    سیٹ_رجسٹر(tcb, SP, (لفظ_t)kernel_ڈھیر_alloc + ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ));
#جے_انت
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    tcb_t *tcb = گرہ_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    سیٹ_VMRoot(tcb);
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

فراغ Arch_prepare_اگلا_Domain(فراغ)
{
    /* Don't need to do anything */
}

فراغ Arch_prepare_سیٹ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
