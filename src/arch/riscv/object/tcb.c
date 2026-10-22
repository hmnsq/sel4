/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/tcb.h>

ਸ਼ਬਦ_t CONST Arch_decodeTransfer(ਸ਼ਬਦ_t flags)
{
    ਵਾਪਸ 0;
}

exception_t CONST Arch_performTransfer(ਸ਼ਬਦ_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    ਵਾਪਸ EXCEPTION_NONE;
}
