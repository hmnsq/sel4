/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <kernel/thread.h>
#समावेश <arch/kernel/thread.h>

व्योम Arch_postModifyRegisters(tcb_t *tptr)
{
    Mode_postModifyRegisters(tptr);
}

व्योम Arch_prepare_अग्र_Domain(व्योम)
{
    /* Don't need to do anything */
}

व्योम Arch_prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
