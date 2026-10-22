/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <arch/kernel/thread.h>

ശൂന്യം Arch_postModifyRegisters(tcb_t *tptr)
{
    Mode_postModifyRegisters(tptr);
}

ശൂന്യം Arch_prepareNextDomain(ശൂന്യം)
{
    /* Don't need to do anything */
}

ശൂന്യം Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
