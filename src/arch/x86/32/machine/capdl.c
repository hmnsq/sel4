/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ଅନ୍ତର୍ଭାଵ <machine/capdl.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    ଫେରନ୍ତୁ tcb->tcbArch.tcbContext.registers[ESP];
}

ଶୂନ୍ୟ print_ipc_buffer_slot(tcb_t *tcb)
{
    printf("print_ipc_buffer_slot not implemented for IA32\n");
}

ଶୂନ୍ୟ obj_vtable_print_slots(tcb_t *tcb)
{
    printf("obj_vtable_print_slots not implemented for IA32\n");
}

ଶୂନ୍ୟ print_cap_arch(cap_t cap)
{
    printf("print_cap_arch not implemented for IA32\n");
}

ଶୂନ୍ୟ print_object_arch(cap_t cap)
{
    printf("print_object_arch not implemented for IA32\n");
}

ଶୂନ୍ୟ obj_tcb_print_vtable(tcb_t *tcb)
{
    printf("obj_tcb_print_vtable not implemented for IA32\n");
}

ଶୂନ୍ୟ debug_capDL(ଶୂନ୍ୟ)
{
    printf("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_seen_list();
}

#endif /* CONFIG_DEBUG_BUILD */
