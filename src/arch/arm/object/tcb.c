/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <sel4/constants.h>
#அந்தர்பாவ <machine/registerset.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    திரும்பு 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    திரும்பு EXCEPTION_NONE;
}
