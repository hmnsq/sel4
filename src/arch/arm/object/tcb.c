/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <sel4/constants.h>
#समावेश <machine/registerset.h>
#समावेश <object/structures.h>
#समावेश <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    वापस 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    वापस EXCEPTION_NONE;
}
