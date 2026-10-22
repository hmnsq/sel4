/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <machine/registerset.h>
#ضمن <object/structures.h>
#ضمن <arch/machine.h>
#ضمن <object/tcb.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    החזר 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    החזר EXCEPTION_NONE;
}
