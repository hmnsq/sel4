/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <kernel/thread.h>
#ضمن <arch/kernel/thread.h>

فراغ Arch_postModifyRegisters(tcb_t *tptr)
{
    نما_post_Modify_Registers(tptr);
}

فراغ Arch_prepare_بعدی_Domain(فراغ)
{
    /* Don't need to do anything */
}

فراغ Arch_prepare_مجموعه_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
