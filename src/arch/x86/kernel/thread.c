/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <arch/kernel/thread.h>

শূন্য Arch_postModifyRegisters(tcb_t *tptr)
{
    Mode_postModifyRegisters(tptr);
}

শূন্য Arch_prepare_পরবর্তী_Domain(শূন্য)
{
    /* Don't need to do anything */
}

শূন্য Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
