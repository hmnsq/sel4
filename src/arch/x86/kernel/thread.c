/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>

ಶೂನ್ಯ Arch_postModifyRegisters(tcb_t *tptr)
{
    ಬಹುಲಕ_post_Modify_Registers(tptr);
}

ಶೂನ್ಯ Arch_prepare_ಮುಂದಿನ_Domain(ಶೂನ್ಯ)
{
    /* Don't need to do anything */
}

ಶೂನ್ಯ Arch_prepare_ಗಣ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
