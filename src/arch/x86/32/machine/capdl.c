/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD

#شامل <machine/capdl.h>
#شامل <arch/machine/capdl.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    واپس tcb->tcbArch.tcbContext.registers[ESP];
}

خالی print_ipc_بفر_خانہ(tcb_t *tcb)
{
    لکھو("print_ipc_buffer_slot not implemented for IA32\n");
}

خالی obj_vtable_print_slots(tcb_t *tcb)
{
    لکھو("obj_vtable_print_slots not implemented for IA32\n");
}

خالی print_cap_arch(cap_t cap)
{
    لکھو("print_cap_arch not implemented for IA32\n");
}

خالی print_object_arch(cap_t cap)
{
    لکھو("print_object_arch not implemented for IA32\n");
}

خالی obj_tcb_print_vtable(tcb_t *tcb)
{
    لکھو("obj_tcb_print_vtable not implemented for IA32\n");
}

خالی debug_capDL(خالی)
{
    لکھو("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_نظر_آیا_list();
}

#اگر_ختم /* CONFIG_DEBUG_BUILD */
