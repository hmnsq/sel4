/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD

#अन्तर्भाव <machine/capdl.h>
#अन्तर्भाव <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    प्रत्यागम tcb->tcbArch.tcbContext.registers[SP];
}

#यदि_परिभाषितम् CONFIG_PRINTING

स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap);
स्थिर शून्य obj_frame_print_attrs(paddr_t paddr);
स्थिर शून्य riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, पूर्णाङ्क level);
स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot);

स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    प्रारूप_लिख("(asid_high: 0x%lx)\n", ASID_उच्चः(asid));
}

शून्य print_ipc_बफर_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    प्रारूप_लिख("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूलम्);
}

स्थिर शून्य riscv_cap_pt_print_slots(pte_t *ऊर्ध्वस्थम्_Pt_Slot, word_t pt_सूचकांक, पूर्णाङ्क level)
{
    pte_t *pt;
    यदि (level == CONFIG_PT_स्तराः) {
        प्रारूप_लिख("%p_pd {\n", ऊर्ध्वस्थम्_Pt_Slot);
        pt = ऊर्ध्वस्थम्_Pt_Slot;
    } अन्यथा {
        प्रारूप_लिख("pt_%p_%04lu {\n", ऊर्ध्वस्थम्_Pt_Slot, pt_सूचकांक);
        pt = getPPtrFromHWPTE(ऊर्ध्वस्थम्_Pt_Slot);
    }
    level -= 1;

    word_t pt_द्वयङ्काः_वामः = PT_सूचकांक_द्वयङ्काः * level + se_L4_Page_द्वयङ्काः;

    /* - 1 to avoid overflowing */
    प्रति (word_t i = 0; i < BIT(pt_द्वयङ्काः_वामः + PT_सूचकांक_द्वयङ्काः) - 1; i += (1 << (pt_द्वयङ्काः_वामः))) {
        word_t pt_Slot_सूचकांक = ((i >> pt_द्वयङ्काः_वामः) & आवरणम्(PT_सूचकांक_द्वयङ्काः));
        pte_t *ptSlot = pt + pt_Slot_सूचकांक;
        यदि (pte_ptr_get_valid(ptSlot)) {
            यदि (level) { /* pt */
                प्रारूप_लिख("0x%lx: pt_%p_%04lu\n", pt_Slot_सूचकांक, ptSlot, pt_Slot_सूचकांक);
            } अन्यथा { /* frame */
                प्रारूप_लिख("0x%lx: frame_%p_%04lu", pt_Slot_सूचकांक, ptSlot, pt_Slot_सूचकांक);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    प्रारूप_लिख("}\n"); /* lvl1pt/pt */

    प्रति (word_t i = 0; i < BIT(pt_द्वयङ्काः_वामः + PT_सूचकांक_द्वयङ्काः) - 1; i += (1 << (pt_द्वयङ्काः_वामः))) {
        word_t pt_Slot_सूचकांक = ((i >> pt_द्वयङ्काः_वामः) & आवरणम्(PT_सूचकांक_द्वयङ्काः));
        pte_t *ptSlot = pt + pt_Slot_सूचकांक;
        यदि (pte_ptr_get_valid(ptSlot)) {
            यदि (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, pt_Slot_सूचकांक, level);
            }
        }
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    यदि (is_Valid_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_सम्बन्धि_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        सङ्कलय_गन्तव्यम्_आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_स्तराः);
    }
}

स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    प्रारूप_लिख("(");

    /* rights */
    यदि (pte_ptr_get_read(ptSlot)) {
        प्रारूप_लिख("R");
    }

    यदि (pte_ptr_get_write(ptSlot)) {
        प्रारूप_लिख("W");
    }

    यदि (pte_ptr_get_execute(ptSlot)) {
        प्रारूप_लिख("X");
    }

    /* cacheable not supported yet */

    प्रारूप_लिख(")\n");
}

स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    प्रतिज्ञा(lu_ret.pt_द्वयङ्काः_वामः == se_L4_Page_द्वयङ्काः);
    word_t slot = ((vptr >> lu_ret.pt_द्वयङ्काः_वामः) & आवरणम्(PT_सूचकांक_द्वयङ्काः));

    प्रारूप_लिख("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

शून्य print_cap_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_सारणी_cap: {
        asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap);

        word_t pt_द्वयङ्काः_वामः = PT_सूचकांक_द्वयङ्काः * CONFIG_PT_स्तराः + se_L4_Page_द्वयङ्काः;
        word_t slot = ((vptr >> pt_द्वयङ्काः_वामः) & आवरणम्(PT_सूचकांक_द्वयङ्काः));
        यदि (asid) {
            प्रारूप_लिख("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_मूलम्, vptr).ptSlot, slot, (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_मूलम्, vptr).ptSlot, slot);
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
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूलम्);
        भङ्ग;
    }
    पक्ष cap_asid_pool_cap: {
        प्रारूप_लिख("%p_asid_pool\n", (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        भङ्ग;
    }
    /* riscv specific caps */
    /* nothing */
    स्वभाव: {
        प्रारूप_लिख("[unknown cap %lu]\n", (दीर्घ अचिह्नित पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

स्थिर शून्य obj_frame_print_attrs(paddr_t paddr)
{
    प्रारूप_लिख("(4k, paddr: 0x%p)\n", (शून्य *)paddr);
}

शून्य print_object_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_frame_cap:
    पक्ष cap_page_सारणी_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        भङ्ग;

    पक्ष cap_asid_pool_cap: {
        प्रारूप_लिख("%p_asid_pool = asid_pool ",
               (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        भङ्ग;
    }
    /* riscv specific caps */
    /* nothing */
    स्वभाव: {
        प्रारूप_लिख("[unknown object %lu]\n", (दीर्घ अचिह्नित पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

स्थिर शून्य riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, पूर्णाङ्क level)
{
    word_t pt_द्वयङ्काः_वामः = PT_सूचकांक_द्वयङ्काः * level + se_L4_Page_द्वयङ्काः;

    प्रति (word_t i = 0; i < BIT(pt_द्वयङ्काः_वामः + PT_सूचकांक_द्वयङ्काः); i += (1 << (pt_द्वयङ्काः_वामः))) {
        word_t pt_सूचकांक = ((i >> pt_द्वयङ्काः_वामः) & आवरणम्(PT_सूचकांक_द्वयङ्काः));
        pte_t *ptSlot = pt + pt_सूचकांक;
        यदि (pte_ptr_get_valid(ptSlot)) {
            यदि (level) { /* pt */
                प्रारूप_लिख("pt_%p_%04lu = pt\n", ptSlot, pt_सूचकांक);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } अन्यथा { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                प्रारूप_लिख("frame_%p_%04lu = frame ", ptSlot, pt_सूचकांक);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    यदि (is_Valid_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        सङ्कलय_गन्तव्यम्_आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_सम्बन्धि_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        प्रारूप_लिख("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_स्तराः - 1);
    }
}

#यदि_अन्तः /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    प्रारूप_लिख("arch riscv\n");
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
#यदि_अन्तः /* CONFIG_PRINTING */
}

#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
