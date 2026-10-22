/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD

#अन्तर्भाव <arch/machine/capdl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    प्रत्यागम tcb->tcbArch.tcbContext.registers[RSP];
}

#यदि_परिभाषितम् CONFIG_PRINTING

स्थिर शून्य obj_frame_print_attrs(paddr_t paddr, word_t page_आकार);
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
    प्रारूप_लिख("(");

    /* rights */
    यदि (read_write) {
        प्रारूप_लिख("RW");
    } अन्यथा यदि (super_user) {
        प्रारूप_लिख("R");
    }

    यदि (!xd) {
        प्रारूप_लिख("X");
    }

    /* asid, mapping */

    यदि (cache_disabled) {
        प्रारूप_लिख(", uncached");
    }

    प्रारूप_लिख(")\n");
}

स्थिर शून्य obj_frame_print_attrs(paddr_t paddr, word_t page_आकार)
{
    प्रारूप_लिख("(");

    /* VM size */
    विकल्प (page_आकार) {
    पक्ष se_L4_Huge_Page_द्वयङ्काः:
        प्रारूप_लिख("1G");
        भङ्ग;
    पक्ष se_L4_Large_Page_द्वयङ्काः:
        प्रारूप_लिख("2M");
        भङ्ग;
    पक्ष se_L4_Page_द्वयङ्काः:
        प्रारूप_लिख("4k");
        भङ्ग;
    }

    प्रारूप_लिख(", paddr: 0x%p)\n", (शून्य *)paddr);
}

स्थिर शून्य x86_64_obj_pt_print_slots(pde_t *pdSlot)
{
    paddr_t paddr;
    word_t page_आकार;
    pte_t *pt = paddr_गन्तव्यम्_pptr(pde_pde_pt_ptr_get_pt_आधारः_स्थानाङ्कः(pdSlot));

    प्रति (word_t i = 0; i < BIT(PT_सूचकांक_विस्थापनम् + PT_सूचकांक_द्वयङ्काः); i += (1UL << PT_सूचकांक_विस्थापनम्)) {
        pte_t *ptSlot = pt + GET_PT_सूचकांक(i);

        यदि (pte_ptr_get_present(ptSlot)) {
            paddr = pte_ptr_get_page_आधारः_स्थानाङ्कः(ptSlot);
            page_आकार = se_L4_Page_द्वयङ्काः;
            प्रारूप_लिख("frame_%p_%04lu = frame ", ptSlot, GET_PT_सूचकांक(i));
            obj_frame_print_attrs(paddr, page_आकार);
        }
    }
}

स्थिर शून्य x86_64_obj_pd_print_slots(pdpte_t *pdptSlot)
{
    paddr_t paddr;
    word_t page_आकार;
    pde_t *pd = paddr_गन्तव्यम्_pptr(pdpte_pdpte_pd_ptr_get_pd_आधारः_स्थानाङ्कः(pdptSlot));

    प्रति (word_t i = 0; i < BIT(PD_सूचकांक_विस्थापनम् + PD_सूचकांक_द्वयङ्काः); i += (1UL << PD_सूचकांक_विस्थापनम्)) {
        pde_t *pdSlot = pd + GET_PD_सूचकांक(i);

        यदि ((pde_ptr_get_page_आकार(pdSlot) == pde_pde_large) && pde_pde_large_ptr_get_present(pdSlot)) {
            paddr = pde_pde_large_ptr_get_page_आधारः_स्थानाङ्कः(pdSlot);
            page_आकार = se_L4_Large_Page_द्वयङ्काः;

            प्रारूप_लिख("frame_%p_%04lu = frame ", pdSlot, GET_PD_सूचकांक(i));
            obj_frame_print_attrs(paddr, page_आकार);

        } अन्यथा यदि (pde_pde_pt_ptr_get_present(pdSlot)) {
            प्रारूप_लिख("pt_%p_%04lu = pt\n", pdSlot, GET_PD_सूचकांक(i));
            x86_64_obj_pt_print_slots(pdSlot);
        }
    }
}

स्थिर शून्य x86_64_obj_pdpt_print_slots(pml4e_t *pml4Slot)
{
    paddr_t paddr;
    word_t page_आकार;
    pdpte_t *pdpt = paddr_गन्तव्यम्_pptr(pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4Slot));

    प्रति (word_t i = 0; i < BIT(PDPT_सूचकांक_विस्थापनम् + PDPT_सूचकांक_द्वयङ्काः); i += (1UL << PDPT_सूचकांक_विस्थापनम्)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_सूचकांक(i);

        यदि (pdpte_ptr_get_page_आकार(pdptSlot) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdptSlot)) {
            paddr = pdpte_pdpte_1g_ptr_get_page_आधारः_स्थानाङ्कः(pdptSlot);
            page_आकार = se_L4_Huge_Page_द्वयङ्काः;

            प्रारूप_लिख("frame_%p_%04lu = frame ", pdptSlot, GET_PDPT_सूचकांक(i));
            obj_frame_print_attrs(paddr, page_आकार);

        } अन्यथा यदि (pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            प्रारूप_लिख("pd_%p_%04lu = pd\n", pdptSlot, GET_PDPT_सूचकांक(i));
            x86_64_obj_pd_print_slots(pdptSlot);
        }
    }
}

स्थिर शून्य x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    प्रति (word_t i = 0; i < BIT(PML4_सूचकांक_विस्थापनम् + PML4_सूचकांक_द्वयङ्काः); i += (1UL << PML4_सूचकांक_विस्थापनम्)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        यदि (pml4e_ptr_get_present(pml4Slot)) {
            प्रारूप_लिख("pdpt_%p_%04lu = pdpt\n", pml4Slot, GET_PML4_सूचकांक(i));
            x86_64_obj_pdpt_print_slots(pml4Slot);
        }
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    यदि (is_Valid_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_आधारः_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        सङ्कलय_गन्तव्यम्_आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        प्रारूप_लिख("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
स्थिर शून्य _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूलम्_t *vspace)
{
    lookupPDPTSlot_ret_t pdptSlot = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t ptSlot;
    lookupPDSlot_ret_t pdSlot;

    यदि (pdptSlot.स्थितिः == EXCEPTION_NONE &&
        pdpte_ptr_get_page_आकार(pdptSlot.pdptSlot) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdptSlot.pdptSlot)) {
        प्रारूप_लिख("frame_%p_%04lu ", pdptSlot.pdptSlot, GET_PDPT_सूचकांक(vptr));
        cap_frame_print_attrs_pdpt(pdptSlot.pdptSlot);
    } अन्यथा {
        pdSlot = lookupPDSlot(vspace, vptr);
        यदि (pdSlot.स्थितिः == EXCEPTION_NONE &&
            ((pde_ptr_get_page_आकार(pdSlot.pdSlot) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pdSlot.pdSlot))) {
            प्रारूप_लिख("frame_%p_%04lu ", pdSlot.pdSlot, GET_PD_सूचकांक(vptr));
            cap_frame_print_attrs_pd(pdSlot.pdSlot);
        } अन्यथा {
            ptSlot = lookupPTSlot(vspace, vptr);
            प्रतिज्ञा(ptSlot.स्थितिः == EXCEPTION_NONE && pte_ptr_get_present(ptSlot.ptSlot));
            प्रारूप_लिख("frame_%p_%04lu ", ptSlot.ptSlot, GET_PT_सूचकांक(vptr));
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
    पक्ष cap_page_सारणी_cap: {
        asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap);
        यदि (asid) {
            प्रारूप_लिख("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.vspace_मूलम्, vptr).pdSlot, GET_PD_सूचकांक(vptr), (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("pt_%p_%04lu\n", lookupPDSlot(find_ret.vspace_मूलम्, vptr).pdSlot, GET_PD_सूचकांक(vptr));
        }
        भङ्ग;
    }
    पक्ष cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_directory_cap_get_cap_PDMapped_स्थानाङ्कः(cap);
        यदि (asid) {
            प्रारूप_लिख("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(find_ret.vspace_मूलम्, vptr).pdptSlot, GET_PDPT_सूचकांक(vptr), (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("pd_%p_%04lu\n", lookupPDPTSlot(find_ret.vspace_मूलम्, vptr).pdptSlot, GET_PDPT_सूचकांक(vptr));
        }
        भङ्ग;
    }
    पक्ष cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_स्थानाङ्कः(cap);
        यदि (asid) {
            प्रारूप_लिख("pdpt_%p_%04lu (asid: %lu)\n",
                   lookupPML4Slot(find_ret.vspace_मूलम्, vptr), GET_PML4_सूचकांक(vptr), (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("pdpt_%p_%04lu\n", lookupPML4Slot(find_ret.vspace_मूलम्, vptr), GET_PML4_सूचकांक(vptr));
        }
        भङ्ग;
    }
    पक्ष cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        यदि (asid) {
            प्रारूप_लिख("%p_pd (asid: %lu)\n",
                   find_ret.vspace_मूलम्, (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("%p_pd\n", find_ret.vspace_मूलम्);
        }
        भङ्ग;
    }
    पक्ष cap_asid_control_cap: {
        /* only one in the system */
        प्रारूप_लिख("asid_control\n");
        भङ्ग;
    }
    पक्ष cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));
        प्रतिज्ञा(find_ret.स्थितिः == EXCEPTION_NONE);
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूलम्);
        भङ्ग;
    }
    पक्ष cap_asid_pool_cap: {
        प्रारूप_लिख("%p_asid_pool\n", (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        भङ्ग;
    }
#यदि_परिभाषितम् CONFIG_VTX
    पक्ष cap_vcpu_cap: {
        प्रारूप_लिख("%p_vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        भङ्ग;
    }
#यदि_अन्तः
    /* X86 specific caps */
    पक्ष cap_io_पत्तनम्_cap: {
        प्रारूप_लिख("%p%p_io_port\n", (शून्य *)cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(cap),
               (शून्य *)cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(cap));
        भङ्ग;
    }
#यदि_परिभाषितम् CONFIG_IOMMU
    पक्ष cap_io_space_cap: {
        प्रारूप_लिख("%p_io_space\n", (शून्य *)cap_io_space_cap_get_capPCIDevice(cap));
        भङ्ग;
    }
    पक्ष cap_io_page_सारणी_cap: {
        प्रारूप_लिख("%p_iopt\n", (शून्य *)cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(cap));
        भङ्ग;
    }
#यदि_अन्तः
    स्वभाव: {
        प्रारूप_लिख("[unknown cap %lu]\n", (दीर्घ अचिह्नित पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    प्रारूप_लिख("(asid_high: 0x%lx)\n", ASID_उच्चः(asid));
}

शून्य print_object_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    /* arch specific objects */
    पक्ष cap_frame_cap:
    पक्ष cap_page_सारणी_cap:
    पक्ष cap_page_directory_cap:
    पक्ष cap_pdpt_cap:
    पक्ष cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        भङ्ग;

    पक्ष cap_asid_pool_cap: {
        प्रारूप_लिख("%p_asid_pool = asid_pool ",
               (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        भङ्ग;
    }
#यदि_परिभाषितम् CONFIG_VTX
    पक्ष cap_vcpu_cap: {
        प्रारूप_लिख("%p_vcpu = vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        भङ्ग;
    }
#यदि_अन्तः
    /* X86 specific caps */
    पक्ष cap_io_पत्तनम्_cap: {
        प्रारूप_लिख("%p%p_io_port = io_port ",
               (शून्य *)cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(cap),
               (शून्य *)cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(cap));
        x86_obj_ioports_print_attrs(cap);
        भङ्ग;
    }
#यदि_परिभाषितम् CONFIG_IOMMU
    पक्ष cap_io_space_cap: {
        प्रारूप_लिख("%p_io_space = io_space ", (शून्य *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        भङ्ग;
    }
    पक्ष cap_io_page_सारणी_cap: {
        प्रारूप_लिख("%p_iopt = iopt ", (शून्य *)cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        भङ्ग;
    }
#यदि_अन्तः
    स्वभाव: {
        प्रारूप_लिख("[unknown object %lu]\n", (दीर्घ अचिह्नित पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

शून्य print_ipc_बफर_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    प्रारूप_लिख("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

स्थिर शून्य x86_64_cap_pt_print_slots(pde_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_गन्तव्यम्_pptr(pde_pde_pt_ptr_get_pt_आधारः_स्थानाङ्कः(pdSlot));
    प्रारूप_लिख("pt_%p_%04lu {\n", pdSlot, GET_PD_सूचकांक(vptr));

    प्रति (word_t i = 0; i < BIT(PT_सूचकांक_विस्थापनम् + PT_सूचकांक_द्वयङ्काः); i += (1UL << PT_सूचकांक_विस्थापनम्)) {
        pte_t *ptSlot = pt + GET_PT_सूचकांक(i);

        यदि (pte_ptr_get_present(ptSlot)) {
            प्रारूप_लिख("0x%lx: frame_%p_%04lu ", GET_PT_सूचकांक(i), ptSlot, GET_PT_सूचकांक(i));
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    प्रारूप_लिख("}\n"); /* pt */
}

स्थिर शून्य x86_64_cap_pd_print_slots(pdpte_t *pdptSlot, vptr_t vptr)
{
    pde_t *pd = paddr_गन्तव्यम्_pptr(pdpte_pdpte_pd_ptr_get_pd_आधारः_स्थानाङ्कः(pdptSlot));
    प्रारूप_लिख("pd_%p_%04lu {\n", pdptSlot, GET_PDPT_सूचकांक(vptr));

    प्रति (word_t i = 0; i < BIT(PD_सूचकांक_विस्थापनम् + PD_सूचकांक_द्वयङ्काः); i += (1UL << PD_सूचकांक_विस्थापनम्)) {
        pde_t *pdSlot = pd + GET_PD_सूचकांक(i);

        यदि ((pde_ptr_get_page_आकार(pdSlot) == pde_pde_large) && pde_pde_large_ptr_get_present(pdSlot)) {
            प्रारूप_लिख("0x%lx: frame_%p_%04lu ", GET_PD_सूचकांक(i), pdSlot, GET_PD_सूचकांक(i));
            cap_frame_print_attrs_pd(pdSlot);

        } अन्यथा यदि (pde_pde_pt_ptr_get_present(pdSlot)) {
            प्रारूप_लिख("0x%lx: pt_%p_%04lu\n", GET_PD_सूचकांक(i), pdSlot, GET_PD_सूचकांक(i));
        }
    }
    प्रारूप_लिख("}\n"); /* pd */

    प्रति (word_t i = 0; i < BIT(PD_सूचकांक_विस्थापनम् + PD_सूचकांक_द्वयङ्काः); i += (1UL << PD_सूचकांक_विस्थापनम्)) {
        pde_t *pdSlot = pd + GET_PD_सूचकांक(i);
        यदि ((pde_ptr_get_page_आकार(pdSlot) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pdSlot)) {
            x86_64_cap_pt_print_slots(pdSlot, i);
        }
    }
}

स्थिर शून्य x86_64_cap_pdpt_print_slots(pml4e_t *pml4Slot, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_गन्तव्यम्_pptr(pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4Slot));

    प्रारूप_लिख("pdpt_%p_%04lu {\n", pml4Slot, GET_PML4_सूचकांक(vptr));
    प्रति (word_t i = 0; i < BIT(PDPT_सूचकांक_विस्थापनम् + PDPT_सूचकांक_द्वयङ्काः); i += (1UL << PDPT_सूचकांक_विस्थापनम्)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_सूचकांक(i);

        यदि (pdpte_ptr_get_page_आकार(pdptSlot) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdptSlot)) {
            प्रारूप_लिख("0x%lx: frame_%p_%04lu ", GET_PDPT_सूचकांक(i), pdptSlot, GET_PDPT_सूचकांक(i));
            cap_frame_print_attrs_pdpt(pdptSlot);

        } अन्यथा यदि (pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            प्रारूप_लिख("0x%lx: pd_%p_%04lu\n", GET_PDPT_सूचकांक(i), pdptSlot, GET_PDPT_सूचकांक(i));
        }
    }
    प्रारूप_लिख("}\n"); /* pdpt */

    प्रति (word_t i = 0; i < BIT(PDPT_सूचकांक_विस्थापनम् + PDPT_सूचकांक_द्वयङ्काः); i += (1UL << PDPT_सूचकांक_विस्थापनम्)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_सूचकांक(i);

        यदि (pdpte_ptr_get_page_आकार(pdptSlot) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            x86_64_cap_pd_print_slots(pdptSlot, i);
        }
    }
}

स्थिर शून्य x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    प्रारूप_लिख("%p_pd {\n", pml4);
    प्रति (word_t i = 0; i < BIT(PML4_सूचकांक_विस्थापनम् + PML4_सूचकांक_द्वयङ्काः); i += (1UL << PML4_सूचकांक_विस्थापनम्)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        यदि (pml4e_ptr_get_present(pml4Slot)) {
            प्रारूप_लिख("0x%lx: pdpt_%p_%04lu\n", GET_PML4_सूचकांक(i), pml4Slot, GET_PML4_सूचकांक(i));
        }
    }
    प्रारूप_लिख("}\n"); /* pml4 */

    प्रति (word_t i = 0; i < BIT(PML4_सूचकांक_विस्थापनम् + PML4_सूचकांक_द्वयङ्काः); i += (1UL << PML4_सूचकांक_विस्थापनम्)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        यदि (pml4e_ptr_get_present(pml4Slot)) {
            x86_64_cap_pdpt_print_slots(pml4Slot, i);
        }
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    यदि (is_Valid_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_आधारः_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        सङ्कलय_गन्तव्यम्_आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#यदि_अन्तः /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    प्रारूप_लिख("arch x86_64\n");
    प्रारूप_लिख("objects {\n");
#यदि_परिभाषितम् CONFIG_PRINTING
    print_objects();
#यदि_अन्तः
    प्रारूप_लिख("}\n");

    प्रारूप_लिख("caps {\n");

    /* reset the seen list */
    reset_आलोकितम्_list();

#यदि_परिभाषितम् CONFIG_PRINTING
    print_caps();
    प्रारूप_लिख("}\n");

    obj_irq_print_maps();
#यदि_अन्तः
}

#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
