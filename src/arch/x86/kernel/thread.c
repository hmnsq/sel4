/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <arch/kernel/thread.h>

શૂન્ય Arch_postModifyRegisters(tcb_t *tptr)
{
    Mode_postModifyRegisters(tptr);
}

શૂન્ય Arch_prepareNextDomain(શૂન્ય)
{
    /* Don't need to do anything */
}

શૂન્ય Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
