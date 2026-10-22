/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <api/failures.h>
#شامل <machine/registerset.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>
#شامل <object/tcb.h>

لفظ_t CONST Arch_decodeTransfer(لفظ_t flags)
{
    واپس 0;
}

exception_t CONST Arch_performTransfer(لفظ_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    واپس EXCEPTION_NONE;
}
