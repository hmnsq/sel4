/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <object/tcb.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    प्रत्यागम 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    प्रत्यागम EXCEPTION_NONE;
}
