/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <kernel/thread.h>

வெறுமை Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

வெறுமை Arch_prepareNextDomain(வெறுமை)
{
    எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

வெறுமை Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
