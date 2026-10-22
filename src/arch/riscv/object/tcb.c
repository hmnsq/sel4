/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <machine/registerset.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <object/tcb.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    திரும்பு 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    திரும்பு EXCEPTION_NONE;
}
