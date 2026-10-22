/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <kernel/thread.h>

تهی Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

تهی Arch_prepareNextDomain(تهی)
{
    اگر (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

تهی Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    اگر (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
