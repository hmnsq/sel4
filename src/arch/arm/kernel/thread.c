/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <kernel/thread.h>

శూన్య Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}

శూన్య Arch_prepare_తదుపరి_Domain(శూన్య)
{
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

శూన్య Arch_prepare_సమితి_Domain(tcb_t *tptr, dom_t dom)
{
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
