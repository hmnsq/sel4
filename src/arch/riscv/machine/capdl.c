/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>

ಶಬ್ದ_t get_tcb_sp(tcb_t *tcb)
{
    ಹಿಂತಿರುಗಿ tcb->tcbArch.tcbContext.registers[SP];
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING

ಸ್ಥಿರ ಶೂನ್ಯ obj_asidpool_print_attrs(cap_t asid_cap);
ಸ್ಥಿರ ಶೂನ್ಯ obj_ಚೌಕಟ್ಟು_print_attrs(paddr_t paddr);
ಸ್ಥಿರ ಶೂನ್ಯ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, ಪೂರ್ಣಾಂಕ level);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr(ಶಬ್ದ_t vptr, pte_t *lvl1pt);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte_t *pt_ಸ್ಥಾನ);

ಸ್ಥಿರ ಶೂನ್ಯ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    ಬರೆ("(asid_high: 0x%lx)\n", ASID_ಮೇಲಿನ(asid));
}

ಶೂನ್ಯ print_ipc_ಬಫರ್_ಸ್ಥಾನ(tcb_t *tcb)
{
    ಶಬ್ದ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);

    ಬರೆ("ipc_buffer_slot: ");
    cap_ಚೌಕಟ್ಟು_print_attrs_vptr(vptr, ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_cap_pt_print_slots(pte_t *ಊರ್ಧ್ವ_Pt_ಸ್ಥಾನ, ಶಬ್ದ_t pt_ಸೂಚ್ಯಂಕ, ಪೂರ್ಣಾಂಕ level)
{
    pte_t *pt;
    ಆದರೆ (level == CONFIG_PT_ಮಟ್ಟಗಳು) {
        ಬರೆ("%p_pd {\n", ಊರ್ಧ್ವ_Pt_ಸ್ಥಾನ);
        pt = ಊರ್ಧ್ವ_Pt_ಸ್ಥಾನ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("pt_%p_%04lu {\n", ಊರ್ಧ್ವ_Pt_ಸ್ಥಾನ, pt_ಸೂಚ್ಯಂಕ);
        pt = getPPtrFromHWPTE(ಊರ್ಧ್ವ_Pt_ಸ್ಥಾನ);
    }
    level -= 1;

    ಶಬ್ದ_t pt_ಬಿಟ್ಟುಗಳು_ಎಡ = PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * level + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು;

    /* - 1 to avoid overflowing */
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(pt_ಬಿಟ್ಟುಗಳು_ಎಡ + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1; i += (1 << (pt_ಬಿಟ್ಟುಗಳು_ಎಡ))) {
        ಶಬ್ದ_t pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ = ((i >> pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
        pte_t *pt_ಸ್ಥಾನ = pt + pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ;
        ಆದರೆ (pte_ptr_get_valid(pt_ಸ್ಥಾನ)) {
            ಆದರೆ (level) { /* pt */
                ಬರೆ("0x%lx: pt_%p_%04lu\n", pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ, pt_ಸ್ಥಾನ, pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ);
            } ಇಲ್ಲದಿದ್ದರೆ { /*ಚೌಕಟ್ಟು*/
                ಬರೆ("0x%lx: frame_%p_%04lu", pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ, pt_ಸ್ಥಾನ, pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ);
                cap_ಚೌಕಟ್ಟು_print_attrs_pt(pt_ಸ್ಥಾನ);
            }
        }
    }
    ಬರೆ("}\n"); /* lvl1pt/pt */

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(pt_ಬಿಟ್ಟುಗಳು_ಎಡ + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1; i += (1 << (pt_ಬಿಟ್ಟುಗಳು_ಎಡ))) {
        ಶಬ್ದ_t pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ = ((i >> pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
        pte_t *pt_ಸ್ಥಾನ = pt + pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ;
        ಆದರೆ (pte_ptr_get_valid(pt_ಸ್ಥಾನ)) {
            ಆದರೆ (level) { /* pt */
                riscv_cap_pt_print_slots(pt_ಸ್ಥಾನ, pt_ಸ್ಥಾನ_ಸೂಚ್ಯಂಕ, level);
            }
        }
    }
}

ಶೂನ್ಯ obj_vtable_print_slots(tcb_t *tcb)
{
    ಆದರೆ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_ದ_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_ಮಟ್ಟಗಳು);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte_t *pt_ಸ್ಥಾನ)
{
    ಬರೆ("(");

    /* rights */
    ಆದರೆ (pte_ptr_get_read(pt_ಸ್ಥಾನ)) {
        ಬರೆ("R");
    }

    ಆದರೆ (pte_ptr_get_write(pt_ಸ್ಥಾನ)) {
        ಬರೆ("W");
    }

    ಆದರೆ (pte_ptr_get_execute(pt_ಸ್ಥಾನ)) {
        ಬರೆ("X");
    }

    /* cacheable not supported yet */

    ಬರೆ(")\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr(ಶಬ್ದ_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    ಪ್ರತಿಜ್ಞಾ(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ == se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    ಶಬ್ದ_t ಸ್ಥಾನ = ((vptr >> lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));

    ಬರೆ("frame_%p_%04lu ", lu_ret.pt_ಸ್ಥಾನ, ಸ್ಥಾನ);
    cap_ಚೌಕಟ್ಟು_print_attrs_pt(lu_ret.pt_ಸ್ಥಾನ);
}

ಶೂನ್ಯ print_cap_arch(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap: {
        asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap);
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap);

        ಶಬ್ದ_t pt_ಬಿಟ್ಟುಗಳು_ಎಡ = PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * CONFIG_PT_ಮಟ್ಟಗಳು + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು;
        ಶಬ್ದ_t ಸ್ಥಾನ = ((vptr >> pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
        ಆದರೆ (asid) {
            ಬರೆ("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr).pt_ಸ್ಥಾನ, ಸ್ಥಾನ, (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("pt_%p_%04lu\n", lookupPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr).pt_ಸ್ಥಾನ, ಸ್ಥಾನ);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_asid_control_cap: {
        /* only one in the system */
        ಬರೆ("asid_control\n");
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap: {
        vptr_t vptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap));

        ಪ್ರತಿಜ್ಞಾ(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE);
        cap_ಚೌಕಟ್ಟು_print_attrs_vptr(vptr, ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        ಬರೆ("%p_asid_pool\n", (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ನಿಲ್ಲಿಸು;
    }
    /* riscv specific caps */
    /* nothing */
    ಪೂರ್ವನಿಯೋಜಿತ: {
        ಬರೆ("[unknown cap %lu]\n", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_get_capType(cap));
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ obj_ಚೌಕಟ್ಟು_print_attrs(paddr_t paddr)
{
    ಬರೆ("(4k, paddr: 0x%p)\n", (ಶೂನ್ಯ *)paddr);
}

ಶೂನ್ಯ print_object_arch(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        ಬರೆ("%p_asid_pool = asid_pool ",
               (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
    /* riscv specific caps */
    /* nothing */
    ಪೂರ್ವನಿಯೋಜಿತ: {
        ಬರೆ("[unknown object %lu]\n", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_get_capType(cap));
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, ಪೂರ್ಣಾಂಕ level)
{
    ಶಬ್ದ_t pt_ಬಿಟ್ಟುಗಳು_ಎಡ = PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * level + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು;

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(pt_ಬಿಟ್ಟುಗಳು_ಎಡ + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1 << (pt_ಬಿಟ್ಟುಗಳು_ಎಡ))) {
        ಶಬ್ದ_t pt_ಸೂಚ್ಯಂಕ = ((i >> pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
        pte_t *pt_ಸ್ಥಾನ = pt + pt_ಸೂಚ್ಯಂಕ;
        ಆದರೆ (pte_ptr_get_valid(pt_ಸ್ಥಾನ)) {
            ಆದರೆ (level) { /* pt */
                ಬರೆ("pt_%p_%04lu = pt\n", pt_ಸ್ಥಾನ, pt_ಸೂಚ್ಯಂಕ);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(pt_ಸ್ಥಾನ), level - 1);
            } ಇಲ್ಲದಿದ್ದರೆ { /*ಚೌಕಟ್ಟು*/
                paddr_t paddr = pte_ptr_get_ppn(pt_ಸ್ಥಾನ);
                ಬರೆ("frame_%p_%04lu = frame ", pt_ಸ್ಥಾನ, pt_ಸೂಚ್ಯಂಕ);
                obj_ಚೌಕಟ್ಟು_print_attrs(paddr);
            }
        }
    }
}

ಶೂನ್ಯ obj_tcb_print_vtable(tcb_t *tcb)
{
    ಆದರೆ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_ದ_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        ಬರೆ("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_ಮಟ್ಟಗಳು - 1);
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

ಶೂನ್ಯ debug_capDL(ಶೂನ್ಯ)
{
    ಬರೆ("arch riscv\n");
    ಬರೆ("objects {\n");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    print_objects();
#ಆದರೆ_ಅಂತ್ಯ
    ಬರೆ("}\n");

    ಬರೆ("caps {\n");

    /* reset the seen list */
    reset_ನೋಡಿದ_list();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    print_caps();
    ಬರೆ("}\n");

    obj_irq_print_maps();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
