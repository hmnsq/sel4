/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <kernel/thread.h>

शून्य Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

शून्य Arch_prepareNextDomain(शून्य)
{
    यदि (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

शून्य Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    यदि (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
