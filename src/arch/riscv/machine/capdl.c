/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#અન્તર્ભાવ <machine/capdl.h>
#અન્તર્ભાવ <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    પાછા tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

સ્થિર શૂન્ય obj_asidpool_print_attrs(cap_t asid_cap);
સ્થિર શૂન્ય obj_frame_print_attrs(paddr_t paddr);
સ્થિર શૂન્ય riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, પૂર્ણાંક level);
સ્થિર શૂન્ય cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
સ્થિર શૂન્ય cap_frame_print_attrs_pt(pte_t *ptSlot);

સ્થિર શૂન્ય obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_ઉપલું(asid));
}

શૂન્ય print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_મૂળ);
}

સ્થિર શૂન્ય riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t ptIndex, પૂર્ણાંક level)
{
    pte_t *pt;
    જો (level == CONFIG_PT_LEVELS) {
        printf("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } નહીંતર {
        printf("pt_%p_%04lu {\n", upperPtSlot, ptIndex);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_ડાબું = PT_INDEX_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    માટે (word_t i = 0; i < BIT(pt_Bits_ડાબું + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_ડાબું))) {
        word_t ptSlotIndex = ((i >> pt_Bits_ડાબું) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        જો (pte_ptr_get_valid(ptSlot)) {
            જો (level) { /* pt */
                printf("0x%lx: pt_%p_%04lu\n", ptSlotIndex, ptSlot, ptSlotIndex);
            } નહીંતર { /* frame */
                printf("0x%lx: frame_%p_%04lu", ptSlotIndex, ptSlot, ptSlotIndex);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    printf("}\n"); /* lvl1pt/pt */

    માટે (word_t i = 0; i < BIT(pt_Bits_ડાબું + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_ડાબું))) {
        word_t ptSlotIndex = ((i >> pt_Bits_ડાબું) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        જો (pte_ptr_get_valid(ptSlot)) {
            જો (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, ptSlotIndex, level);
            }
        }
    }
}

શૂન્ય obj_vtable_print_slots(tcb_t *tcb)
{
    જો (is_Valid_VTable_મૂળ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

સ્થિર શૂન્ય cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    printf("(");

    /* rights */
    જો (pte_ptr_get_read(ptSlot)) {
        printf("R");
    }

    જો (pte_ptr_get_write(ptSlot)) {
        printf("W");
    }

    જો (pte_ptr_get_execute(ptSlot)) {
        printf("X");
    }

    /* cacheable not supported yet */

    printf(")\n");
}

સ્થિર શૂન્ય cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    પ્રતિજ્ઞા(lu_ret.pt_Bits_ડાબું == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_ડાબું) & MASK(PT_INDEX_BITS));

    printf("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

શૂન્ય print_cap_arch(cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_ડાબું = PT_INDEX_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_ડાબું) & MASK(PT_INDEX_BITS));
        જો (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_મૂળ, vptr).ptSlot, slot, (long unsigned પૂર્ણાંક)asid);
        } નહીંતર {
            printf("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_મૂળ, vptr).ptSlot, slot);
        }
        તોડો;
    }
    સ્થિતિ cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        તોડો;
    }
    સ્થિતિ cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));

        પ્રતિજ્ઞા(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_મૂળ);
        તોડો;
    }
    સ્થિતિ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (શૂન્ય *)cap_asid_pool_cap_get_capASIDPool(cap));
        તોડો;
    }
    /* riscv specific caps */
    /* nothing */
    મૂળભૂત: {
        printf("[unknown cap %lu]\n", (long unsigned પૂર્ણાંક)cap_get_capType(cap));
        તોડો;
    }
    }
}

સ્થિર શૂન્ય obj_frame_print_attrs(paddr_t paddr)
{
    printf("(4k, paddr: 0x%p)\n", (શૂન્ય *)paddr);
}

શૂન્ય print_object_arch(cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_frame_cap:
    સ્થિતિ cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        તોડો;

    સ્થિતિ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (શૂન્ય *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        તોડો;
    }
    /* riscv specific caps */
    /* nothing */
    મૂળભૂત: {
        printf("[unknown object %lu]\n", (long unsigned પૂર્ણાંક)cap_get_capType(cap));
        તોડો;
    }
    }
}

સ્થિર શૂન્ય riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, પૂર્ણાંક level)
{
    word_t pt_Bits_ડાબું = PT_INDEX_BITS * level + seL4_PageBits;

    માટે (word_t i = 0; i < BIT(pt_Bits_ડાબું + PT_INDEX_BITS); i += (1 << (pt_Bits_ડાબું))) {
        word_t ptIndex = ((i >> pt_Bits_ડાબું) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptIndex;
        જો (pte_ptr_get_valid(ptSlot)) {
            જો (level) { /* pt */
                printf("pt_%p_%04lu = pt\n", ptSlot, ptIndex);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } નહીંતર { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                printf("frame_%p_%04lu = frame ", ptSlot, ptIndex);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

શૂન્ય obj_tcb_print_vtable(tcb_t *tcb)
{
    જો (is_Valid_VTable_મૂળ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        printf("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

શૂન્ય debug_capDL(શૂન્ય)
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
