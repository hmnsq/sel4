/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    ಪ್ರತ್ಯಾಗಮ tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

ಸ್ಥಿರ ಶೂನ್ಯ obj_asidpool_print_attrs(cap_t asid_cap);
ಸ್ಥಿರ ಶೂನ್ಯ obj_frame_print_attrs(paddr_t paddr);
ಸ್ಥಿರ ಶೂನ್ಯ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, ಪೂರ್ಣಾಙ್ಕ level);
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_pt(pte_t *ptSlot);

ಸ್ಥಿರ ಶೂನ್ಯ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_ಮೇಲಿನ(asid));
}

ಶೂನ್ಯ print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ಬೇರು);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t ptIndex, ಪೂರ್ಣಾಙ್ಕ level)
{
    pte_t *pt;
    ಯದಿ (level == CONFIG_PT_LEVELS) {
        printf("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } ಅನ್ಯಥಾ {
        printf("pt_%p_%04lu {\n", upperPtSlot, ptIndex);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_ಎಡ = PT_INDEX_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    ಪ್ರತಿ (word_t i = 0; i < BIT(pt_Bits_ಎಡ + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_ಎಡ))) {
        word_t ptSlotIndex = ((i >> pt_Bits_ಎಡ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        ಯದಿ (pte_ptr_get_valid(ptSlot)) {
            ಯದಿ (level) { /* pt */
                printf("0x%lx: pt_%p_%04lu\n", ptSlotIndex, ptSlot, ptSlotIndex);
            } ಅನ್ಯಥಾ { /* frame */
                printf("0x%lx: frame_%p_%04lu", ptSlotIndex, ptSlot, ptSlotIndex);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    printf("}\n"); /* lvl1pt/pt */

    ಪ್ರತಿ (word_t i = 0; i < BIT(pt_Bits_ಎಡ + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_ಎಡ))) {
        word_t ptSlotIndex = ((i >> pt_Bits_ಎಡ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        ಯದಿ (pte_ptr_get_valid(ptSlot)) {
            ಯದಿ (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, ptSlotIndex, level);
            }
        }
    }
}

ಶೂನ್ಯ obj_vtable_print_slots(tcb_t *tcb)
{
    ಯದಿ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    printf("(");

    /* rights */
    ಯದಿ (pte_ptr_get_read(ptSlot)) {
        printf("R");
    }

    ಯದಿ (pte_ptr_get_write(ptSlot)) {
        printf("W");
    }

    ಯದಿ (pte_ptr_get_execute(ptSlot)) {
        printf("X");
    }

    /* cacheable not supported yet */

    printf(")\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    ಪ್ರತಿಜ್ಞಾ(lu_ret.pt_Bits_ಎಡ == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_ಎಡ) & MASK(PT_INDEX_BITS));

    printf("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

ಶೂನ್ಯ print_cap_arch(cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_ಎಡ = PT_INDEX_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_ಎಡ) & MASK(PT_INDEX_BITS));
        ಯದಿ (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_ಬೇರು, vptr).ptSlot, slot, (long unsigned ಪೂರ್ಣಾಙ್ಕ)asid);
        } ಅನ್ಯಥಾ {
            printf("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_ಬೇರು, vptr).ptSlot, slot);
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));

        ಪ್ರತಿಜ್ಞಾ(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ಬೇರು);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ಭಙ್ಗ;
    }
    /* riscv specific caps */
    /* nothing */
    ಸ್ವಭಾವ: {
        printf("[unknown cap %lu]\n", (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_get_capType(cap));
        ಭಙ್ಗ;
    }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ obj_frame_print_attrs(paddr_t paddr)
{
    printf("(4k, paddr: 0x%p)\n", (ಶೂನ್ಯ *)paddr);
}

ಶೂನ್ಯ print_object_arch(cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_frame_cap:
    ಪಕ್ಷ cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ಭಙ್ಗ;
    }
    /* riscv specific caps */
    /* nothing */
    ಸ್ವಭಾವ: {
        printf("[unknown object %lu]\n", (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_get_capType(cap));
        ಭಙ್ಗ;
    }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, ಪೂರ್ಣಾಙ್ಕ level)
{
    word_t pt_Bits_ಎಡ = PT_INDEX_BITS * level + seL4_PageBits;

    ಪ್ರತಿ (word_t i = 0; i < BIT(pt_Bits_ಎಡ + PT_INDEX_BITS); i += (1 << (pt_Bits_ಎಡ))) {
        word_t ptIndex = ((i >> pt_Bits_ಎಡ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptIndex;
        ಯದಿ (pte_ptr_get_valid(ptSlot)) {
            ಯದಿ (level) { /* pt */
                printf("pt_%p_%04lu = pt\n", ptSlot, ptIndex);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } ಅನ್ಯಥಾ { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                printf("frame_%p_%04lu = frame ", ptSlot, ptIndex);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

ಶೂನ್ಯ obj_tcb_print_vtable(tcb_t *tcb)
{
    ಯದಿ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        printf("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

ಶೂನ್ಯ debug_capDL(ಶೂನ್ಯ)
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
