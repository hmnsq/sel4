/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/capdl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>

ਸ਼ਬਦ_t get_tcb_sp(tcb_t *tcb)
{
    ਵਾਪਸ tcb->tcbArch.tcbContext.registers[RSP];
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING

ਸਥਿਰ ਖਾਲੀ obj_ਚੌਖਟਾ_print_attrs(paddr_t paddr, ਸ਼ਬਦ_t ਪੰਨਾ_ਮਾਪ);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr_2(ਸ਼ਬਦ_t vptr, vspace_ਜੜ੍ਹ_t *vspace);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr(ਸ਼ਬਦ_t vptr, cap_t vspace);
ਸਥਿਰ ਖਾਲੀ obj_asidpool_print_attrs(cap_t asid_cap);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pdpt(pdpte_t *pdpt_ਥਾਂ);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pd(pde_t *pd_ਥਾਂ);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pt(pte_t *pt_ਥਾਂ);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_impl(ਸ਼ਬਦ_t super_user, ਸ਼ਬਦ_t read_write, ਸ਼ਬਦ_t cache_disabled, ਸ਼ਬਦ_t xd);

/* use when only have access to pte of frames */
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pdpt(pdpte_t *pdpt_ਥਾਂ)
{
    cap_ਚੌਖਟਾ_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdpt_ਥਾਂ),
                               pdpte_pdpte_1g_ptr_get_read_write(pdpt_ਥਾਂ),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdpt_ਥਾਂ),
                               pdpte_pdpte_1g_ptr_get_xd(pdpt_ਥਾਂ));
}
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pd(pde_t *pd_ਥਾਂ)
{
    cap_ਚੌਖਟਾ_print_attrs_impl(pde_pde_large_ptr_get_super_user(pd_ਥਾਂ),
                               pde_pde_large_ptr_get_read_write(pd_ਥਾਂ),
                               pde_pde_large_ptr_get_cache_disabled(pd_ਥਾਂ),
                               pde_pde_large_ptr_get_xd(pd_ਥਾਂ));
}

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pt(pte_t *pt_ਥਾਂ)
{
    cap_ਚੌਖਟਾ_print_attrs_impl(pte_ptr_get_super_user(pt_ਥਾਂ),
                               pte_ptr_get_read_write(pt_ਥਾਂ),
                               pte_ptr_get_cache_disabled(pt_ਥਾਂ),
                               pte_ptr_get_xd(pt_ਥਾਂ));
}

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_impl(ਸ਼ਬਦ_t super_user, ਸ਼ਬਦ_t read_write, ਸ਼ਬਦ_t cache_disabled, ਸ਼ਬਦ_t xd)
{
    ਲਿਖੋ("(");

    /* rights */
    ਜੇ (read_write) {
        ਲਿਖੋ("RW");
    } ਨਹੀਂਤਾਂ ਜੇ (super_user) {
        ਲਿਖੋ("R");
    }

    ਜੇ (!xd) {
        ਲਿਖੋ("X");
    }

    /* asid, mapping */

    ਜੇ (cache_disabled) {
        ਲਿਖੋ(", uncached");
    }

    ਲਿਖੋ(")\n");
}

ਸਥਿਰ ਖਾਲੀ obj_ਚੌਖਟਾ_print_attrs(paddr_t paddr, ਸ਼ਬਦ_t ਪੰਨਾ_ਮਾਪ)
{
    ਲਿਖੋ("(");

    /* VM size */
    ਚੋਣ (ਪੰਨਾ_ਮਾਪ) {
    ਹਾਲਤ se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ:
        ਲਿਖੋ("1G");
        ਰੋਕੋ;
    ਹਾਲਤ se_L4_Large_ਪੰਨਾ_ਬਿੱਟ:
        ਲਿਖੋ("2M");
        ਰੋਕੋ;
    ਹਾਲਤ se_L4_ਪੰਨਾ_ਬਿੱਟ:
        ਲਿਖੋ("4k");
        ਰੋਕੋ;
    }

    ਲਿਖੋ(", paddr: 0x%p)\n", (ਖਾਲੀ *)paddr);
}

ਸਥਿਰ ਖਾਲੀ x86_64_obj_pt_print_slots(pde_t *pd_ਥਾਂ)
{
    paddr_t paddr;
    ਸ਼ਬਦ_t ਪੰਨਾ_ਮਾਪ;
    pte_t *pt = paddr_ਮੰਜ਼ਿਲ_pptr(pde_pde_pt_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pd_ਥਾਂ));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PT_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pte_t *pt_ਥਾਂ = pt + GET_PT_ਸੂਚਕਾਂਕ(i);

        ਜੇ (pte_ptr_get_present(pt_ਥਾਂ)) {
            paddr = pte_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pt_ਥਾਂ);
            ਪੰਨਾ_ਮਾਪ = se_L4_ਪੰਨਾ_ਬਿੱਟ;
            ਲਿਖੋ("frame_%p_%04lu = frame ", pt_ਥਾਂ, GET_PT_ਸੂਚਕਾਂਕ(i));
            obj_ਚੌਖਟਾ_print_attrs(paddr, ਪੰਨਾ_ਮਾਪ);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ x86_64_obj_pd_print_slots(pdpte_t *pdpt_ਥਾਂ)
{
    paddr_t paddr;
    ਸ਼ਬਦ_t ਪੰਨਾ_ਮਾਪ;
    pde_t *pd = paddr_ਮੰਜ਼ਿਲ_pptr(pdpte_pdpte_pd_ptr_get_pd_ਅਧਾਰ_ਪਤਾ(pdpt_ਥਾਂ));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pde_t *pd_ਥਾਂ = pd + GET_PD_ਸੂਚਕਾਂਕ(i);

        ਜੇ ((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ਥਾਂ) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_ਥਾਂ)) {
            paddr = pde_pde_large_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pd_ਥਾਂ);
            ਪੰਨਾ_ਮਾਪ = se_L4_Large_ਪੰਨਾ_ਬਿੱਟ;

            ਲਿਖੋ("frame_%p_%04lu = frame ", pd_ਥਾਂ, GET_PD_ਸੂਚਕਾਂਕ(i));
            obj_ਚੌਖਟਾ_print_attrs(paddr, ਪੰਨਾ_ਮਾਪ);

        } ਨਹੀਂਤਾਂ ਜੇ (pde_pde_pt_ptr_get_present(pd_ਥਾਂ)) {
            ਲਿਖੋ("pt_%p_%04lu = pt\n", pd_ਥਾਂ, GET_PD_ਸੂਚਕਾਂਕ(i));
            x86_64_obj_pt_print_slots(pd_ਥਾਂ);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ x86_64_obj_pdpt_print_slots(pml4e_t *pml4_ਥਾਂ)
{
    paddr_t paddr;
    ਸ਼ਬਦ_t ਪੰਨਾ_ਮਾਪ;
    pdpte_t *pdpt = paddr_ਮੰਜ਼ਿਲ_pptr(pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4_ਥਾਂ));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pdpte_t *pdpt_ਥਾਂ = pdpt + GET_PDPT_ਸੂਚਕਾਂਕ(i);

        ਜੇ (pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpt_ਥਾਂ) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_ਥਾਂ)) {
            paddr = pdpte_pdpte_1g_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pdpt_ਥਾਂ);
            ਪੰਨਾ_ਮਾਪ = se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ;

            ਲਿਖੋ("frame_%p_%04lu = frame ", pdpt_ਥਾਂ, GET_PDPT_ਸੂਚਕਾਂਕ(i));
            obj_ਚੌਖਟਾ_print_attrs(paddr, ਪੰਨਾ_ਮਾਪ);

        } ਨਹੀਂਤਾਂ ਜੇ (pdpte_pdpte_pd_ptr_get_present(pdpt_ਥਾਂ)) {
            ਲਿਖੋ("pd_%p_%04lu = pd\n", pdpt_ਥਾਂ, GET_PDPT_ਸੂਚਕਾਂਕ(i));
            x86_64_obj_pd_print_slots(pdpt_ਥਾਂ);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pml4e_t *pml4_ਥਾਂ = lookup_PML4_ਥਾਂ(pml4, i);
        ਜੇ (pml4e_ptr_get_present(pml4_ਥਾਂ)) {
            ਲਿਖੋ("pdpt_%p_%04lu = pdpt\n", pml4_ਥਾਂ, GET_PML4_ਸੂਚਕਾਂਕ(i));
            x86_64_obj_pdpt_print_slots(pml4_ਥਾਂ);
        }
    }
}

ਖਾਲੀ obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    ਜੇ (is_Valid_VTable_ਜੜ੍ਹ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        ਲਿਖੋ("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr_2(ਸ਼ਬਦ_t vptr, vspace_ਜੜ੍ਹ_t *vspace)
{
    lookupPDPTSlot_ret_t pdpt_ਥਾਂ = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t pt_ਥਾਂ;
    lookupPDSlot_ret_t pd_ਥਾਂ;

    ਜੇ (pdpt_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE &&
        pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpt_ਥਾਂ.pdpt_ਥਾਂ) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_ਥਾਂ.pdpt_ਥਾਂ)) {
        ਲਿਖੋ("frame_%p_%04lu ", pdpt_ਥਾਂ.pdpt_ਥਾਂ, GET_PDPT_ਸੂਚਕਾਂਕ(vptr));
        cap_ਚੌਖਟਾ_print_attrs_pdpt(pdpt_ਥਾਂ.pdpt_ਥਾਂ);
    } ਨਹੀਂਤਾਂ {
        pd_ਥਾਂ = lookupPDSlot(vspace, vptr);
        ਜੇ (pd_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE &&
            ((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ਥਾਂ.pd_ਥਾਂ) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_ਥਾਂ.pd_ਥਾਂ))) {
            ਲਿਖੋ("frame_%p_%04lu ", pd_ਥਾਂ.pd_ਥਾਂ, GET_PD_ਸੂਚਕਾਂਕ(vptr));
            cap_ਚੌਖਟਾ_print_attrs_pd(pd_ਥਾਂ.pd_ਥਾਂ);
        } ਨਹੀਂਤਾਂ {
            pt_ਥਾਂ = lookupPTSlot(vspace, vptr);
            ਪ੍ਰਤਿਜ੍ਞਾ(pt_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE && pte_ptr_get_present(pt_ਥਾਂ.pt_ਥਾਂ));
            ਲਿਖੋ("frame_%p_%04lu ", pt_ਥਾਂ.pt_ਥਾਂ, GET_PT_ਸੂਚਕਾਂਕ(vptr));
            cap_ਚੌਖਟਾ_print_attrs_pt(pt_ਥਾਂ.pt_ਥਾਂ);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr(ਸ਼ਬਦ_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    cap_ਚੌਖਟਾ_print_attrs_vptr_2(vptr, ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ);
}

ਖਾਲੀ print_cap_arch(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    /* arch specific caps */
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap: {
        asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap);
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap);
        ਜੇ (asid) {
            ਲਿਖੋ("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr).pd_ਥਾਂ, GET_PD_ਸੂਚਕਾਂਕ(vptr), (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("pt_%p_%04lu\n", lookupPDSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr).pd_ਥਾਂ, GET_PD_ਸੂਚਕਾਂਕ(vptr));
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap: {
        asid_t asid = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(cap);
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ਪਤਾ(cap);
        ਜੇ (asid) {
            ਲਿਖੋ("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr).pdpt_ਥਾਂ, GET_PDPT_ਸੂਚਕਾਂਕ(vptr), (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("pd_%p_%04lu\n", lookupPDPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr).pdpt_ਥਾਂ, GET_PDPT_ਸੂਚਕਾਂਕ(vptr));
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_ਪਤਾ(cap);
        ਜੇ (asid) {
            ਲਿਖੋ("pdpt_%p_%04lu (asid: %lu)\n",
                   lookup_PML4_ਥਾਂ(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr), GET_PML4_ਸੂਚਕਾਂਕ(vptr), (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("pdpt_%p_%04lu\n", lookup_PML4_ਥਾਂ(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr), GET_PML4_ਸੂਚਕਾਂਕ(vptr));
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        ਜੇ (asid) {
            ਲਿਖੋ("%p_pd (asid: %lu)\n",
                   ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("%p_pd\n", ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_asid_control_cap: {
        /* only one in the system */
        ਲਿਖੋ("asid_control\n");
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap: {
        vptr_t vptr = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਲੱਭੋ_ret.ਸਥਿਤੀ == EXCEPTION_NONE);
        cap_ਚੌਖਟਾ_print_attrs_vptr_2(vptr, ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ);
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_asid_pool_cap: {
        ਲਿਖੋ("%p_asid_pool\n", (ਖਾਲੀ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ cap_vcpu_cap: {
        ਲਿਖੋ("%p_vcpu\n", (ਖਾਲੀ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    /* X86 specific caps */
    ਹਾਲਤ cap_io_ਪੋਰਟ_cap: {
        ਲਿਖੋ("%p%p_io_port\n", (ਖਾਲੀ *)cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(cap),
               (ਖਾਲੀ *)cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ cap_io_space_cap: {
        ਲਿਖੋ("%p_io_space\n", (ਖਾਲੀ *)cap_io_space_cap_get_capPCIDevice(cap));
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap: {
        ਲਿਖੋ("%p_iopt\n", (ਖਾਲੀ *)cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਮੂਲ: {
        ਲਿਖੋ("[unknown cap %lu]\n", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_get_capType(cap));
        ਰੋਕੋ;
    }
    }
}

ਸਥਿਰ ਖਾਲੀ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    ਲਿਖੋ("(asid_high: 0x%lx)\n", ASID_ਉੱਚਾ(asid));
}

ਖਾਲੀ print_object_arch(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    /* arch specific objects */
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
    ਹਾਲਤ cap_pdpt_cap:
    ਹਾਲਤ cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ਰੋਕੋ;

    ਹਾਲਤ cap_asid_pool_cap: {
        ਲਿਖੋ("%p_asid_pool = asid_pool ",
               (ਖਾਲੀ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ cap_vcpu_cap: {
        ਲਿਖੋ("%p_vcpu = vcpu\n", (ਖਾਲੀ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    /* X86 specific caps */
    ਹਾਲਤ cap_io_ਪੋਰਟ_cap: {
        ਲਿਖੋ("%p%p_io_port = io_port ",
               (ਖਾਲੀ *)cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(cap),
               (ਖਾਲੀ *)cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(cap));
        x86_obj_ioports_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ cap_io_space_cap: {
        ਲਿਖੋ("%p_io_space = io_space ", (ਖਾਲੀ *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap: {
        ਲਿਖੋ("%p_iopt = iopt ", (ਖਾਲੀ *)cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਮੂਲ: {
        ਲਿਖੋ("[unknown object %lu]\n", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_get_capType(cap));
        ਰੋਕੋ;
    }
    }
}

ਖਾਲੀ print_ipc_ਬਫਰ_ਥਾਂ(tcb_t *tcb)
{
    ਸ਼ਬਦ_t vptr = tcb->tcbIPCBuffer;
    ਲਿਖੋ("ipc_buffer_slot: ");
    cap_ਚੌਖਟਾ_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ਸਥਿਰ ਖਾਲੀ x86_64_cap_pt_print_slots(pde_t *pd_ਥਾਂ, vptr_t vptr)
{
    pte_t *pt = paddr_ਮੰਜ਼ਿਲ_pptr(pde_pde_pt_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pd_ਥਾਂ));
    ਲਿਖੋ("pt_%p_%04lu {\n", pd_ਥਾਂ, GET_PD_ਸੂਚਕਾਂਕ(vptr));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PT_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pte_t *pt_ਥਾਂ = pt + GET_PT_ਸੂਚਕਾਂਕ(i);

        ਜੇ (pte_ptr_get_present(pt_ਥਾਂ)) {
            ਲਿਖੋ("0x%lx: frame_%p_%04lu ", GET_PT_ਸੂਚਕਾਂਕ(i), pt_ਥਾਂ, GET_PT_ਸੂਚਕਾਂਕ(i));
            cap_ਚੌਖਟਾ_print_attrs_pt(pt_ਥਾਂ);
        }
    }
    ਲਿਖੋ("}\n"); /* pt */
}

ਸਥਿਰ ਖਾਲੀ x86_64_cap_pd_print_slots(pdpte_t *pdpt_ਥਾਂ, vptr_t vptr)
{
    pde_t *pd = paddr_ਮੰਜ਼ਿਲ_pptr(pdpte_pdpte_pd_ptr_get_pd_ਅਧਾਰ_ਪਤਾ(pdpt_ਥਾਂ));
    ਲਿਖੋ("pd_%p_%04lu {\n", pdpt_ਥਾਂ, GET_PDPT_ਸੂਚਕਾਂਕ(vptr));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pde_t *pd_ਥਾਂ = pd + GET_PD_ਸੂਚਕਾਂਕ(i);

        ਜੇ ((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ਥਾਂ) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_ਥਾਂ)) {
            ਲਿਖੋ("0x%lx: frame_%p_%04lu ", GET_PD_ਸੂਚਕਾਂਕ(i), pd_ਥਾਂ, GET_PD_ਸੂਚਕਾਂਕ(i));
            cap_ਚੌਖਟਾ_print_attrs_pd(pd_ਥਾਂ);

        } ਨਹੀਂਤਾਂ ਜੇ (pde_pde_pt_ptr_get_present(pd_ਥਾਂ)) {
            ਲਿਖੋ("0x%lx: pt_%p_%04lu\n", GET_PD_ਸੂਚਕਾਂਕ(i), pd_ਥਾਂ, GET_PD_ਸੂਚਕਾਂਕ(i));
        }
    }
    ਲਿਖੋ("}\n"); /* pd */

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pde_t *pd_ਥਾਂ = pd + GET_PD_ਸੂਚਕਾਂਕ(i);
        ਜੇ ((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ਥਾਂ) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_ਥਾਂ)) {
            x86_64_cap_pt_print_slots(pd_ਥਾਂ, i);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ x86_64_cap_pdpt_print_slots(pml4e_t *pml4_ਥਾਂ, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_ਮੰਜ਼ਿਲ_pptr(pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4_ਥਾਂ));

    ਲਿਖੋ("pdpt_%p_%04lu {\n", pml4_ਥਾਂ, GET_PML4_ਸੂਚਕਾਂਕ(vptr));
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pdpte_t *pdpt_ਥਾਂ = pdpt + GET_PDPT_ਸੂਚਕਾਂਕ(i);

        ਜੇ (pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpt_ਥਾਂ) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_ਥਾਂ)) {
            ਲਿਖੋ("0x%lx: frame_%p_%04lu ", GET_PDPT_ਸੂਚਕਾਂਕ(i), pdpt_ਥਾਂ, GET_PDPT_ਸੂਚਕਾਂਕ(i));
            cap_ਚੌਖਟਾ_print_attrs_pdpt(pdpt_ਥਾਂ);

        } ਨਹੀਂਤਾਂ ਜੇ (pdpte_pdpte_pd_ptr_get_present(pdpt_ਥਾਂ)) {
            ਲਿਖੋ("0x%lx: pd_%p_%04lu\n", GET_PDPT_ਸੂਚਕਾਂਕ(i), pdpt_ਥਾਂ, GET_PDPT_ਸੂਚਕਾਂਕ(i));
        }
    }
    ਲਿਖੋ("}\n"); /* pdpt */

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pdpte_t *pdpt_ਥਾਂ = pdpt + GET_PDPT_ਸੂਚਕਾਂਕ(i);

        ਜੇ (pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpt_ਥਾਂ) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdpt_ਥਾਂ)) {
            x86_64_cap_pd_print_slots(pdpt_ਥਾਂ, i);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    ਲਿਖੋ("%p_pd {\n", pml4);
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pml4e_t *pml4_ਥਾਂ = lookup_PML4_ਥਾਂ(pml4, i);
        ਜੇ (pml4e_ptr_get_present(pml4_ਥਾਂ)) {
            ਲਿਖੋ("0x%lx: pdpt_%p_%04lu\n", GET_PML4_ਸੂਚਕਾਂਕ(i), pml4_ਥਾਂ, GET_PML4_ਸੂਚਕਾਂਕ(i));
        }
    }
    ਲਿਖੋ("}\n"); /* pml4 */

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ + PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1UL << PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        pml4e_t *pml4_ਥਾਂ = lookup_PML4_ਥਾਂ(pml4, i);
        ਜੇ (pml4e_ptr_get_present(pml4_ਥਾਂ)) {
            x86_64_cap_pdpt_print_slots(pml4_ਥਾਂ, i);
        }
    }
}

ਖਾਲੀ obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    ਜੇ (is_Valid_VTable_ਜੜ੍ਹ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

ਖਾਲੀ debug_capDL(ਖਾਲੀ)
{
    ਲਿਖੋ("arch x86_64\n");
    ਲਿਖੋ("objects {\n");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    print_objects();
#ਜੇ_ਅੰਤ
    ਲਿਖੋ("}\n");

    ਲਿਖੋ("caps {\n");

    /* reset the seen list */
    reset_ਵੇਖਿਆ_list();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    print_caps();
    ਲਿਖੋ("}\n");

    obj_irq_print_maps();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
