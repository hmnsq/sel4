/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <arch/kernel/thread.h>

శూన్య Arch_postModifyRegisters(tcb_t *tptr)
{
    బహుళకం_post_Modify_Registers(tptr);
}

శూన్య Arch_prepare_తదుపరి_Domain(శూన్య)
{
    /* Don't need to do anything */
}

శూన్య Arch_prepare_సమితి_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
