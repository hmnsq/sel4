/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/thread.h>

ଶୂନ୍ୟ Arch_postModifyRegisters(tcb_t *tptr)
{
    Mode_postModifyRegisters(tptr);
}

ଶୂନ୍ୟ Arch_prepareNextDomain(ଶୂନ୍ୟ)
{
    /* Don't need to do anything */
}

ଶୂନ୍ୟ Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
