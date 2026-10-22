/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ଅନ୍ତର୍ଭାଵ <machine/capdl.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    ଫେରନ୍ତୁ tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

ସ୍ଥିର ଶୂନ୍ୟ obj_asidpool_print_attrs(cap_t asid_cap);
ସ୍ଥିର ଶୂନ୍ୟ obj_frame_print_attrs(paddr_t paddr);
ସ୍ଥିର ଶୂନ୍ୟ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level);
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_pt(pte_t *ptSlot);

ସ୍ଥିର ଶୂନ୍ୟ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_ଉପର(asid));
}

ଶୂନ୍ୟ print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ମୂଳ);
}

ସ୍ଥିର ଶୂନ୍ୟ riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t ptIndex, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level)
{
    pte_t *pt;
    ଯଦି (level == CONFIG_PT_LEVELS) {
        printf("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } ଅନ୍ୟଥା {
        printf("pt_%p_%04lu {\n", upperPtSlot, ptIndex);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_ବାମ = PT_INDEX_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    ପାଇଁ (word_t i = 0; i < BIT(pt_Bits_ବାମ + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_ବାମ))) {
        word_t ptSlotIndex = ((i >> pt_Bits_ବାମ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        ଯଦି (pte_ptr_get_valid(ptSlot)) {
            ଯଦି (level) { /* pt */
                printf("0x%lx: pt_%p_%04lu\n", ptSlotIndex, ptSlot, ptSlotIndex);
            } ଅନ୍ୟଥା { /* frame */
                printf("0x%lx: frame_%p_%04lu", ptSlotIndex, ptSlot, ptSlotIndex);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    printf("}\n"); /* lvl1pt/pt */

    ପାଇଁ (word_t i = 0; i < BIT(pt_Bits_ବାମ + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_ବାମ))) {
        word_t ptSlotIndex = ((i >> pt_Bits_ବାମ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        ଯଦି (pte_ptr_get_valid(ptSlot)) {
            ଯଦି (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, ptSlotIndex, level);
            }
        }
    }
}

ଶୂନ୍ୟ obj_vtable_print_slots(tcb_t *tcb)
{
    ଯଦି (is_Valid_VTable_ମୂଳ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    printf("(");

    /* rights */
    ଯଦି (pte_ptr_get_read(ptSlot)) {
        printf("R");
    }

    ଯଦି (pte_ptr_get_write(ptSlot)) {
        printf("W");
    }

    ଯଦି (pte_ptr_get_execute(ptSlot)) {
        printf("X");
    }

    /* cacheable not supported yet */

    printf(")\n");
}

ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    ପ୍ରତିଜ୍ଞା(lu_ret.pt_Bits_ବାମ == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_ବାମ) & MASK(PT_INDEX_BITS));

    printf("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

ଶୂନ୍ୟ print_cap_arch(cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_ବାମ = PT_INDEX_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_ବାମ) & MASK(PT_INDEX_BITS));
        ଯଦି (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_ମୂଳ, vptr).ptSlot, slot, (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)asid);
        } ଅନ୍ୟଥା {
            printf("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_ମୂଳ, vptr).ptSlot, slot);
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));

        ପ୍ରତିଜ୍ଞା(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ମୂଳ);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ଶୂନ୍ୟ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ଭାଙ୍ଗ;
    }
    /* riscv specific caps */
    /* nothing */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
        printf("[unknown cap %lu]\n", (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_get_capType(cap));
        ଭାଙ୍ଗ;
    }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ obj_frame_print_attrs(paddr_t paddr)
{
    printf("(4k, paddr: 0x%p)\n", (ଶୂନ୍ୟ *)paddr);
}

ଶୂନ୍ୟ print_object_arch(cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_frame_cap:
    ସ୍ଥିତି cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ଶୂନ୍ୟ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
    /* riscv specific caps */
    /* nothing */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
        printf("[unknown object %lu]\n", (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_get_capType(cap));
        ଭାଙ୍ଗ;
    }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level)
{
    word_t pt_Bits_ବାମ = PT_INDEX_BITS * level + seL4_PageBits;

    ପାଇଁ (word_t i = 0; i < BIT(pt_Bits_ବାମ + PT_INDEX_BITS); i += (1 << (pt_Bits_ବାମ))) {
        word_t ptIndex = ((i >> pt_Bits_ବାମ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptIndex;
        ଯଦି (pte_ptr_get_valid(ptSlot)) {
            ଯଦି (level) { /* pt */
                printf("pt_%p_%04lu = pt\n", ptSlot, ptIndex);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } ଅନ୍ୟଥା { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                printf("frame_%p_%04lu = frame ", ptSlot, ptIndex);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

ଶୂନ୍ୟ obj_tcb_print_vtable(tcb_t *tcb)
{
    ଯଦି (is_Valid_VTable_ମୂଳ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        printf("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

ଶୂନ୍ୟ debug_capDL(ଶୂନ୍ୟ)
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
