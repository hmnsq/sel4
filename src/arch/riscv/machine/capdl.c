/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#അന്തര്ഭാവ <machine/capdl.h>
#അന്തര്ഭാവ <arch/machine/capdl.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    തിരികെ tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

സ്ഥിതം ശൂന്യം obj_asidpool_print_attrs(cap_t asid_cap);
സ്ഥിതം ശൂന്യം obj_frame_print_attrs(paddr_t paddr);
സ്ഥിതം ശൂന്യം riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, പൂർണ്ണസംഖ്യ level);
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt);
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_pt(pte_t *ptSlot);

സ്ഥിതം ശൂന്യം obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    printf("(asid_high: 0x%lx)\n", ASID_മുകളിലെ(asid));
}

ശൂന്യം print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);

    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.vspace_വേര്);
}

സ്ഥിതം ശൂന്യം riscv_cap_pt_print_slots(pte_t *upperPtSlot, word_t ptIndex, പൂർണ്ണസംഖ്യ level)
{
    pte_t *pt;
    എങ്കിൽ (level == CONFIG_PT_LEVELS) {
        printf("%p_pd {\n", upperPtSlot);
        pt = upperPtSlot;
    } അല്ലെങ്കിൽ {
        printf("pt_%p_%04lu {\n", upperPtSlot, ptIndex);
        pt = getPPtrFromHWPTE(upperPtSlot);
    }
    level -= 1;

    word_t pt_Bits_ഇടത് = PT_INDEX_BITS * level + seL4_PageBits;

    /* - 1 to avoid overflowing */
    വേണ്ടി (word_t i = 0; i < BIT(pt_Bits_ഇടത് + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_ഇടത്))) {
        word_t ptSlotIndex = ((i >> pt_Bits_ഇടത്) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        എങ്കിൽ (pte_ptr_get_valid(ptSlot)) {
            എങ്കിൽ (level) { /* pt */
                printf("0x%lx: pt_%p_%04lu\n", ptSlotIndex, ptSlot, ptSlotIndex);
            } അല്ലെങ്കിൽ { /* frame */
                printf("0x%lx: frame_%p_%04lu", ptSlotIndex, ptSlot, ptSlotIndex);
                cap_frame_print_attrs_pt(ptSlot);
            }
        }
    }
    printf("}\n"); /* lvl1pt/pt */

    വേണ്ടി (word_t i = 0; i < BIT(pt_Bits_ഇടത് + PT_INDEX_BITS) - 1; i += (1 << (pt_Bits_ഇടത്))) {
        word_t ptSlotIndex = ((i >> pt_Bits_ഇടത്) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptSlotIndex;
        എങ്കിൽ (pte_ptr_get_valid(ptSlot)) {
            എങ്കിൽ (level) { /* pt */
                riscv_cap_pt_print_slots(ptSlot, ptSlotIndex, level);
            }
        }
    }
}

ശൂന്യം obj_vtable_print_slots(tcb_t *tcb)
{
    എങ്കിൽ (is_Valid_VTable_വേര്(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_LEVELS);
    }
}

സ്ഥിതം ശൂന്യം cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    printf("(");

    /* rights */
    എങ്കിൽ (pte_ptr_get_read(ptSlot)) {
        printf("R");
    }

    എങ്കിൽ (pte_ptr_get_write(ptSlot)) {
        printf("W");
    }

    എങ്കിൽ (pte_ptr_get_execute(ptSlot)) {
        printf("X");
    }

    /* cacheable not supported yet */

    printf(")\n");
}

സ്ഥിതം ശൂന്യം cap_frame_print_attrs_vptr(word_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    പ്രതിജ്ഞാ(lu_ret.pt_Bits_ഇടത് == seL4_PageBits);
    word_t slot = ((vptr >> lu_ret.pt_Bits_ഇടത്) & MASK(PT_INDEX_BITS));

    printf("frame_%p_%04lu ", lu_ret.ptSlot, slot);
    cap_frame_print_attrs_pt(lu_ret.ptSlot);
}

ശൂന്യം print_cap_arch(cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);

        word_t pt_Bits_ഇടത് = PT_INDEX_BITS * CONFIG_PT_LEVELS + seL4_PageBits;
        word_t slot = ((vptr >> pt_Bits_ഇടത്) & MASK(PT_INDEX_BITS));
        എങ്കിൽ (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(find_ret.vspace_വേര്, vptr).ptSlot, slot, (long unsigned പൂർണ്ണസംഖ്യ)asid);
        } അല്ലെങ്കിൽ {
            printf("pt_%p_%04lu\n", lookupPTSlot(find_ret.vspace_വേര്, vptr).ptSlot, slot);
        }
        നിർത്തുക;
    }
    അവസ്ഥ cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        നിർത്തുക;
    }
    അവസ്ഥ cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));

        പ്രതിജ്ഞാ(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.vspace_വേര്);
        നിർത്തുക;
    }
    അവസ്ഥ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ശൂന്യം *)cap_asid_pool_cap_get_capASIDPool(cap));
        നിർത്തുക;
    }
    /* riscv specific caps */
    /* nothing */
    സ്ഥിരസ്ഥിതി: {
        printf("[unknown cap %lu]\n", (long unsigned പൂർണ്ണസംഖ്യ)cap_get_capType(cap));
        നിർത്തുക;
    }
    }
}

സ്ഥിതം ശൂന്യം obj_frame_print_attrs(paddr_t paddr)
{
    printf("(4k, paddr: 0x%p)\n", (ശൂന്യം *)paddr);
}

ശൂന്യം print_object_arch(cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_frame_cap:
    അവസ്ഥ cap_page_table_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        നിർത്തുക;

    അവസ്ഥ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ശൂന്യം *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        നിർത്തുക;
    }
    /* riscv specific caps */
    /* nothing */
    സ്ഥിരസ്ഥിതി: {
        printf("[unknown object %lu]\n", (long unsigned പൂർണ്ണസംഖ്യ)cap_get_capType(cap));
        നിർത്തുക;
    }
    }
}

സ്ഥിതം ശൂന്യം riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, പൂർണ്ണസംഖ്യ level)
{
    word_t pt_Bits_ഇടത് = PT_INDEX_BITS * level + seL4_PageBits;

    വേണ്ടി (word_t i = 0; i < BIT(pt_Bits_ഇടത് + PT_INDEX_BITS); i += (1 << (pt_Bits_ഇടത്))) {
        word_t ptIndex = ((i >> pt_Bits_ഇടത്) & MASK(PT_INDEX_BITS));
        pte_t *ptSlot = pt + ptIndex;
        എങ്കിൽ (pte_ptr_get_valid(ptSlot)) {
            എങ്കിൽ (level) { /* pt */
                printf("pt_%p_%04lu = pt\n", ptSlot, ptIndex);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(ptSlot), level - 1);
            } അല്ലെങ്കിൽ { /* frame */
                paddr_t paddr = pte_ptr_get_ppn(ptSlot);
                printf("frame_%p_%04lu = frame ", ptSlot, ptIndex);
                obj_frame_print_attrs(paddr);
            }
        }
    }
}

ശൂന്യം obj_tcb_print_vtable(tcb_t *tcb)
{
    എങ്കിൽ (is_Valid_VTable_വേര്(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        printf("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_LEVELS - 1);
    }
}

#endif /* CONFIG_PRINTING */

ശൂന്യം debug_capDL(ശൂന്യം)
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
