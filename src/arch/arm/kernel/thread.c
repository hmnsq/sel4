/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <kernel/thread.h>

শূন্য Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

শূন্য Arch_prepare_পরবর্তী_Domain(শূন্য)
{
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

শূন্য Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
