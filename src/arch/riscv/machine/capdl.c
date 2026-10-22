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
    प्रत्यागम tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap);
स्थिर शून्य obj_frame_print_attrs(paddr_t paddr);
स्थिर शून्य riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, पूर्णाङ्क level);
स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot);

स्थिर शून्य obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_ऊपरी(asid));
}

शून्य print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूल);
}

स्थिर शून्य riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t ptIndex, पूर्णाङ्क level)
{
    pte_t *pt;
    यदि (level == CONFIG_PT_LEVELS) {
        printf("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } अन्यथा {
        printf("pt_%p_%04lu {\n", upperPtSlot, ptIndex);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_बायाँ = PT_INDEX_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    प्रति (word_t i = 0; i < BIT(pt_Bits_बायाँ + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_बायाँ))) {
        word_t ptSlotIndex = ((i >> pt_Bits_बायाँ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        यदि (pte_ptr_get_valid(ptSlot)) {
            यदि (level) { /* pt */
                printf("0x%lx: pt_%p_%04lu\n", ptSlotIndex, ptSlot, ptSlotIndex);
            } अन्यथा { /* frame */
                printf("0x%lx: frame_%p_%04lu", ptSlotIndex, ptSlot, ptSlotIndex);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    printf("}\n"); /* lvl1pt/pt */

    प्रति (word_t i = 0; i < BIT(pt_Bits_बायाँ + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_बायाँ))) {
        word_t ptSlotIndex = ((i >> pt_Bits_बायाँ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        यदि (pte_ptr_get_valid(ptSlot)) {
            यदि (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, ptSlotIndex, level);
            }
        }
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    यदि (is_Valid_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    printf("(");

    /* rights */
    यदि (pte_ptr_get_read(ptSlot)) {
        printf("R");
    }

    यदि (pte_ptr_get_write(ptSlot)) {
        printf("W");
    }

    यदि (pte_ptr_get_execute(ptSlot)) {
        printf("X");
    }

    /* cacheable not supported yet */

    printf(")\n");
}

स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    प्रतिज्ञा(lu_ret.pt_Bits_बायाँ == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_बायाँ) & MASK(PT_INDEX_BITS));

    printf("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

शून्य print_cap_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_बायाँ = PT_INDEX_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_बायाँ) & MASK(PT_INDEX_BITS));
        यदि (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_मूल, vptr).ptSlot, slot, (long unsigned पूर्णाङ्क)asid);
        } अन्यथा {
            printf("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_मूल, vptr).ptSlot, slot);
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
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूल);
        भङ्ग;
    }
    पक्ष cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        भङ्ग;
    }
    /* riscv specific caps */
    /* nothing */
    स्वभाव: {
        printf("[unknown cap %lu]\n", (long unsigned पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

स्थिर शून्य obj_frame_print_attrs(paddr_t paddr)
{
    printf("(4k, paddr: 0x%p)\n", (शून्य *)paddr);
}

शून्य print_object_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_frame_cap:
    पक्ष cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        भङ्ग;

    पक्ष cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        भङ्ग;
    }
    /* riscv specific caps */
    /* nothing */
    स्वभाव: {
        printf("[unknown object %lu]\n", (long unsigned पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

स्थिर शून्य riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, पूर्णाङ्क level)
{
    word_t pt_Bits_बायाँ = PT_INDEX_BITS * level + seL4_PageBits;

    प्रति (word_t i = 0; i < BIT(pt_Bits_बायाँ + PT_INDEX_BITS); i += (1 << (pt_Bits_बायाँ))) {
        word_t ptIndex = ((i >> pt_Bits_बायाँ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptIndex;
        यदि (pte_ptr_get_valid(ptSlot)) {
            यदि (level) { /* pt */
                printf("pt_%p_%04lu = pt\n", ptSlot, ptIndex);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } अन्यथा { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                printf("frame_%p_%04lu = frame ", ptSlot, ptIndex);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    यदि (is_Valid_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        printf("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    printf("arch riscv\n");
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
#endif /* CONFIG_PRINTING */
}

#endif /* CONFIG_DEBUG_BUILD */
