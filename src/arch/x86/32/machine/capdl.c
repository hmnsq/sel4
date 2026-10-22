/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 * Copyright 2021, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>

ಶಬ್ದ_t get_tcb_sp(tcb_t *tcb)
{
    ಹಿಂತಿರುಗಿ tcb->tcbArch.tcbContext.registers[ESP];
}

ಶೂನ್ಯ print_ipc_ಬಫರ್_ಸ್ಥಾನ(tcb_t *tcb)
{
    ಬರೆ("print_ipc_buffer_slot not implemented for IA32\n");
}

ಶೂನ್ಯ obj_vtable_print_slots(tcb_t *tcb)
{
    ಬರೆ("obj_vtable_print_slots not implemented for IA32\n");
}

ಶೂನ್ಯ print_cap_arch(cap_t cap)
{
    ಬರೆ("print_cap_arch not implemented for IA32\n");
}

ಶೂನ್ಯ print_object_arch(cap_t cap)
{
    ಬರೆ("print_object_arch not implemented for IA32\n");
}

ಶೂನ್ಯ obj_tcb_print_vtable(tcb_t *tcb)
{
    ಬರೆ("obj_tcb_print_vtable not implemented for IA32\n");
}

ಶೂನ್ಯ debug_capDL(ಶೂನ್ಯ)
{
    ಬರೆ("Debug CapDL snapshot not full implemented for IA32\n");
    /* reset the seen list */
    reset_ನೋಡಿದ_list();
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
