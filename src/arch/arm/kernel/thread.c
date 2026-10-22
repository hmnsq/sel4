/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <kernel/thread.h>

ಶೂನ್ಯ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

ಶೂನ್ಯ Arch_prepareNextDomain(ಶೂನ್ಯ)
{
    ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

ಶೂನ್ಯ Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
