/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <object/tcb.h>

మాట_t CONST Arch_decodeTransfer(మాట_t flags)
{
    తిరిగి 0;
}

exception_t CONST Arch_performTransfer(మాట_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    తిరిగి EXCEPTION_NONE;
}
