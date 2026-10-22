/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <kernel/thread.h>
#شامل <arch/kernel/thread.h>

تهی Arch_postModifyRegisters(tcb_t *tptr)
{
    Mode_postModifyRegisters(tptr);
}

تهی Arch_prepareNextDomain(تهی)
{
    /* Don't need to do anything */
}

تهی Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
