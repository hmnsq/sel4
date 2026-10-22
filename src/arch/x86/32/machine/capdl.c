/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_DEBUG_BUILD

#समावेश <machine/capdl.h>
#समावेश <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    वापस tcb->tcbArch.tcbContext.registers[ESP];
}

व्योम print_ipc_बफर_slot(tcb_t *tcb)
{
    म_लिखो("print_ipc_buffer_slot not implemented for IA32\n");
}

व्योम obj_vtable_print_slots(tcb_t *tcb)
{
    म_लिखो("obj_vtable_print_slots not implemented for IA32\n");
}

व्योम print_cap_arch(cap_t cap)
{
    म_लिखो("print_cap_arch not implemented for IA32\n");
}

व्योम print_object_arch(cap_t cap)
{
    म_लिखो("print_object_arch not implemented for IA32\n");
}

व्योम obj_tcb_print_vtable(tcb_t *tcb)
{
    म_लिखो("obj_tcb_print_vtable not implemented for IA32\n");
}

व्योम debug_capDL(व्योम)
{
    म_लिखो("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_seen_list();
}

#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
