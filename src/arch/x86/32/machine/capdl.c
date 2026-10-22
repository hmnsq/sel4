/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_DEBUG_BUILD

#అన్తర్భావ <machine/capdl.h>
#అన్తర్భావ <arch/machine/capdl.h>

మాట_t get_tcb_sp(tcb_t *tcb)
{
    తిరిగి tcb->tcbArch.tcbContext.registers[ESP];
}

శూన్య print_ipc_బఫరు_స్థానం(tcb_t *tcb)
{
    రాయి("print_ipc_buffer_slot not implemented for IA32\n");
}

శూన్య obj_vtable_print_slots(tcb_t *tcb)
{
    రాయి("obj_vtable_print_slots not implemented for IA32\n");
}

శూన్య print_cap_arch(cap_t cap)
{
    రాయి("print_cap_arch not implemented for IA32\n");
}

శూన్య print_object_arch(cap_t cap)
{
    రాయి("print_object_arch not implemented for IA32\n");
}

శూన్య obj_tcb_print_vtable(tcb_t *tcb)
{
    రాయి("obj_tcb_print_vtable not implemented for IA32\n");
}

శూన్య debug_capDL(శూన్య)
{
    రాయి("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_చూసిన_list();
}

#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
