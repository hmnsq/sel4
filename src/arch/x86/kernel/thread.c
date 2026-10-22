/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <arch/kernel/thread.h>

வெறுமை Arch_postModifyRegisters(tcb_t *tptr)
{
    Mode_postModifyRegisters(tptr);
}

வெறுமை Arch_prepareNextDomain(வெறுமை)
{
    /* Don't need to do anything */
}

வெறுமை Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
