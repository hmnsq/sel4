/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_DEBUG_BUILD

#अन्तर्भाव <machine/capdl.h>
#अन्तर्भाव <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    परत tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap);
स्थिर शून्य obj_frame_print_attrs(paddr_t paddr);
स्थिर शून्य riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, पूर्णांक level);
स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot);

स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    म_लिखो("(asid_high: 0x%lx)\n", ASID_वरचा(asid));
}

शून्य print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    म_लिखो("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूळ);
}

स्थिर शून्य riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t ptIndex, पूर्णांक level)
{
    pte_t *pt;
    जर (level == CONFIG_PT_LEVELS) {
        म_लिखो("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } अन्यथा {
        म_लिखो("pt_%p_%04lu {\n", upperPtSlot, ptIndex);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_डावा = PT_INDEX_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    साठी (word_t i = 0; i < BIT(pt_Bits_डावा + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_डावा))) {
        word_t ptSlotIndex = ((i >> pt_Bits_डावा) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        जर (pte_ptr_get_valid(ptSlot)) {
            जर (level) { /* pt */
                म_लिखो("0x%lx: pt_%p_%04lu\n", ptSlotIndex, ptSlot, ptSlotIndex);
            } अन्यथा { /* frame */
                म_लिखो("0x%lx: frame_%p_%04lu", ptSlotIndex, ptSlot, ptSlotIndex);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    म_लिखो("}\n"); /* lvl1pt/pt */

    साठी (word_t i = 0; i < BIT(pt_Bits_डावा + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_डावा))) {
        word_t ptSlotIndex = ((i >> pt_Bits_डावा) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        जर (pte_ptr_get_valid(ptSlot)) {
            जर (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, ptSlotIndex, level);
            }
        }
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    जर (is_Valid_VTable_मूळ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    म_लिखो("(");

    /* rights */
    जर (pte_ptr_get_read(ptSlot)) {
        म_लिखो("R");
    }

    जर (pte_ptr_get_write(ptSlot)) {
        म_लिखो("W");
    }

    जर (pte_ptr_get_execute(ptSlot)) {
        म_लिखो("X");
    }

    /* cacheable not supported yet */

    म_लिखो(")\n");
}

स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    प्रतिज्ञा(lu_ret.pt_Bits_डावा == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_डावा) & MASK(PT_INDEX_BITS));

    म_लिखो("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

शून्य print_cap_arch(cap_t cap)
{
    निवड (cap_get_capType(cap)) {
    स्थिती cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_डावा = PT_INDEX_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_डावा) & MASK(PT_INDEX_BITS));
        जर (asid) {
            म_लिखो("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_मूळ, vptr).ptSlot, slot, (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_मूळ, vptr).ptSlot, slot);
        }
        थांबा;
    }
    स्थिती cap_asid_control_cap: {
        /* only one in the system */
        म_लिखो("asid_control\n");
        थांबा;
    }
    स्थिती cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));

        प्रतिज्ञा(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूळ);
        थांबा;
    }
    स्थिती cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool\n", (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        थांबा;
    }
    /* riscv specific caps */
    /* nothing */
    पूर्वनिर्धारित: {
        म_लिखो("[unknown cap %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        थांबा;
    }
    }
}

स्थिर शून्य obj_frame_print_attrs(paddr_t paddr)
{
    म_लिखो("(4k, paddr: 0x%p)\n", (शून्य *)paddr);
}

शून्य print_object_arch(cap_t cap)
{
    निवड (cap_get_capType(cap)) {
    स्थिती cap_frame_cap:
    स्थिती cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        थांबा;

    स्थिती cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool = asid_pool ",
               (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        थांबा;
    }
    /* riscv specific caps */
    /* nothing */
    पूर्वनिर्धारित: {
        म_लिखो("[unknown object %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        थांबा;
    }
    }
}

स्थिर शून्य riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, पूर्णांक level)
{
    word_t pt_Bits_डावा = PT_INDEX_BITS * level + seL4_PageBits;

    साठी (word_t i = 0; i < BIT(pt_Bits_डावा + PT_INDEX_BITS); i += (1 << (pt_Bits_डावा))) {
        word_t ptIndex = ((i >> pt_Bits_डावा) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptIndex;
        जर (pte_ptr_get_valid(ptSlot)) {
            जर (level) { /* pt */
                म_लिखो("pt_%p_%04lu = pt\n", ptSlot, ptIndex);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } अन्यथा { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                म_लिखो("frame_%p_%04lu = frame ", ptSlot, ptIndex);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    जर (is_Valid_VTable_मूळ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        म_लिखो("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    म_लिखो("arch riscv\n");
    म_लिखो("objects {\n");
#ifdef CONFIG_PRINTING
    print_objects();
#endif
    म_लिखो("}\n");

    म_लिखो("caps {\n");

    /* reset the seen list */
    reset_seen_list();

#ifdef CONFIG_PRINTING
    print_caps();
    म_लिखो("}\n");

    obj_irq_print_maps();
#endif /* CONFIG_PRINTING */
}

#endif /* CONFIG_DEBUG_BUILD */
