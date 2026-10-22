/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_DEBUG_BUILD

#अन्तर्भाव <arch/machine/capdl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    पटिनिवत्त tcb->tcbArch.tcbContext.registers[RSP];
}

#ifdef CONFIG_PRINTING

स्थिर शून्य obj_frame_print_attrs(paddr_t paddr, word_t page_size);
स्थिर शून्य _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूलम्_t *vspace);
स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);
स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap);
स्थिर शून्य cap_frame_print_attrs_pdpt(pdpte_t *pdptSlot);
स्थिर शून्य cap_frame_print_attrs_pd(pde_t *pdSlot);
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot);
स्थिर शून्य cap_frame_print_attrs_impl(word_t super_user, word_t read_write, word_t cache_disabled, word_t xd);

/* use when only have access to pte of frames */
स्थिर शून्य cap_frame_print_attrs_pdpt(pdpte_t *pdptSlot)
{
    cap_frame_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdptSlot),
                               pdpte_pdpte_1g_ptr_get_read_write(pdptSlot),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdptSlot),
                               pdpte_pdpte_1g_ptr_get_xd(pdptSlot));
}
स्थिर शून्य cap_frame_print_attrs_pd(pde_t *pdSlot)
{
    cap_frame_print_attrs_impl(pde_pde_large_ptr_get_super_user(pdSlot),
                               pde_pde_large_ptr_get_read_write(pdSlot),
                               pde_pde_large_ptr_get_cache_disabled(pdSlot),
                               pde_pde_large_ptr_get_xd(pdSlot));
}

स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_ptr_get_super_user(ptSlot),
                               pte_ptr_get_read_write(ptSlot),
                               pte_ptr_get_cache_disabled(ptSlot),
                               pte_ptr_get_xd(ptSlot));
}

स्थिर शून्य cap_frame_print_attrs_impl(word_t super_user, word_t read_write, word_t cache_disabled, word_t xd)
{
    printf("(");

    /* rights */
    सचे (read_write) {
        printf("RW");
    } अञ्ञथा सचे (super_user) {
        printf("R");
    }

    सचे (!xd) {
        printf("X");
    }

    /* asid, mapping */

    सचे (cache_disabled) {
        printf(", uncached");
    }

    printf(")\n");
}

स्थिर शून्य obj_frame_print_attrs(paddr_t paddr, word_t page_size)
{
    printf("(");

    /* VM size */
    विकल्प (page_size) {
    पक्ष seL4_HugePageBits:
        printf("1G");
        भङ्ग;
    पक्ष seL4_LargePageBits:
        printf("2M");
        भङ्ग;
    पक्ष seL4_PageBits:
        printf("4k");
        भङ्ग;
    }

    printf(", paddr: 0x%p)\n", (शून्य *)paddr);
}

स्थिर शून्य x86_64_obj_pt_print_slots(pde_t *pdSlot)
{
    paddr_t paddr;
    word_t page_size;
    pte_t *pt = paddr_to_pptr(pde_pde_pt_ptr_get_pt_base_address(pdSlot));

    अत्थाय (word_t i = 0; i < BIT(PT_INDEX_OFFSET + PT_INDEX_BITS); i += (1UL << PT_INDEX_OFFSET)) {
        pte_t *ptSlot = pt + GET_PT_INDEX(i);

        सचे (pte_ptr_get_present(ptSlot)) {
            paddr = pte_ptr_get_page_base_address(ptSlot);
            page_size = seL4_PageBits;
            printf("frame_%p_%04lu = frame ", ptSlot, GET_PT_INDEX(i));
            obj_frame_print_attrs(paddr, page_size);
        }
    }
}

स्थिर शून्य x86_64_obj_pd_print_slots(pdpte_t *pdptSlot)
{
    paddr_t paddr;
    word_t page_size;
    pde_t *pd = paddr_to_pptr(pdpte_pdpte_pd_ptr_get_pd_base_address(pdptSlot));

    अत्थाय (word_t i = 0; i < BIT(PD_INDEX_OFFSET + PD_INDEX_BITS); i += (1UL << PD_INDEX_OFFSET)) {
        pde_t *pdSlot = pd + GET_PD_INDEX(i);

        सचे ((pde_ptr_get_page_size(pdSlot) == pde_pde_large) && pde_pde_large_ptr_get_present(pdSlot)) {
            paddr = pde_pde_large_ptr_get_page_base_address(pdSlot);
            page_size = seL4_LargePageBits;

            printf("frame_%p_%04lu = frame ", pdSlot, GET_PD_INDEX(i));
            obj_frame_print_attrs(paddr, page_size);

        } अञ्ञथा सचे (pde_pde_pt_ptr_get_present(pdSlot)) {
            printf("pt_%p_%04lu = pt\n", pdSlot, GET_PD_INDEX(i));
            x86_64_obj_pt_print_slots(pdSlot);
        }
    }
}

स्थिर शून्य x86_64_obj_pdpt_print_slots(pml4e_t *pml4Slot)
{
    paddr_t paddr;
    word_t page_size;
    pdpte_t *pdpt = paddr_to_pptr(pml4e_ptr_get_pdpt_base_address(pml4Slot));

    अत्थाय (word_t i = 0; i < BIT(PDPT_INDEX_OFFSET + PDPT_INDEX_BITS); i += (1UL << PDPT_INDEX_OFFSET)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_INDEX(i);

        सचे (pdpte_ptr_get_page_size(pdptSlot) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdptSlot)) {
            paddr = pdpte_pdpte_1g_ptr_get_page_base_address(pdptSlot);
            page_size = seL4_HugePageBits;

            printf("frame_%p_%04lu = frame ", pdptSlot, GET_PDPT_INDEX(i));
            obj_frame_print_attrs(paddr, page_size);

        } अञ्ञथा सचे (pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            printf("pd_%p_%04lu = pd\n", pdptSlot, GET_PDPT_INDEX(i));
            x86_64_obj_pd_print_slots(pdptSlot);
        }
    }
}

स्थिर शून्य x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    अत्थाय (word_t i = 0; i < BIT(PML4_INDEX_OFFSET + PML4_INDEX_BITS); i += (1UL << PML4_INDEX_OFFSET)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        सचे (pml4e_ptr_get_present(pml4Slot)) {
            printf("pdpt_%p_%04lu = pdpt\n", pml4Slot, GET_PML4_INDEX(i));
            x86_64_obj_pdpt_print_slots(pml4Slot);
        }
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    सचे (is_Valid_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        printf("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
स्थिर शून्य _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूलम्_t *vspace)
{
    lookupPDPTSlot_ret_t pdptSlot = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t ptSlot;
    lookupPDSlot_ret_t pdSlot;

    सचे (pdptSlot.status == EXCEPTION_NONE &&
        pdpte_ptr_get_page_size(pdptSlot.pdptSlot) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdptSlot.pdptSlot)) {
        printf("frame_%p_%04lu ", pdptSlot.pdptSlot, GET_PDPT_INDEX(vptr));
        cap_frame_print_attrs_pdpt(pdptSlot.pdptSlot);
    } अञ्ञथा {
        pdSlot = lookupPDSlot(vspace, vptr);
        सचे (pdSlot.status == EXCEPTION_NONE &&
            ((pde_ptr_get_page_size(pdSlot.pdSlot) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pdSlot.pdSlot))) {
            printf("frame_%p_%04lu ", pdSlot.pdSlot, GET_PD_INDEX(vptr));
            cap_frame_print_attrs_pd(pdSlot.pdSlot);
        } अञ्ञथा {
            ptSlot = lookupPTSlot(vspace, vptr);
            प्रतिज्ञा(ptSlot.status == EXCEPTION_NONE && pte_ptr_get_present(ptSlot.ptSlot));
            printf("frame_%p_%04lu ", ptSlot.ptSlot, GET_PT_INDEX(vptr));
            cap_frame_print_attrs_pt(ptSlot.ptSlot);
        }
    }
}

स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूलम्);
}

शून्य print_cap_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    /* arch specific caps */
    पक्ष cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        सचे (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.vspace_मूलम्, vptr).pdSlot, GET_PD_INDEX(vptr), (long unsigned पूर्णाङ्क)asid);
        } अञ्ञथा {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.vspace_मूलम्, vptr).pdSlot, GET_PD_INDEX(vptr));
        }
        भङ्ग;
    }
    पक्ष cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_directory_cap_get_capPDMappedAddress(cap);
        सचे (asid) {
            printf("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(find_ret.vspace_मूलम्, vptr).pdptSlot, GET_PDPT_INDEX(vptr), (long unsigned पूर्णाङ्क)asid);
        } अञ्ञथा {
            printf("pd_%p_%04lu\n", lookupPDPTSlot(find_ret.vspace_मूलम्, vptr).pdptSlot, GET_PDPT_INDEX(vptr));
        }
        भङ्ग;
    }
    पक्ष cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_capPDPTMappedAddress(cap);
        सचे (asid) {
            printf("pdpt_%p_%04lu (asid: %lu)\n",
                   lookupPML4Slot(find_ret.vspace_मूलम्, vptr), GET_PML4_INDEX(vptr), (long unsigned पूर्णाङ्क)asid);
        } अञ्ञथा {
            printf("pdpt_%p_%04lu\n", lookupPML4Slot(find_ret.vspace_मूलम्, vptr), GET_PML4_INDEX(vptr));
        }
        भङ्ग;
    }
    पक्ष cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        सचे (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_मूलम्, (long unsigned पूर्णाङ्क)asid);
        } अञ्ञथा {
            printf("%p_pd\n", find_ret.vspace_मूलम्);
        }
        भङ्ग;
    }
    पक्ष cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        भङ्ग;
    }
    पक्ष cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));
        प्रतिज्ञा(find_ret.status == EXCEPTION_NONE);
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूलम्);
        भङ्ग;
    }
    पक्ष cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        भङ्ग;
    }
#ifdef CONFIG_VTX
    पक्ष cap_vcpu_cap: {
        printf("%p_vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        भङ्ग;
    }
#endif
    /* X86 specific caps */
    पक्ष cap_io_port_cap: {
        printf("%p%p_io_port\n", (शून्य *)cap_io_port_cap_get_capIOPortFirstPort(cap),
               (शून्य *)cap_io_port_cap_get_capIOPortLastPort(cap));
        भङ्ग;
    }
#ifdef CONFIG_IOMMU
    पक्ष cap_io_space_cap: {
        printf("%p_io_space\n", (शून्य *)cap_io_space_cap_get_capPCIDevice(cap));
        भङ्ग;
    }
    पक्ष cap_io_page_table_cap: {
        printf("%p_iopt\n", (शून्य *)cap_io_page_table_cap_get_capIOPTBasePtr(cap));
        भङ्ग;
    }
#endif
    स्वभाव: {
        printf("[unknown cap %lu]\n", (long unsigned पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_उच्चः(asid));
}

शून्य print_object_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    /* arch specific objects */
    पक्ष cap_frame_cap:
    पक्ष cap_page_table_cap:
    पक्ष cap_page_directory_cap:
    पक्ष cap_pdpt_cap:
    पक्ष cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        भङ्ग;

    पक्ष cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        भङ्ग;
    }
#ifdef CONFIG_VTX
    पक्ष cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        भङ्ग;
    }
#endif
    /* X86 specific caps */
    पक्ष cap_io_port_cap: {
        printf("%p%p_io_port = io_port ",
               (शून्य *)cap_io_port_cap_get_capIOPortFirstPort(cap),
               (शून्य *)cap_io_port_cap_get_capIOPortLastPort(cap));
        x86_obj_ioports_print_attrs(cap);
        भङ्ग;
    }
#ifdef CONFIG_IOMMU
    पक्ष cap_io_space_cap: {
        printf("%p_io_space = io_space ", (शून्य *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        भङ्ग;
    }
    पक्ष cap_io_page_table_cap: {
        printf("%p_iopt = iopt ", (शून्य *)cap_io_page_table_cap_get_capIOPTBasePtr(cap));
        x86_obj_iopt_print_attrs(cap);
        भङ्ग;
    }
#endif
    स्वभाव: {
        printf("[unknown object %lu]\n", (long unsigned पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

शून्य print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

स्थिर शून्य x86_64_cap_pt_print_slots(pde_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pde_pde_pt_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_PD_INDEX(vptr));

    अत्थाय (word_t i = 0; i < BIT(PT_INDEX_OFFSET + PT_INDEX_BITS); i += (1UL << PT_INDEX_OFFSET)) {
        pte_t *ptSlot = pt + GET_PT_INDEX(i);

        सचे (pte_ptr_get_present(ptSlot)) {
            printf("0x%lx: frame_%p_%04lu ", GET_PT_INDEX(i), ptSlot, GET_PT_INDEX(i));
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

स्थिर शून्य x86_64_cap_pd_print_slots(pdpte_t *pdptSlot, vptr_t vptr)
{
    pde_t *pd = paddr_to_pptr(pdpte_pdpte_pd_ptr_get_pd_base_address(pdptSlot));
    printf("pd_%p_%04lu {\n", pdptSlot, GET_PDPT_INDEX(vptr));

    अत्थाय (word_t i = 0; i < BIT(PD_INDEX_OFFSET + PD_INDEX_BITS); i += (1UL << PD_INDEX_OFFSET)) {
        pde_t *pdSlot = pd + GET_PD_INDEX(i);

        सचे ((pde_ptr_get_page_size(pdSlot) == pde_pde_large) && pde_pde_large_ptr_get_present(pdSlot)) {
            printf("0x%lx: frame_%p_%04lu ", GET_PD_INDEX(i), pdSlot, GET_PD_INDEX(i));
            cap_frame_print_attrs_pd(pdSlot);

        } अञ्ञथा सचे (pde_pde_pt_ptr_get_present(pdSlot)) {
            printf("0x%lx: pt_%p_%04lu\n", GET_PD_INDEX(i), pdSlot, GET_PD_INDEX(i));
        }
    }
    printf("}\n"); /* pd */

    अत्थाय (word_t i = 0; i < BIT(PD_INDEX_OFFSET + PD_INDEX_BITS); i += (1UL << PD_INDEX_OFFSET)) {
        pde_t *pdSlot = pd + GET_PD_INDEX(i);
        सचे ((pde_ptr_get_page_size(pdSlot) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pdSlot)) {
            x86_64_cap_pt_print_slots(pdSlot, i);
        }
    }
}

स्थिर शून्य x86_64_cap_pdpt_print_slots(pml4e_t *pml4Slot, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_to_pptr(pml4e_ptr_get_pdpt_base_address(pml4Slot));

    printf("pdpt_%p_%04lu {\n", pml4Slot, GET_PML4_INDEX(vptr));
    अत्थाय (word_t i = 0; i < BIT(PDPT_INDEX_OFFSET + PDPT_INDEX_BITS); i += (1UL << PDPT_INDEX_OFFSET)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_INDEX(i);

        सचे (pdpte_ptr_get_page_size(pdptSlot) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdptSlot)) {
            printf("0x%lx: frame_%p_%04lu ", GET_PDPT_INDEX(i), pdptSlot, GET_PDPT_INDEX(i));
            cap_frame_print_attrs_pdpt(pdptSlot);

        } अञ्ञथा सचे (pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            printf("0x%lx: pd_%p_%04lu\n", GET_PDPT_INDEX(i), pdptSlot, GET_PDPT_INDEX(i));
        }
    }
    printf("}\n"); /* pdpt */

    अत्थाय (word_t i = 0; i < BIT(PDPT_INDEX_OFFSET + PDPT_INDEX_BITS); i += (1UL << PDPT_INDEX_OFFSET)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_INDEX(i);

        सचे (pdpte_ptr_get_page_size(pdptSlot) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            x86_64_cap_pd_print_slots(pdptSlot, i);
        }
    }
}

स्थिर शून्य x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    printf("%p_pd {\n", pml4);
    अत्थाय (word_t i = 0; i < BIT(PML4_INDEX_OFFSET + PML4_INDEX_BITS); i += (1UL << PML4_INDEX_OFFSET)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        सचे (pml4e_ptr_get_present(pml4Slot)) {
            printf("0x%lx: pdpt_%p_%04lu\n", GET_PML4_INDEX(i), pml4Slot, GET_PML4_INDEX(i));
        }
    }
    printf("}\n"); /* pml4 */

    अत्थाय (word_t i = 0; i < BIT(PML4_INDEX_OFFSET + PML4_INDEX_BITS); i += (1UL << PML4_INDEX_OFFSET)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        सचे (pml4e_ptr_get_present(pml4Slot)) {
            x86_64_cap_pdpt_print_slots(pml4Slot, i);
        }
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    सचे (is_Valid_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#endif /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    printf("arch x86_64\n");
    printf("objects {\n");
#ifdef CONFIG_PRINTING
    print_objects();
#endif
    printf("}\n");

    printf("caps {\n");

    /* reset the seen list */
    reset_seen_list();

#ifdef CONFIG_PRINTING
    print_caps();
    printf("}\n");

    obj_irq_print_maps();
#endif
}

#endif /* CONFIG_DEBUG_BUILD */
