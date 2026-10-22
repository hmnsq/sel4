/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <sel4/constants.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    परत 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    परत EXCEPTION_NONE;
}
