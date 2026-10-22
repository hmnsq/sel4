/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <kernel/thread.h>

व्योम Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

व्योम Arch_prepare_अग्र_Domain(व्योम)
{
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

व्योम Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
