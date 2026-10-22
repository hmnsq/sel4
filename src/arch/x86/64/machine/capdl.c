/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_DEBUG_BUILD

#समावेश <arch/machine/capdl.h>
#समावेश <string.h>
#समावेश <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    वापस tcb->tcbArch.tcbContext.registers[RSP];
}

#अगर_घोषित CONFIG_PRINTING

अटल व्योम obj_frame_print_attrs(paddr_t paddr, word_t page_आकार);
अटल व्योम _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूल_t *vspace);
अटल व्योम cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);
अटल व्योम obj_asidpool_print_attrs(cap_t asid_cap);
अटल व्योम cap_frame_print_attrs_pdpt(pdpte_t *pdptSlot);
अटल व्योम cap_frame_print_attrs_pd(pde_t *pdSlot);
अटल व्योम cap_frame_print_attrs_pt(pte_t *ptSlot);
अटल व्योम cap_frame_print_attrs_impl(word_t super_user, word_t read_write, word_t cache_disabled, word_t xd);

/* use when only have access to pte of frames */
अटल व्योम cap_frame_print_attrs_pdpt(pdpte_t *pdptSlot)
{
    cap_frame_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdptSlot),
                               pdpte_pdpte_1g_ptr_get_read_write(pdptSlot),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdptSlot),
                               pdpte_pdpte_1g_ptr_get_xd(pdptSlot));
}
अटल व्योम cap_frame_print_attrs_pd(pde_t *pdSlot)
{
    cap_frame_print_attrs_impl(pde_pde_large_ptr_get_super_user(pdSlot),
                               pde_pde_large_ptr_get_read_write(pdSlot),
                               pde_pde_large_ptr_get_cache_disabled(pdSlot),
                               pde_pde_large_ptr_get_xd(pdSlot));
}

अटल व्योम cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_ptr_get_super_user(ptSlot),
                               pte_ptr_get_read_write(ptSlot),
                               pte_ptr_get_cache_disabled(ptSlot),
                               pte_ptr_get_xd(ptSlot));
}

अटल व्योम cap_frame_print_attrs_impl(word_t super_user, word_t read_write, word_t cache_disabled, word_t xd)
{
    म_लिखो("(");

    /* rights */
    अगर (read_write) {
        म_लिखो("RW");
    } अन्यथा अगर (super_user) {
        म_लिखो("R");
    }

    अगर (!xd) {
        म_लिखो("X");
    }

    /* asid, mapping */

    अगर (cache_disabled) {
        म_लिखो(", uncached");
    }

    म_लिखो(")\n");
}

अटल व्योम obj_frame_print_attrs(paddr_t paddr, word_t page_आकार)
{
    म_लिखो("(");

    /* VM size */
    चयन (page_आकार) {
    हाल seL4_HugePageBits:
        म_लिखो("1G");
        अवरोध;
    हाल seL4_LargePageBits:
        म_लिखो("2M");
        अवरोध;
    हाल seL4_PageBits:
        म_लिखो("4k");
        अवरोध;
    }

    म_लिखो(", paddr: 0x%p)\n", (व्योम *)paddr);
}

अटल व्योम x86_64_obj_pt_print_slots(pde_t *pdSlot)
{
    paddr_t paddr;
    word_t page_आकार;
    pte_t *pt = paddr_to_pptr(pde_pde_pt_ptr_get_pt_base_address(pdSlot));

    क्रम (word_t i = 0; i < BIT(PT_सूचकांक_OFFSET + PT_सूचकांक_BITS); i += (1UL << PT_सूचकांक_OFFSET)) {
        pte_t *ptSlot = pt + GET_PT_सूचकांक(i);

        अगर (pte_ptr_get_present(ptSlot)) {
            paddr = pte_ptr_get_page_base_address(ptSlot);
            page_आकार = seL4_PageBits;
            म_लिखो("frame_%p_%04lu = frame ", ptSlot, GET_PT_सूचकांक(i));
            obj_frame_print_attrs(paddr, page_आकार);
        }
    }
}

अटल व्योम x86_64_obj_pd_print_slots(pdpte_t *pdptSlot)
{
    paddr_t paddr;
    word_t page_आकार;
    pde_t *pd = paddr_to_pptr(pdpte_pdpte_pd_ptr_get_pd_base_address(pdptSlot));

    क्रम (word_t i = 0; i < BIT(PD_सूचकांक_OFFSET + PD_सूचकांक_BITS); i += (1UL << PD_सूचकांक_OFFSET)) {
        pde_t *pdSlot = pd + GET_PD_सूचकांक(i);

        अगर ((pde_ptr_get_page_आकार(pdSlot) == pde_pde_large) && pde_pde_large_ptr_get_present(pdSlot)) {
            paddr = pde_pde_large_ptr_get_page_base_address(pdSlot);
            page_आकार = seL4_LargePageBits;

            म_लिखो("frame_%p_%04lu = frame ", pdSlot, GET_PD_सूचकांक(i));
            obj_frame_print_attrs(paddr, page_आकार);

        } अन्यथा अगर (pde_pde_pt_ptr_get_present(pdSlot)) {
            म_लिखो("pt_%p_%04lu = pt\n", pdSlot, GET_PD_सूचकांक(i));
            x86_64_obj_pt_print_slots(pdSlot);
        }
    }
}

अटल व्योम x86_64_obj_pdpt_print_slots(pml4e_t *pml4Slot)
{
    paddr_t paddr;
    word_t page_आकार;
    pdpte_t *pdpt = paddr_to_pptr(pml4e_ptr_get_pdpt_base_address(pml4Slot));

    क्रम (word_t i = 0; i < BIT(PDPT_सूचकांक_OFFSET + PDPT_सूचकांक_BITS); i += (1UL << PDPT_सूचकांक_OFFSET)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_सूचकांक(i);

        अगर (pdpte_ptr_get_page_आकार(pdptSlot) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdptSlot)) {
            paddr = pdpte_pdpte_1g_ptr_get_page_base_address(pdptSlot);
            page_आकार = seL4_HugePageBits;

            म_लिखो("frame_%p_%04lu = frame ", pdptSlot, GET_PDPT_सूचकांक(i));
            obj_frame_print_attrs(paddr, page_आकार);

        } अन्यथा अगर (pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            म_लिखो("pd_%p_%04lu = pd\n", pdptSlot, GET_PDPT_सूचकांक(i));
            x86_64_obj_pd_print_slots(pdptSlot);
        }
    }
}

अटल व्योम x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    क्रम (word_t i = 0; i < BIT(PML4_सूचकांक_OFFSET + PML4_सूचकांक_BITS); i += (1UL << PML4_सूचकांक_OFFSET)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        अगर (pml4e_ptr_get_present(pml4Slot)) {
            म_लिखो("pdpt_%p_%04lu = pdpt\n", pml4Slot, GET_PML4_सूचकांक(i));
            x86_64_obj_pdpt_print_slots(pml4Slot);
        }
    }
}

व्योम obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    अगर (is_Valid_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        म_लिखो("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
अटल व्योम _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूल_t *vspace)
{
    lookupPDPTSlot_ret_t pdptSlot = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t ptSlot;
    lookupPDSlot_ret_t pdSlot;

    अगर (pdptSlot.status == EXCEPTION_NONE &&
        pdpte_ptr_get_page_आकार(pdptSlot.pdptSlot) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdptSlot.pdptSlot)) {
        म_लिखो("frame_%p_%04lu ", pdptSlot.pdptSlot, GET_PDPT_सूचकांक(vptr));
        cap_frame_print_attrs_pdpt(pdptSlot.pdptSlot);
    } अन्यथा {
        pdSlot = lookupPDSlot(vspace, vptr);
        अगर (pdSlot.status == EXCEPTION_NONE &&
            ((pde_ptr_get_page_आकार(pdSlot.pdSlot) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pdSlot.pdSlot))) {
            म_लिखो("frame_%p_%04lu ", pdSlot.pdSlot, GET_PD_सूचकांक(vptr));
            cap_frame_print_attrs_pd(pdSlot.pdSlot);
        } अन्यथा {
            ptSlot = lookupPTSlot(vspace, vptr);
            निश्चित(ptSlot.status == EXCEPTION_NONE && pte_ptr_get_present(ptSlot.ptSlot));
            म_लिखो("frame_%p_%04lu ", ptSlot.ptSlot, GET_PT_सूचकांक(vptr));
            cap_frame_print_attrs_pt(ptSlot.ptSlot);
        }
    }
}

अटल व्योम cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूल);
}

व्योम print_cap_arch(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    /* arch specific caps */
    हाल cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        अगर (asid) {
            म_लिखो("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.vspace_मूल, vptr).pdSlot, GET_PD_सूचकांक(vptr), (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("pt_%p_%04lu\n", lookupPDSlot(find_ret.vspace_मूल, vptr).pdSlot, GET_PD_सूचकांक(vptr));
        }
        अवरोध;
    }
    हाल cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_directory_cap_get_capPDMappedAddress(cap);
        अगर (asid) {
            म_लिखो("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(find_ret.vspace_मूल, vptr).pdptSlot, GET_PDPT_सूचकांक(vptr), (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("pd_%p_%04lu\n", lookupPDPTSlot(find_ret.vspace_मूल, vptr).pdptSlot, GET_PDPT_सूचकांक(vptr));
        }
        अवरोध;
    }
    हाल cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_capPDPTMappedAddress(cap);
        अगर (asid) {
            म_लिखो("pdpt_%p_%04lu (asid: %lu)\n",
                   lookupPML4Slot(find_ret.vspace_मूल, vptr), GET_PML4_सूचकांक(vptr), (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("pdpt_%p_%04lu\n", lookupPML4Slot(find_ret.vspace_मूल, vptr), GET_PML4_सूचकांक(vptr));
        }
        अवरोध;
    }
    हाल cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        अगर (asid) {
            म_लिखो("%p_pd (asid: %lu)\n",
                   find_ret.vspace_मूल, (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("%p_pd\n", find_ret.vspace_मूल);
        }
        अवरोध;
    }
    हाल cap_asid_control_cap: {
        /* only one in the system */
        म_लिखो("asid_control\n");
        अवरोध;
    }
    हाल cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));
        निश्चित(find_ret.status == EXCEPTION_NONE);
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूल);
        अवरोध;
    }
    हाल cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool\n", (व्योम *)cap_asid_pool_cap_get_capASIDPool(cap));
        अवरोध;
    }
#अगर_घोषित CONFIG_VTX
    हाल cap_vcpu_cap: {
        म_लिखो("%p_vcpu\n", (व्योम *)cap_vcpu_cap_get_capVCPUPtr(cap));
        अवरोध;
    }
#पूर्ण_अगर
    /* X86 specific caps */
    हाल cap_io_port_cap: {
        म_लिखो("%p%p_io_port\n", (व्योम *)cap_io_port_cap_get_capIOPortFirstPort(cap),
               (व्योम *)cap_io_port_cap_get_capIOPortLastPort(cap));
        अवरोध;
    }
#अगर_घोषित CONFIG_IOMMU
    हाल cap_io_space_cap: {
        म_लिखो("%p_io_space\n", (व्योम *)cap_io_space_cap_get_capPCIDevice(cap));
        अवरोध;
    }
    हाल cap_io_page_table_cap: {
        म_लिखो("%p_iopt\n", (व्योम *)cap_io_page_table_cap_get_capIOPTBasePtr(cap));
        अवरोध;
    }
#पूर्ण_अगर
    शेष: {
        म_लिखो("[unknown cap %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        अवरोध;
    }
    }
}

अटल व्योम obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    म_लिखो("(asid_high: 0x%lx)\n", ASID_ऊपरी(asid));
}

व्योम print_object_arch(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    /* arch specific objects */
    हाल cap_frame_cap:
    हाल cap_page_table_cap:
    हाल cap_page_directory_cap:
    हाल cap_pdpt_cap:
    हाल cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        अवरोध;

    हाल cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool = asid_pool ",
               (व्योम *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        अवरोध;
    }
#अगर_घोषित CONFIG_VTX
    हाल cap_vcpu_cap: {
        म_लिखो("%p_vcpu = vcpu\n", (व्योम *)cap_vcpu_cap_get_capVCPUPtr(cap));
        अवरोध;
    }
#पूर्ण_अगर
    /* X86 specific caps */
    हाल cap_io_port_cap: {
        म_लिखो("%p%p_io_port = io_port ",
               (व्योम *)cap_io_port_cap_get_capIOPortFirstPort(cap),
               (व्योम *)cap_io_port_cap_get_capIOPortLastPort(cap));
        x86_obj_ioports_print_attrs(cap);
        अवरोध;
    }
#अगर_घोषित CONFIG_IOMMU
    हाल cap_io_space_cap: {
        म_लिखो("%p_io_space = io_space ", (व्योम *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        अवरोध;
    }
    हाल cap_io_page_table_cap: {
        म_लिखो("%p_iopt = iopt ", (व्योम *)cap_io_page_table_cap_get_capIOPTBasePtr(cap));
        x86_obj_iopt_print_attrs(cap);
        अवरोध;
    }
#पूर्ण_अगर
    शेष: {
        म_लिखो("[unknown object %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        अवरोध;
    }
    }
}

व्योम print_ipc_बफर_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    म_लिखो("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

अटल व्योम x86_64_cap_pt_print_slots(pde_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pde_pde_pt_ptr_get_pt_base_address(pdSlot));
    म_लिखो("pt_%p_%04lu {\n", pdSlot, GET_PD_सूचकांक(vptr));

    क्रम (word_t i = 0; i < BIT(PT_सूचकांक_OFFSET + PT_सूचकांक_BITS); i += (1UL << PT_सूचकांक_OFFSET)) {
        pte_t *ptSlot = pt + GET_PT_सूचकांक(i);

        अगर (pte_ptr_get_present(ptSlot)) {
            म_लिखो("0x%lx: frame_%p_%04lu ", GET_PT_सूचकांक(i), ptSlot, GET_PT_सूचकांक(i));
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    म_लिखो("}\n"); /* pt */
}

अटल व्योम x86_64_cap_pd_print_slots(pdpte_t *pdptSlot, vptr_t vptr)
{
    pde_t *pd = paddr_to_pptr(pdpte_pdpte_pd_ptr_get_pd_base_address(pdptSlot));
    म_लिखो("pd_%p_%04lu {\n", pdptSlot, GET_PDPT_सूचकांक(vptr));

    क्रम (word_t i = 0; i < BIT(PD_सूचकांक_OFFSET + PD_सूचकांक_BITS); i += (1UL << PD_सूचकांक_OFFSET)) {
        pde_t *pdSlot = pd + GET_PD_सूचकांक(i);

        अगर ((pde_ptr_get_page_आकार(pdSlot) == pde_pde_large) && pde_pde_large_ptr_get_present(pdSlot)) {
            म_लिखो("0x%lx: frame_%p_%04lu ", GET_PD_सूचकांक(i), pdSlot, GET_PD_सूचकांक(i));
            cap_frame_print_attrs_pd(pdSlot);

        } अन्यथा अगर (pde_pde_pt_ptr_get_present(pdSlot)) {
            म_लिखो("0x%lx: pt_%p_%04lu\n", GET_PD_सूचकांक(i), pdSlot, GET_PD_सूचकांक(i));
        }
    }
    म_लिखो("}\n"); /* pd */

    क्रम (word_t i = 0; i < BIT(PD_सूचकांक_OFFSET + PD_सूचकांक_BITS); i += (1UL << PD_सूचकांक_OFFSET)) {
        pde_t *pdSlot = pd + GET_PD_सूचकांक(i);
        अगर ((pde_ptr_get_page_आकार(pdSlot) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pdSlot)) {
            x86_64_cap_pt_print_slots(pdSlot, i);
        }
    }
}

अटल व्योम x86_64_cap_pdpt_print_slots(pml4e_t *pml4Slot, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_to_pptr(pml4e_ptr_get_pdpt_base_address(pml4Slot));

    म_लिखो("pdpt_%p_%04lu {\n", pml4Slot, GET_PML4_सूचकांक(vptr));
    क्रम (word_t i = 0; i < BIT(PDPT_सूचकांक_OFFSET + PDPT_सूचकांक_BITS); i += (1UL << PDPT_सूचकांक_OFFSET)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_सूचकांक(i);

        अगर (pdpte_ptr_get_page_आकार(pdptSlot) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdptSlot)) {
            म_लिखो("0x%lx: frame_%p_%04lu ", GET_PDPT_सूचकांक(i), pdptSlot, GET_PDPT_सूचकांक(i));
            cap_frame_print_attrs_pdpt(pdptSlot);

        } अन्यथा अगर (pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            म_लिखो("0x%lx: pd_%p_%04lu\n", GET_PDPT_सूचकांक(i), pdptSlot, GET_PDPT_सूचकांक(i));
        }
    }
    म_लिखो("}\n"); /* pdpt */

    क्रम (word_t i = 0; i < BIT(PDPT_सूचकांक_OFFSET + PDPT_सूचकांक_BITS); i += (1UL << PDPT_सूचकांक_OFFSET)) {
        pdpte_t *pdptSlot = pdpt + GET_PDPT_सूचकांक(i);

        अगर (pdpte_ptr_get_page_आकार(pdptSlot) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdptSlot)) {
            x86_64_cap_pd_print_slots(pdptSlot, i);
        }
    }
}

अटल व्योम x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    म_लिखो("%p_pd {\n", pml4);
    क्रम (word_t i = 0; i < BIT(PML4_सूचकांक_OFFSET + PML4_सूचकांक_BITS); i += (1UL << PML4_सूचकांक_OFFSET)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        अगर (pml4e_ptr_get_present(pml4Slot)) {
            म_लिखो("0x%lx: pdpt_%p_%04lu\n", GET_PML4_सूचकांक(i), pml4Slot, GET_PML4_सूचकांक(i));
        }
    }
    म_लिखो("}\n"); /* pml4 */

    क्रम (word_t i = 0; i < BIT(PML4_सूचकांक_OFFSET + PML4_सूचकांक_BITS); i += (1UL << PML4_सूचकांक_OFFSET)) {
        pml4e_t *pml4Slot = lookupPML4Slot(pml4, i);
        अगर (pml4e_ptr_get_present(pml4Slot)) {
            x86_64_cap_pdpt_print_slots(pml4Slot, i);
        }
    }
}

व्योम obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    अगर (is_Valid_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#पूर्ण_अगर /* CONFIG_PRINTING */

व्योम debug_capDL(व्योम)
{
    म_लिखो("arch x86_64\n");
    म_लिखो("objects {\n");
#अगर_घोषित CONFIG_PRINTING
    print_objects();
#पूर्ण_अगर
    म_लिखो("}\n");

    म_लिखो("caps {\n");

    /* reset the seen list */
    reset_seen_list();

#अगर_घोषित CONFIG_PRINTING
    print_caps();
    म_लिखो("}\n");

    obj_irq_print_maps();
#पूर्ण_अगर
}

#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
