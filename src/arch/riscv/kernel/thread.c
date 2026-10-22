/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <object.h>
#شامل <machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/kernel/thread.h>
#شامل <linker.h>

خارجی حرف kernel_ڈھیر_alloc[CONFIG_MAX_عدد_NODES][ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ)];

خالی Arch_switch_مقصد_Thread(tcb_t *tcb)
{
    سیٹ_VMRoot(tcb);
}

BOOT_رمز خالی Arch_configureIdleThread(tcb_t *tcb)
{
    سیٹ_رجسٹر(tcb, اگلا_IP, (لفظ_t)&idle_thread);

    /* Enable interrupts and keep working in supervisor mode */
    سیٹ_رجسٹر(tcb, SSTATUS, (لفظ_t) SSTATUS_SPP | SSTATUS_SPIE);
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    برائے (صحیح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
        اگر (گرہ_STATE_ON_CORE(ksIdleThread, i) == tcb) {
            سیٹ_رجسٹر(tcb, SP, (لفظ_t)kernel_ڈھیر_alloc + (i + 1) * ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ));
            توڑو;
        }
    }
#ورنہ
    سیٹ_رجسٹر(tcb, SP, (لفظ_t)kernel_ڈھیر_alloc + ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ));
#اگر_ختم
}

خالی Arch_switch_مقصد_Idle_Thread(خالی)
{
    tcb_t *tcb = گرہ_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    سیٹ_VMRoot(tcb);
}

خالی Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

خالی Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

خالی Arch_prepare_اگلا_Domain(خالی)
{
    /* Don't need to do anything */
}

خالی Arch_prepare_سیٹ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
