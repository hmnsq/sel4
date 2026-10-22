/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <arch/machine/capdl.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[ESP];
}

فراغ print_ipc_بفر_تھاں(tcb_t *tcb)
{
    لکھو("print_ipc_buffer_slot not implemented for IA32\n");
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    لکھو("obj_vtable_print_slots not implemented for IA32\n");
}

فراغ print_cap_arch(cap_t cap)
{
    لکھو("print_cap_arch not implemented for IA32\n");
}

فراغ print_object_arch(cap_t cap)
{
    لکھو("print_object_arch not implemented for IA32\n");
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    لکھو("obj_tcb_print_vtable not implemented for IA32\n");
}

فراغ debug_capDL(فراغ)
{
    لکھو("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_تکیا_list();
}

#جے_انت /* CONFIG_DEBUG_BUILD */
