/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <kernel/thread.h>
#شامل <arch/kernel/thread.h>

خالی Arch_postModifyRegisters(tcb_t *tptr)
{
    منوال_post_Modify_Registers(tptr);
}

خالی Arch_prepare_اگلا_Domain(خالی)
{
    /* Don't need to do anything */
}

خالی Arch_prepare_سیٹ_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
