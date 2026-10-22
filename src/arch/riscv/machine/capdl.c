/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_DEBUG_BUILD

#অন্তর্ভাব <machine/capdl.h>
#অন্তর্ভাব <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    ফেরত tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

স্থির শূন্য obj_asidpool_print_attrs(cap_t asid_cap);
স্থির শূন্য obj_frame_print_attrs(paddr_t paddr);
স্থির শূন্য riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, পূর্ণসংখ্যা level);
স্থির শূন্য cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
স্থির শূন্য cap_frame_print_attrs_pt(pte_t *ptSlot);

স্থির শূন্য obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_উপরের(asid));
}

শূন্য print_ipc_বাফার_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_মূল);
}

স্থির শূন্য riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t pt_সূচক, পূর্ণসংখ্যা level)
{
    pte_t *pt;
    যদি (level == CONFIG_PT_LEVELS) {
        printf("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } অন্যথা {
        printf("pt_%p_%04lu {\n", upperPtSlot, pt_সূচক);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_বাম = PT_সূচক_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    জন্য (word_t i = 0; i < BIT(pt_Bits_বাম + PT_সূচক_BITS) - 1; i += (1 << (pt_Bits_বাম))) {
        word_t pt_Slot_সূচক = ((i >> pt_Bits_বাম) & MASK(PT_সূচক_BITS));
        pte_t *ptSlot = pt + pt_Slot_সূচক;
        যদি (pte_ptr_get_valid(ptSlot)) {
            যদি (level) { /* pt */
                printf("0x%lx: pt_%p_%04lu\n", pt_Slot_সূচক, ptSlot, pt_Slot_সূচক);
            } অন্যথা { /* frame */
                printf("0x%lx: frame_%p_%04lu", pt_Slot_সূচক, ptSlot, pt_Slot_সূচক);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    printf("}\n"); /* lvl1pt/pt */

    জন্য (word_t i = 0; i < BIT(pt_Bits_বাম + PT_সূচক_BITS) - 1; i += (1 << (pt_Bits_বাম))) {
        word_t pt_Slot_সূচক = ((i >> pt_Bits_বাম) & MASK(PT_সূচক_BITS));
        pte_t *ptSlot = pt + pt_Slot_সূচক;
        যদি (pte_ptr_get_valid(ptSlot)) {
            যদি (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, pt_Slot_সূচক, level);
            }
        }
    }
}

শূন্য obj_vtable_print_slots(tcb_t *tcb)
{
    যদি (is_Valid_VTable_মূল(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

স্থির শূন্য cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    printf("(");

    /* rights */
    যদি (pte_ptr_get_read(ptSlot)) {
        printf("R");
    }

    যদি (pte_ptr_get_write(ptSlot)) {
        printf("W");
    }

    যদি (pte_ptr_get_execute(ptSlot)) {
        printf("X");
    }

    /* cacheable not supported yet */

    printf(")\n");
}

স্থির শূন্য cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    প্রতিজ্ঞা(lu_ret.pt_Bits_বাম == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_বাম) & MASK(PT_সূচক_BITS));

    printf("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

শূন্য print_cap_arch(cap_t cap)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_বাম = PT_সূচক_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_বাম) & MASK(PT_সূচক_BITS));
        যদি (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_মূল, vptr).ptSlot, slot, (long unsigned পূর্ণসংখ্যা)asid);
        } অন্যথা {
            printf("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_মূল, vptr).ptSlot, slot);
        }
        বিরতি;
    }
    ক্ষেত্র cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        বিরতি;
    }
    ক্ষেত্র cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));

        প্রতিজ্ঞা(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_মূল);
        বিরতি;
    }
    ক্ষেত্র cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (শূন্য *)cap_asid_pool_cap_get_capASIDPool(cap));
        বিরতি;
    }
    /* riscv specific caps */
    /* nothing */
    পূর্বনির্ধারিত: {
        printf("[unknown cap %lu]\n", (long unsigned পূর্ণসংখ্যা)cap_get_capType(cap));
        বিরতি;
    }
    }
}

স্থির শূন্য obj_frame_print_attrs(paddr_t paddr)
{
    printf("(4k, paddr: 0x%p)\n", (শূন্য *)paddr);
}

শূন্য print_object_arch(cap_t cap)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_frame_cap:
    ক্ষেত্র cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        বিরতি;

    ক্ষেত্র cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (শূন্য *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        বিরতি;
    }
    /* riscv specific caps */
    /* nothing */
    পূর্বনির্ধারিত: {
        printf("[unknown object %lu]\n", (long unsigned পূর্ণসংখ্যা)cap_get_capType(cap));
        বিরতি;
    }
    }
}

স্থির শূন্য riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, পূর্ণসংখ্যা level)
{
    word_t pt_Bits_বাম = PT_সূচক_BITS * level + seL4_PageBits;

    জন্য (word_t i = 0; i < BIT(pt_Bits_বাম + PT_সূচক_BITS); i += (1 << (pt_Bits_বাম))) {
        word_t pt_সূচক = ((i >> pt_Bits_বাম) & MASK(PT_সূচক_BITS));
        pte_t *ptSlot = pt + pt_সূচক;
        যদি (pte_ptr_get_valid(ptSlot)) {
            যদি (level) { /* pt */
                printf("pt_%p_%04lu = pt\n", ptSlot, pt_সূচক);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } অন্যথা { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                printf("frame_%p_%04lu = frame ", ptSlot, pt_সূচক);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

শূন্য obj_tcb_print_vtable(tcb_t *tcb)
{
    যদি (is_Valid_VTable_মূল(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        printf("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

শূন্য debug_capDL(শূন্য)
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
