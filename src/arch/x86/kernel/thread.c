/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/thread.h>

ਖਾਲੀ Arch_postModifyRegisters(tcb_t *tptr)
{
    ਬਹੁਲਕ_post_Modify_Registers(tptr);
}

ਖਾਲੀ Arch_prepare_ਅਗਲਾ_Domain(ਖਾਲੀ)
{
    /* Don't need to do anything */
}

ਖਾਲੀ Arch_prepare_ਸੈੱਟ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
