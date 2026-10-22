/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_DEBUG_BUILD

#समावेश <machine/capdl.h>
#समावेश <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    वापस tcb->tcbArch.tcbContext.registers[SP];
}

#अगर_घोषित CONFIG_PRINTING

अटल व्योम obj_asidpool_print_attrs(cap_t asid_cap);
अटल व्योम obj_frame_print_attrs(paddr_t paddr);
अटल व्योम riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, पूर्णांक level);
अटल व्योम cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
अटल व्योम cap_frame_print_attrs_pt(pte_t *ptSlot);

अटल व्योम obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    म_लिखो("(asid_high: 0x%lx)\n", ASID_ऊपरी(asid));
}

व्योम print_ipc_बफर_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    म_लिखो("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूल);
}

अटल व्योम riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t pt_सूचकांक, पूर्णांक level)
{
    pte_t *pt;
    अगर (level == CONFIG_PT_LEVELS) {
        म_लिखो("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } अन्यथा {
        म_लिखो("pt_%p_%04lu {\n", upperPtSlot, pt_सूचकांक);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_बायाँ = PT_सूचकांक_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    क्रम (word_t i = 0; i < BIT(pt_Bits_बायाँ + PT_सूचकांक_BITS) - 1; i += (1 << (pt_Bits_बायाँ))) {
        word_t pt_Slot_सूचकांक = ((i >> pt_Bits_बायाँ) & MASK(PT_सूचकांक_BITS));
        pte_t *ptSlot = pt + pt_Slot_सूचकांक;
        अगर (pte_ptr_get_valid(ptSlot)) {
            अगर (level) { /* pt */
                म_लिखो("0x%lx: pt_%p_%04lu\n", pt_Slot_सूचकांक, ptSlot, pt_Slot_सूचकांक);
            } अन्यथा { /* frame */
                म_लिखो("0x%lx: frame_%p_%04lu", pt_Slot_सूचकांक, ptSlot, pt_Slot_सूचकांक);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    म_लिखो("}\n"); /* lvl1pt/pt */

    क्रम (word_t i = 0; i < BIT(pt_Bits_बायाँ + PT_सूचकांक_BITS) - 1; i += (1 << (pt_Bits_बायाँ))) {
        word_t pt_Slot_सूचकांक = ((i >> pt_Bits_बायाँ) & MASK(PT_सूचकांक_BITS));
        pte_t *ptSlot = pt + pt_Slot_सूचकांक;
        अगर (pte_ptr_get_valid(ptSlot)) {
            अगर (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, pt_Slot_सूचकांक, level);
            }
        }
    }
}

व्योम obj_vtable_print_slots(tcb_t *tcb)
{
    अगर (is_Valid_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

अटल व्योम cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    म_लिखो("(");

    /* rights */
    अगर (pte_ptr_get_read(ptSlot)) {
        म_लिखो("R");
    }

    अगर (pte_ptr_get_write(ptSlot)) {
        म_लिखो("W");
    }

    अगर (pte_ptr_get_execute(ptSlot)) {
        म_लिखो("X");
    }

    /* cacheable not supported yet */

    म_लिखो(")\n");
}

अटल व्योम cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    निश्चित(lu_ret.pt_Bits_बायाँ == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_बायाँ) & MASK(PT_सूचकांक_BITS));

    म_लिखो("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

व्योम print_cap_arch(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_बायाँ = PT_सूचकांक_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_बायाँ) & MASK(PT_सूचकांक_BITS));
        अगर (asid) {
            म_लिखो("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_मूल, vptr).ptSlot, slot, (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_मूल, vptr).ptSlot, slot);
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
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूल);
        अवरोध;
    }
    हाल cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool\n", (व्योम *)cap_asid_pool_cap_get_capASIDPool(cap));
        अवरोध;
    }
    /* riscv specific caps */
    /* nothing */
    शेष: {
        म_लिखो("[unknown cap %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        अवरोध;
    }
    }
}

अटल व्योम obj_frame_print_attrs(paddr_t paddr)
{
    म_लिखो("(4k, paddr: 0x%p)\n", (व्योम *)paddr);
}

व्योम print_object_arch(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_frame_cap:
    हाल cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        अवरोध;

    हाल cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool = asid_pool ",
               (व्योम *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        अवरोध;
    }
    /* riscv specific caps */
    /* nothing */
    शेष: {
        म_लिखो("[unknown object %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        अवरोध;
    }
    }
}

अटल व्योम riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, पूर्णांक level)
{
    word_t pt_Bits_बायाँ = PT_सूचकांक_BITS * level + seL4_PageBits;

    क्रम (word_t i = 0; i < BIT(pt_Bits_बायाँ + PT_सूचकांक_BITS); i += (1 << (pt_Bits_बायाँ))) {
        word_t pt_सूचकांक = ((i >> pt_Bits_बायाँ) & MASK(PT_सूचकांक_BITS));
        pte_t *ptSlot = pt + pt_सूचकांक;
        अगर (pte_ptr_get_valid(ptSlot)) {
            अगर (level) { /* pt */
                म_लिखो("pt_%p_%04lu = pt\n", ptSlot, pt_सूचकांक);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } अन्यथा { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                म_लिखो("frame_%p_%04lu = frame ", ptSlot, pt_सूचकांक);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

व्योम obj_tcb_print_vtable(tcb_t *tcb)
{
    अगर (is_Valid_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        म_लिखो("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#पूर्ण_अगर /* CONFIG_PRINTING */

व्योम debug_capDL(व्योम)
{
    म_लिखो("arch riscv\n");
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
#पूर्ण_अगर /* CONFIG_PRINTING */
}

#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
