/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <sel4/constants.h>
#അന്തര്ഭാവ <machine/registerset.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    തിരികെ 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    തിരികെ EXCEPTION_NONE;
}
