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

ಶೂನ್ಯ Arch_prepare_ಮುಂದಿನ_Domain(ಶೂನ್ಯ)
{
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush();
    }
}

ಶೂನ್ಯ Arch_prepare_ಗಣ_Domain(tcb_t *tptr, dom_t dom)
{
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_flush_if_current(tptr);
    }
}
