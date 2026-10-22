/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>

ಶೂನ್ಯ Arch_postModifyRegisters(tcb_t *tptr)
{
    Mode_postModifyRegisters(tptr);
}

ಶೂನ್ಯ Arch_prepareNextDomain(ಶೂನ್ಯ)
{
    /* Don't need to do anything */
}

ಶೂನ್ಯ Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
