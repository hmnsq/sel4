/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <api/failures.h>
#অন্তর্ভাৱ <sel4/constants.h>
#অন্তর্ভাৱ <machine/registerset.h>
#অন্তর্ভাৱ <object/structures.h>
#অন্তর্ভাৱ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    ঘূৰাই 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    ঘূৰাই EXCEPTION_NONE;
}
