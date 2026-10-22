/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <kernel/thread.h>

શૂન્ય Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

શૂન્ય Arch_prepareNextDomain(શૂન્ય)
{
    જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

શૂન્ય Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
