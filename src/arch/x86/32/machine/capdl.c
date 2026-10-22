/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/capdl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/capdl.h>

ਸ਼ਬਦ_t get_tcb_sp(tcb_t *tcb)
{
    ਵਾਪਸ tcb->tcbArch.tcbContext.registers[ESP];
}

ਖਾਲੀ print_ipc_ਬਫਰ_ਥਾਂ(tcb_t *tcb)
{
    ਲਿਖੋ("print_ipc_buffer_slot not implemented for IA32\n");
}

ਖਾਲੀ obj_vtable_print_slots(tcb_t *tcb)
{
    ਲਿਖੋ("obj_vtable_print_slots not implemented for IA32\n");
}

ਖਾਲੀ print_cap_arch(cap_t cap)
{
    ਲਿਖੋ("print_cap_arch not implemented for IA32\n");
}

ਖਾਲੀ print_object_arch(cap_t cap)
{
    ਲਿਖੋ("print_object_arch not implemented for IA32\n");
}

ਖਾਲੀ obj_tcb_print_vtable(tcb_t *tcb)
{
    ਲਿਖੋ("obj_tcb_print_vtable not implemented for IA32\n");
}

ਖਾਲੀ debug_capDL(ਖਾਲੀ)
{
    ਲਿਖੋ("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_ਵੇਖਿਆ_list();
}

#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
