/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <arch/machine/capdl.h>

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[ESP];
}

فراغ print_ipc_بفر_خونه(tcb_t *tcb)
{
    چاپ_کړه("print_ipc_buffer_slot not implemented for IA32\n");
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    چاپ_کړه("obj_vtable_print_slots not implemented for IA32\n");
}

فراغ print_cap_arch(cap_t cap)
{
    چاپ_کړه("print_cap_arch not implemented for IA32\n");
}

فراغ print_object_arch(cap_t cap)
{
    چاپ_کړه("print_object_arch not implemented for IA32\n");
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    چاپ_کړه("obj_tcb_print_vtable not implemented for IA32\n");
}

فراغ debug_capDL(فراغ)
{
    چاپ_کړه("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_کتل_شوی_list();
}

#د_که_پای /* CONFIG_DEBUG_BUILD */
