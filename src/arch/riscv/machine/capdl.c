/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#ifdef CONFIG_DEBUG_BUILD

#شامل <machine/capdl.h>
#شامل <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    بازگشت tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

ایستا تهی obj_asidpool_print_attrs(cap_t asid_cap);
ایستا تهی obj_frame_print_attrs(paddr_t paddr);
ایستا تهی riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحیح level);
ایستا تهی cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
ایستا تهی cap_frame_print_attrs_pt(pte_t *ptSlot);

ایستا تهی obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_بالا(asid));
}

تهی print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ریشه);
}

ایستا تهی riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t ptIndex, صحیح level)
{
    pte_t *pt;
    اگر (level == CONFIG_PT_LEVELS) {
        printf("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } وگرنه {
        printf("pt_%p_%04lu {\n", upperPtSlot, ptIndex);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_چپ = PT_INDEX_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    برای (word_t i = 0; i < BIT(pt_Bits_چپ + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_چپ))) {
        word_t ptSlotIndex = ((i >> pt_Bits_چپ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        اگر (pte_ptr_get_valid(ptSlot)) {
            اگر (level) { /* pt */
                printf("0x%lx: pt_%p_%04lu\n", ptSlotIndex, ptSlot, ptSlotIndex);
            } وگرنه { /* frame */
                printf("0x%lx: frame_%p_%04lu", ptSlotIndex, ptSlot, ptSlotIndex);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    printf("}\n"); /* lvl1pt/pt */

    برای (word_t i = 0; i < BIT(pt_Bits_چپ + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_چپ))) {
        word_t ptSlotIndex = ((i >> pt_Bits_چپ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        اگر (pte_ptr_get_valid(ptSlot)) {
            اگر (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, ptSlotIndex, level);
            }
        }
    }
}

تهی obj_vtable_print_slots(tcb_t *tcb)
{
    اگر (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

ایستا تهی cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    printf("(");

    /* rights */
    اگر (pte_ptr_get_read(ptSlot)) {
        printf("R");
    }

    اگر (pte_ptr_get_write(ptSlot)) {
        printf("W");
    }

    اگر (pte_ptr_get_execute(ptSlot)) {
        printf("X");
    }

    /* cacheable not supported yet */

    printf(")\n");
}

ایستا تهی cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    أكد(lu_ret.pt_Bits_چپ == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_چپ) & MASK(PT_INDEX_BITS));

    printf("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

تهی print_cap_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_چپ = PT_INDEX_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_چپ) & MASK(PT_INDEX_BITS));
        اگر (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_ریشه, vptr).ptSlot, slot, (long unsigned صحیح)asid);
        } وگرنه {
            printf("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_ریشه, vptr).ptSlot, slot);
        }
        بشکن;
    }
    حالت cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        بشکن;
    }
    حالت cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));

        أكد(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ریشه);
        بشکن;
    }
    حالت cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (تهی *)cap_asid_pool_cap_get_capASIDPool(cap));
        بشکن;
    }
    /* riscv specific caps */
    /* nothing */
    پیشفرض: {
        printf("[unknown cap %lu]\n", (long unsigned صحیح)cap_get_capType(cap));
        بشکن;
    }
    }
}

ایستا تهی obj_frame_print_attrs(paddr_t paddr)
{
    printf("(4k, paddr: 0x%p)\n", (تهی *)paddr);
}

تهی print_object_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_frame_cap:
    حالت cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        بشکن;

    حالت cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (تهی *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        بشکن;
    }
    /* riscv specific caps */
    /* nothing */
    پیشفرض: {
        printf("[unknown object %lu]\n", (long unsigned صحیح)cap_get_capType(cap));
        بشکن;
    }
    }
}

ایستا تهی riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحیح level)
{
    word_t pt_Bits_چپ = PT_INDEX_BITS * level + seL4_PageBits;

    برای (word_t i = 0; i < BIT(pt_Bits_چپ + PT_INDEX_BITS); i += (1 << (pt_Bits_چپ))) {
        word_t ptIndex = ((i >> pt_Bits_چپ) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptIndex;
        اگر (pte_ptr_get_valid(ptSlot)) {
            اگر (level) { /* pt */
                printf("pt_%p_%04lu = pt\n", ptSlot, ptIndex);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } وگرنه { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                printf("frame_%p_%04lu = frame ", ptSlot, ptIndex);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

تهی obj_tcb_print_vtable(tcb_t *tcb)
{
    اگر (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        printf("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

تهی debug_capDL(تهی)
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
