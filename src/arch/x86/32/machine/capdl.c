/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#அந்தர்பாவ <machine/capdl.h>
#அந்தர்பாவ <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    திரும்பு tcb->tcbArch.tcbContext.registers[ESP];
}

வெறுமை print_ipc_buffer_slot(tcb_t *tcb)
{
    printf("print_ipc_buffer_slot not implemented for IA32\n");
}

வெறுமை obj_vtable_print_slots(tcb_t *tcb)
{
    printf("obj_vtable_print_slots not implemented for IA32\n");
}

வெறுமை print_cap_arch(cap_t cap)
{
    printf("print_cap_arch not implemented for IA32\n");
}

வெறுமை print_object_arch(cap_t cap)
{
    printf("print_object_arch not implemented for IA32\n");
}

வெறுமை obj_tcb_print_vtable(tcb_t *tcb)
{
    printf("obj_tcb_print_vtable not implemented for IA32\n");
}

வெறுமை debug_capDL(வெறுமை)
{
    printf("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_seen_list();
}

#endif /* CONFIG_DEBUG_BUILD */
