/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <kernel/thread.h>

ശൂന്യം Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

ശൂന്യം Arch_prepareNextDomain(ശൂന്യം)
{
    എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

ശൂന്യം Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
