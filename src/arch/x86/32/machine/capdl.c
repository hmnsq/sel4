/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <arch/machine/capdl.h>

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[ESP];
}

فراغ print_ipc_میانگیر_جایگاه(tcb_t *tcb)
{
    چاپ_کن("print_ipc_buffer_slot not implemented for IA32\n");
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    چاپ_کن("obj_vtable_print_slots not implemented for IA32\n");
}

فراغ print_cap_arch(cap_t cap)
{
    چاپ_کن("print_cap_arch not implemented for IA32\n");
}

فراغ print_object_arch(cap_t cap)
{
    چاپ_کن("print_object_arch not implemented for IA32\n");
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    چاپ_کن("obj_tcb_print_vtable not implemented for IA32\n");
}

فراغ debug_capDL(فراغ)
{
    چاپ_کن("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_دیده_list();
}

#پایان_اگر /* CONFIG_DEBUG_BUILD */
