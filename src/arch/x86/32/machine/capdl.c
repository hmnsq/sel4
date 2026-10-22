/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <arch/machine/capdl.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[ESP];
}

فراغ print_ipc_بفر_جڳهه(tcb_t *tcb)
{
    لکو("print_ipc_buffer_slot not implemented for IA32\n");
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    لکو("obj_vtable_print_slots not implemented for IA32\n");
}

فراغ print_cap_arch(cap_t cap)
{
    لکو("print_cap_arch not implemented for IA32\n");
}

فراغ print_object_arch(cap_t cap)
{
    لکو("print_object_arch not implemented for IA32\n");
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    لکو("obj_tcb_print_vtable not implemented for IA32\n");
}

فراغ debug_capDL(فراغ)
{
    لکو("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_ڏٺل_list();
}

#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
