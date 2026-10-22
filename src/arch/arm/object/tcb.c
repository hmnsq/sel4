/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <sel4/constants.h>
#شامل <machine/registerset.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    بازگشت 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    بازگشت EXCEPTION_NONE;
}
