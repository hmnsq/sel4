/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <kernel/thread.h>

خالی Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

خالی Arch_prepare_اگلا_Domain(خالی)
{
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

خالی Arch_prepare_سیٹ_Domain(tcb_t *tptr, dom_t dom)
{
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
