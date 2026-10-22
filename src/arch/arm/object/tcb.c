/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <sel4/constants.h>
#ضمن <machine/registerset.h>
#ضمن <object/structures.h>
#ضمن <arch/machine.h>

لفظ_t CONST Arch_decodeTransfer(لفظ_t flags)
{
    أرجع 0;
}

exception_t CONST Arch_performTransfer(لفظ_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    أرجع EXCEPTION_NONE;
}
