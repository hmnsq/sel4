/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>

ಶಬ್ದ_t get_tcb_sp(tcb_t *tcb)
{
    ಹಿಂತಿರುಗಿ tcb->tcbArch.tcbContext.registers[RSP];
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING

ಸ್ಥಿರ ಶೂನ್ಯ obj_ಚೌಕಟ್ಟು_print_attrs(paddr_t paddr, ಶಬ್ದ_t ಪುಟ_ಗಾತ್ರ);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr_2(ಶಬ್ದ_t vptr, vspace_ಬೇರು_t *vspace);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr(ಶಬ್ದ_t vptr, cap_t vspace);
ಸ್ಥಿರ ಶೂನ್ಯ obj_asidpool_print_attrs(cap_t asid_cap);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pdpt(pdpte_t *pdpt_ಸ್ಥಾನ);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pd(pde_t *pd_ಸ್ಥಾನ);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte_t *pt_ಸ್ಥಾನ);
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_impl(ಶಬ್ದ_t super_user, ಶಬ್ದ_t read_write, ಶಬ್ದ_t cache_disabled, ಶಬ್ದ_t xd);

/* use when only have access to pte of frames */
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pdpt(pdpte_t *pdpt_ಸ್ಥಾನ)
{
    cap_ಚೌಕಟ್ಟು_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdpt_ಸ್ಥಾನ),
                               pdpte_pdpte_1g_ptr_get_read_write(pdpt_ಸ್ಥಾನ),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdpt_ಸ್ಥಾನ),
                               pdpte_pdpte_1g_ptr_get_xd(pdpt_ಸ್ಥಾನ));
}
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pd(pde_t *pd_ಸ್ಥಾನ)
{
    cap_ಚೌಕಟ್ಟು_print_attrs_impl(pde_pde_large_ptr_get_super_user(pd_ಸ್ಥಾನ),
                               pde_pde_large_ptr_get_read_write(pd_ಸ್ಥಾನ),
                               pde_pde_large_ptr_get_cache_disabled(pd_ಸ್ಥಾನ),
                               pde_pde_large_ptr_get_xd(pd_ಸ್ಥಾನ));
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_pt(pte_t *pt_ಸ್ಥಾನ)
{
    cap_ಚೌಕಟ್ಟು_print_attrs_impl(pte_ptr_get_super_user(pt_ಸ್ಥಾನ),
                               pte_ptr_get_read_write(pt_ಸ್ಥಾನ),
                               pte_ptr_get_cache_disabled(pt_ಸ್ಥಾನ),
                               pte_ptr_get_xd(pt_ಸ್ಥಾನ));
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_impl(ಶಬ್ದ_t super_user, ಶಬ್ದ_t read_write, ಶಬ್ದ_t cache_disabled, ಶಬ್ದ_t xd)
{
    ಬರೆ("(");

    /* rights */
    ಆದರೆ (read_write) {
        ಬರೆ("RW");
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (super_user) {
        ಬರೆ("R");
    }

    ಆದರೆ (!xd) {
        ಬರೆ("X");
    }

    /* asid, mapping */

    ಆದರೆ (cache_disabled) {
        ಬರೆ(", uncached");
    }

    ಬರೆ(")\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ obj_ಚೌಕಟ್ಟು_print_attrs(paddr_t paddr, ಶಬ್ದ_t ಪುಟ_ಗಾತ್ರ)
{
    ಬರೆ("(");

    /* VM size */
    ಆಯ್ಕೆ (ಪುಟ_ಗಾತ್ರ) {
    ಸ್ಥಿತಿ se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು:
        ಬರೆ("1G");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು:
        ಬರೆ("2M");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು:
        ಬರೆ("4k");
        ನಿಲ್ಲಿಸು;
    }

    ಬರೆ(", paddr: 0x%p)\n", (ಶೂನ್ಯ *)paddr);
}

ಸ್ಥಿರ ಶೂನ್ಯ x86_64_obj_pt_print_slots(pde_t *pd_ಸ್ಥಾನ)
{
    paddr_t paddr;
    ಶಬ್ದ_t ಪುಟ_ಗಾತ್ರ;
    pte_t *pt = paddr_ಗಮ್ಯ_pptr(pde_pde_pt_ptr_get_pt_ತಳ_ವಿಳಾಸ(pd_ಸ್ಥಾನ));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pte_t *pt_ಸ್ಥಾನ = pt + GET_PT_ಸೂಚ್ಯಂಕ(i);

        ಆದರೆ (pte_ptr_get_present(pt_ಸ್ಥಾನ)) {
            paddr = pte_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pt_ಸ್ಥಾನ);
            ಪುಟ_ಗಾತ್ರ = se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು;
            ಬರೆ("frame_%p_%04lu = frame ", pt_ಸ್ಥಾನ, GET_PT_ಸೂಚ್ಯಂಕ(i));
            obj_ಚೌಕಟ್ಟು_print_attrs(paddr, ಪುಟ_ಗಾತ್ರ);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ x86_64_obj_pd_print_slots(pdpte_t *pdpt_ಸ್ಥಾನ)
{
    paddr_t paddr;
    ಶಬ್ದ_t ಪುಟ_ಗಾತ್ರ;
    pde_t *pd = paddr_ಗಮ್ಯ_pptr(pdpte_pdpte_pd_ptr_get_pd_ತಳ_ವಿಳಾಸ(pdpt_ಸ್ಥಾನ));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pde_t *pd_ಸ್ಥಾನ = pd + GET_PD_ಸೂಚ್ಯಂಕ(i);

        ಆದರೆ ((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ಸ್ಥಾನ) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_ಸ್ಥಾನ)) {
            paddr = pde_pde_large_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pd_ಸ್ಥಾನ);
            ಪುಟ_ಗಾತ್ರ = se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು;

            ಬರೆ("frame_%p_%04lu = frame ", pd_ಸ್ಥಾನ, GET_PD_ಸೂಚ್ಯಂಕ(i));
            obj_ಚೌಕಟ್ಟು_print_attrs(paddr, ಪುಟ_ಗಾತ್ರ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pde_pde_pt_ptr_get_present(pd_ಸ್ಥಾನ)) {
            ಬರೆ("pt_%p_%04lu = pt\n", pd_ಸ್ಥಾನ, GET_PD_ಸೂಚ್ಯಂಕ(i));
            x86_64_obj_pt_print_slots(pd_ಸ್ಥಾನ);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ x86_64_obj_pdpt_print_slots(pml4e_t *pml4_ಸ್ಥಾನ)
{
    paddr_t paddr;
    ಶಬ್ದ_t ಪುಟ_ಗಾತ್ರ;
    pdpte_t *pdpt = paddr_ಗಮ್ಯ_pptr(pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4_ಸ್ಥಾನ));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pdpte_t *pdpt_ಸ್ಥಾನ = pdpt + GET_PDPT_ಸೂಚ್ಯಂಕ(i);

        ಆದರೆ (pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpt_ಸ್ಥಾನ) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_ಸ್ಥಾನ)) {
            paddr = pdpte_pdpte_1g_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pdpt_ಸ್ಥಾನ);
            ಪುಟ_ಗಾತ್ರ = se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು;

            ಬರೆ("frame_%p_%04lu = frame ", pdpt_ಸ್ಥಾನ, GET_PDPT_ಸೂಚ್ಯಂಕ(i));
            obj_ಚೌಕಟ್ಟು_print_attrs(paddr, ಪುಟ_ಗಾತ್ರ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pdpte_pdpte_pd_ptr_get_present(pdpt_ಸ್ಥಾನ)) {
            ಬರೆ("pd_%p_%04lu = pd\n", pdpt_ಸ್ಥಾನ, GET_PDPT_ಸೂಚ್ಯಂಕ(i));
            x86_64_obj_pd_print_slots(pdpt_ಸ್ಥಾನ);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pml4e_t *pml4_ಸ್ಥಾನ = lookup_PML4_ಸ್ಥಾನ(pml4, i);
        ಆದರೆ (pml4e_ptr_get_present(pml4_ಸ್ಥಾನ)) {
            ಬರೆ("pdpt_%p_%04lu = pdpt\n", pml4_ಸ್ಥಾನ, GET_PML4_ಸೂಚ್ಯಂಕ(i));
            x86_64_obj_pdpt_print_slots(pml4_ಸ್ಥಾನ);
        }
    }
}

ಶೂನ್ಯ obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    ಆದರೆ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_ತಳ_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        ಬರೆ("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr_2(ಶಬ್ದ_t vptr, vspace_ಬೇರು_t *vspace)
{
    lookupPDPTSlot_ret_t pdpt_ಸ್ಥಾನ = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t pt_ಸ್ಥಾನ;
    lookupPDSlot_ret_t pd_ಸ್ಥಾನ;

    ಆದರೆ (pdpt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE &&
        pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ)) {
        ಬರೆ("frame_%p_%04lu ", pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ, GET_PDPT_ಸೂಚ್ಯಂಕ(vptr));
        cap_ಚೌಕಟ್ಟು_print_attrs_pdpt(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        pd_ಸ್ಥಾನ = lookupPDSlot(vspace, vptr);
        ಆದರೆ (pd_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE &&
            ((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ))) {
            ಬರೆ("frame_%p_%04lu ", pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ, GET_PD_ಸೂಚ್ಯಂಕ(vptr));
            cap_ಚೌಕಟ್ಟು_print_attrs_pd(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            pt_ಸ್ಥಾನ = lookupPTSlot(vspace, vptr);
            ಪ್ರತಿಜ್ಞಾ(pt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE && pte_ptr_get_present(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ));
            ಬರೆ("frame_%p_%04lu ", pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ, GET_PT_ಸೂಚ್ಯಂಕ(vptr));
            cap_ಚೌಕಟ್ಟು_print_attrs_pt(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_ಚೌಕಟ್ಟು_print_attrs_vptr(ಶಬ್ದ_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    cap_ಚೌಕಟ್ಟು_print_attrs_vptr_2(vptr, ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು);
}

ಶೂನ್ಯ print_cap_arch(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    /* arch specific caps */
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap: {
        asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap);
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap);
        ಆದರೆ (asid) {
            ಬರೆ("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr).pd_ಸ್ಥಾನ, GET_PD_ಸೂಚ್ಯಂಕ(vptr), (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("pt_%p_%04lu\n", lookupPDSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr).pd_ಸ್ಥಾನ, GET_PD_ಸೂಚ್ಯಂಕ(vptr));
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap: {
        asid_t asid = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(cap);
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ವಿಳಾಸ(cap);
        ಆದರೆ (asid) {
            ಬರೆ("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr).pdpt_ಸ್ಥಾನ, GET_PDPT_ಸೂಚ್ಯಂಕ(vptr), (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("pd_%p_%04lu\n", lookupPDPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr).pdpt_ಸ್ಥಾನ, GET_PDPT_ಸೂಚ್ಯಂಕ(vptr));
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_ವಿಳಾಸ(cap);
        ಆದರೆ (asid) {
            ಬರೆ("pdpt_%p_%04lu (asid: %lu)\n",
                   lookup_PML4_ಸ್ಥಾನ(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr), GET_PML4_ಸೂಚ್ಯಂಕ(vptr), (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("pdpt_%p_%04lu\n", lookup_PML4_ಸ್ಥಾನ(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr), GET_PML4_ಸೂಚ್ಯಂಕ(vptr));
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        ಆದರೆ (asid) {
            ಬರೆ("%p_pd (asid: %lu)\n",
                   ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)asid);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("%p_pd\n", ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು);
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
        cap_ಚೌಕಟ್ಟು_print_attrs_vptr_2(vptr, ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        ಬರೆ("%p_asid_pool\n", (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ನಿಲ್ಲಿಸು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ cap_vcpu_cap: {
        ಬರೆ("%p_vcpu\n", (ಶೂನ್ಯ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    /* X86 specific caps */
    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_cap: {
        ಬರೆ("%p%p_io_port\n", (ಶೂನ್ಯ *)cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(cap),
               (ಶೂನ್ಯ *)cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(cap));
        ನಿಲ್ಲಿಸು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ cap_io_space_cap: {
        ಬರೆ("%p_io_space\n", (ಶೂನ್ಯ *)cap_io_space_cap_get_capPCIDevice(cap));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap: {
        ಬರೆ("%p_iopt\n", (ಶೂನ್ಯ *)cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(cap));
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ: {
        ಬರೆ("[unknown cap %lu]\n", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_get_capType(cap));
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    ಬರೆ("(asid_high: 0x%lx)\n", ASID_ಮೇಲಿನ(asid));
}

ಶೂನ್ಯ print_object_arch(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    /* arch specific objects */
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
    ಸ್ಥಿತಿ cap_pdpt_cap:
    ಸ್ಥಿತಿ cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        ಬರೆ("%p_asid_pool = asid_pool ",
               (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ cap_vcpu_cap: {
        ಬರೆ("%p_vcpu = vcpu\n", (ಶೂನ್ಯ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    /* X86 specific caps */
    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_cap: {
        ಬರೆ("%p%p_io_port = io_port ",
               (ಶೂನ್ಯ *)cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(cap),
               (ಶೂನ್ಯ *)cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(cap));
        x86_obj_ioports_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ cap_io_space_cap: {
        ಬರೆ("%p_io_space = io_space ", (ಶೂನ್ಯ *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap: {
        ಬರೆ("%p_iopt = iopt ", (ಶೂನ್ಯ *)cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ: {
        ಬರೆ("[unknown object %lu]\n", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_get_capType(cap));
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಶೂನ್ಯ print_ipc_ಬಫರ್_ಸ್ಥಾನ(tcb_t *tcb)
{
    ಶಬ್ದ_t vptr = tcb->tcbIPCBuffer;
    ಬರೆ("ipc_buffer_slot: ");
    cap_ಚೌಕಟ್ಟು_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ಸ್ಥಿರ ಶೂನ್ಯ x86_64_cap_pt_print_slots(pde_t *pd_ಸ್ಥಾನ, vptr_t vptr)
{
    pte_t *pt = paddr_ಗಮ್ಯ_pptr(pde_pde_pt_ptr_get_pt_ತಳ_ವಿಳಾಸ(pd_ಸ್ಥಾನ));
    ಬರೆ("pt_%p_%04lu {\n", pd_ಸ್ಥಾನ, GET_PD_ಸೂಚ್ಯಂಕ(vptr));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pte_t *pt_ಸ್ಥಾನ = pt + GET_PT_ಸೂಚ್ಯಂಕ(i);

        ಆದರೆ (pte_ptr_get_present(pt_ಸ್ಥಾನ)) {
            ಬರೆ("0x%lx: frame_%p_%04lu ", GET_PT_ಸೂಚ್ಯಂಕ(i), pt_ಸ್ಥಾನ, GET_PT_ಸೂಚ್ಯಂಕ(i));
            cap_ಚೌಕಟ್ಟು_print_attrs_pt(pt_ಸ್ಥಾನ);
        }
    }
    ಬರೆ("}\n"); /* pt */
}

ಸ್ಥಿರ ಶೂನ್ಯ x86_64_cap_pd_print_slots(pdpte_t *pdpt_ಸ್ಥಾನ, vptr_t vptr)
{
    pde_t *pd = paddr_ಗಮ್ಯ_pptr(pdpte_pdpte_pd_ptr_get_pd_ತಳ_ವಿಳಾಸ(pdpt_ಸ್ಥಾನ));
    ಬರೆ("pd_%p_%04lu {\n", pdpt_ಸ್ಥಾನ, GET_PDPT_ಸೂಚ್ಯಂಕ(vptr));

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pde_t *pd_ಸ್ಥಾನ = pd + GET_PD_ಸೂಚ್ಯಂಕ(i);

        ಆದರೆ ((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ಸ್ಥಾನ) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_ಸ್ಥಾನ)) {
            ಬರೆ("0x%lx: frame_%p_%04lu ", GET_PD_ಸೂಚ್ಯಂಕ(i), pd_ಸ್ಥಾನ, GET_PD_ಸೂಚ್ಯಂಕ(i));
            cap_ಚೌಕಟ್ಟು_print_attrs_pd(pd_ಸ್ಥಾನ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pde_pde_pt_ptr_get_present(pd_ಸ್ಥಾನ)) {
            ಬರೆ("0x%lx: pt_%p_%04lu\n", GET_PD_ಸೂಚ್ಯಂಕ(i), pd_ಸ್ಥಾನ, GET_PD_ಸೂಚ್ಯಂಕ(i));
        }
    }
    ಬರೆ("}\n"); /* pd */

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pde_t *pd_ಸ್ಥಾನ = pd + GET_PD_ಸೂಚ್ಯಂಕ(i);
        ಆದರೆ ((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ಸ್ಥಾನ) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_ಸ್ಥಾನ)) {
            x86_64_cap_pt_print_slots(pd_ಸ್ಥಾನ, i);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ x86_64_cap_pdpt_print_slots(pml4e_t *pml4_ಸ್ಥಾನ, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_ಗಮ್ಯ_pptr(pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4_ಸ್ಥಾನ));

    ಬರೆ("pdpt_%p_%04lu {\n", pml4_ಸ್ಥಾನ, GET_PML4_ಸೂಚ್ಯಂಕ(vptr));
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pdpte_t *pdpt_ಸ್ಥಾನ = pdpt + GET_PDPT_ಸೂಚ್ಯಂಕ(i);

        ಆದರೆ (pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpt_ಸ್ಥಾನ) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_ಸ್ಥಾನ)) {
            ಬರೆ("0x%lx: frame_%p_%04lu ", GET_PDPT_ಸೂಚ್ಯಂಕ(i), pdpt_ಸ್ಥಾನ, GET_PDPT_ಸೂಚ್ಯಂಕ(i));
            cap_ಚೌಕಟ್ಟು_print_attrs_pdpt(pdpt_ಸ್ಥಾನ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pdpte_pdpte_pd_ptr_get_present(pdpt_ಸ್ಥಾನ)) {
            ಬರೆ("0x%lx: pd_%p_%04lu\n", GET_PDPT_ಸೂಚ್ಯಂಕ(i), pdpt_ಸ್ಥಾನ, GET_PDPT_ಸೂಚ್ಯಂಕ(i));
        }
    }
    ಬರೆ("}\n"); /* pdpt */

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pdpte_t *pdpt_ಸ್ಥಾನ = pdpt + GET_PDPT_ಸೂಚ್ಯಂಕ(i);

        ಆದರೆ (pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpt_ಸ್ಥಾನ) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdpt_ಸ್ಥಾನ)) {
            x86_64_cap_pd_print_slots(pdpt_ಸ್ಥಾನ, i);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    ಬರೆ("%p_pd {\n", pml4);
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pml4e_t *pml4_ಸ್ಥಾನ = lookup_PML4_ಸ್ಥಾನ(pml4, i);
        ಆದರೆ (pml4e_ptr_get_present(pml4_ಸ್ಥಾನ)) {
            ಬರೆ("0x%lx: pdpt_%p_%04lu\n", GET_PML4_ಸೂಚ್ಯಂಕ(i), pml4_ಸ್ಥಾನ, GET_PML4_ಸೂಚ್ಯಂಕ(i));
        }
    }
    ಬರೆ("}\n"); /* pml4 */

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ + PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += (1UL << PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        pml4e_t *pml4_ಸ್ಥಾನ = lookup_PML4_ಸ್ಥಾನ(pml4, i);
        ಆದರೆ (pml4e_ptr_get_present(pml4_ಸ್ಥಾನ)) {
            x86_64_cap_pdpt_print_slots(pml4_ಸ್ಥಾನ, i);
        }
    }
}

ಶೂನ್ಯ obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    ಆದರೆ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_ತಳ_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

ಶೂನ್ಯ debug_capDL(ಶೂನ್ಯ)
{
    ಬರೆ("arch x86_64\n");
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
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
