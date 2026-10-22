/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <machine/registerset.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <object/tcb.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    પાછા 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    પાછા EXCEPTION_NONE;
}
