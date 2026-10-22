/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <kernel/thread.h>

فراغ Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

فراغ Arch_prepareNextDomain(فراغ)
{
    𐤀𐤌 (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

فراغ Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    𐤀𐤌 (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
