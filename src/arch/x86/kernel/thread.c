/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <arch/kernel/thread.h>

शून्य Arch_postModifyRegisters(tcb_t *tptr)
{
    बहुलकम्_post_Modify_Registers(tptr);
}

शून्य Arch_prepare_अग्र_Domain(शून्य)
{
    /* Don't need to do anything */
}

शून्य Arch_prepare_समुच्चयः_Domain(tcb_t *tptr, dom_t dom)
{
    /* Don't need to do anything */
}
