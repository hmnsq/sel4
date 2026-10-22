/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#அந்தர்பாவ <machine/capdl.h>
#அந்தர்பாவ <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    திரும்பு tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

நிலையான வெறுமை obj_asidpool_print_attrs(cap_t asid_cap);
நிலையான வெறுமை obj_frame_print_attrs(paddr_t paddr);
நிலையான வெறுமை riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, முழு level);
நிலையான வெறுமை cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
நிலையான வெறுமை cap_frame_print_attrs_pt(pte_t *ptSlot);

நிலையான வெறுமை obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_உயர்வு(asid));
}

வெறுமை print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_வேர்);
}

நிலையான வெறுமை riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t ptIndex, முழு level)
{
    pte_t *pt;
    எனில் (level == CONFIG_PT_LEVELS) {
        printf("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } இல்லையெனில் {
        printf("pt_%p_%04lu {\n", upperPtSlot, ptIndex);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_இடது = PT_INDEX_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    க்கு (word_t i = 0; i < BIT(pt_Bits_இடது + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_இடது))) {
        word_t ptSlotIndex = ((i >> pt_Bits_இடது) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        எனில் (pte_ptr_get_valid(ptSlot)) {
            எனில் (level) { /* pt */
                printf("0x%lx: pt_%p_%04lu\n", ptSlotIndex, ptSlot, ptSlotIndex);
            } இல்லையெனில் { /* frame */
                printf("0x%lx: frame_%p_%04lu", ptSlotIndex, ptSlot, ptSlotIndex);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    printf("}\n"); /* lvl1pt/pt */

    க்கு (word_t i = 0; i < BIT(pt_Bits_இடது + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_இடது))) {
        word_t ptSlotIndex = ((i >> pt_Bits_இடது) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        எனில் (pte_ptr_get_valid(ptSlot)) {
            எனில் (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, ptSlotIndex, level);
            }
        }
    }
}

வெறுமை obj_vtable_print_slots(tcb_t *tcb)
{
    எனில் (is_Valid_VTable_வேர்(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

நிலையான வெறுமை cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    printf("(");

    /* rights */
    எனில் (pte_ptr_get_read(ptSlot)) {
        printf("R");
    }

    எனில் (pte_ptr_get_write(ptSlot)) {
        printf("W");
    }

    எனில் (pte_ptr_get_execute(ptSlot)) {
        printf("X");
    }

    /* cacheable not supported yet */

    printf(")\n");
}

நிலையான வெறுமை cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    ப்ரதிஜ்ஞா(lu_ret.pt_Bits_இடது == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_இடது) & MASK(PT_INDEX_BITS));

    printf("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

வெறுமை print_cap_arch(cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_இடது = PT_INDEX_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_இடது) & MASK(PT_INDEX_BITS));
        எனில் (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_வேர், vptr).ptSlot, slot, (long unsigned முழு)asid);
        } இல்லையெனில் {
            printf("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_வேர், vptr).ptSlot, slot);
        }
        நிறுத்து;
    }
    நிலை cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        நிறுத்து;
    }
    நிலை cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));

        ப்ரதிஜ்ஞா(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_வேர்);
        நிறுத்து;
    }
    நிலை cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (வெறுமை *)cap_asid_pool_cap_get_capASIDPool(cap));
        நிறுத்து;
    }
    /* riscv specific caps */
    /* nothing */
    இயல்பு: {
        printf("[unknown cap %lu]\n", (long unsigned முழு)cap_get_capType(cap));
        நிறுத்து;
    }
    }
}

நிலையான வெறுமை obj_frame_print_attrs(paddr_t paddr)
{
    printf("(4k, paddr: 0x%p)\n", (வெறுமை *)paddr);
}

வெறுமை print_object_arch(cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_frame_cap:
    நிலை cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        நிறுத்து;

    நிலை cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (வெறுமை *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        நிறுத்து;
    }
    /* riscv specific caps */
    /* nothing */
    இயல்பு: {
        printf("[unknown object %lu]\n", (long unsigned முழு)cap_get_capType(cap));
        நிறுத்து;
    }
    }
}

நிலையான வெறுமை riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, முழு level)
{
    word_t pt_Bits_இடது = PT_INDEX_BITS * level + seL4_PageBits;

    க்கு (word_t i = 0; i < BIT(pt_Bits_இடது + PT_INDEX_BITS); i += (1 << (pt_Bits_இடது))) {
        word_t ptIndex = ((i >> pt_Bits_இடது) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptIndex;
        எனில் (pte_ptr_get_valid(ptSlot)) {
            எனில் (level) { /* pt */
                printf("pt_%p_%04lu = pt\n", ptSlot, ptIndex);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } இல்லையெனில் { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                printf("frame_%p_%04lu = frame ", ptSlot, ptIndex);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

வெறுமை obj_tcb_print_vtable(tcb_t *tcb)
{
    எனில் (is_Valid_VTable_வேர்(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        printf("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

வெறுமை debug_capDL(வெறுமை)
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
