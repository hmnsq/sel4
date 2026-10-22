/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#അന്തര്ഭാവ <arch/machine/capdl.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    തിരികെ tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

സ്ഥിതം ശൂന്യം obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase);
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_pt(pte_t *ptSlot);
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

സ്ഥിതം ശൂന്യം _cap_frame_print_attrs_vptr(word_t vptr, vspace_വേര്_t *vspace_വേര്_2);

സ്ഥിതം ശൂന്യം arm64_obj_pt_print_slots(pte_t *pdSlot);
സ്ഥിതം ശൂന്യം arm64_obj_pd_print_slots(pte_t *pudSlot);
സ്ഥിതം ശൂന്യം arm64_obj_pud_print_slots(ശൂന്യം *pgdSlot_or_vspace);

സ്ഥിതം ശൂന്യം arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
സ്ഥിതം ശൂന്യം arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
സ്ഥിതം ശൂന്യം arm64_cap_pud_print_slots(ശൂന്യം *pgdSlot_or_vspace, vptr_t vptr);

/* Stage-1 access permissions:
 * AP[2:1]  higher EL        EL0
 *   00       rw            None
 *   01       rw            rw
 *   10       r             None
 *   11       r             r
 *
 * Stage-2 access permissions:
 * S2AP    Access from Nonsecure EL1 or Non-secure EL0
 *  00                      None
 *  01                      r
 *  10                      w
 *  11                      rw
 *
 *  For VMs or native seL4 applications, if hypervisor support
 *  is enabled, we use the S2AP. The kernel itself running in
 *  EL2 still uses the Stage-1 AP format.
 */
/* use when only have access to pte of frames */
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

സ്ഥിതം ശൂന്യം cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    printf("(");

    /* rights */
    തിരഞ്ഞെടുപ്പ് (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ 0b00:
        നിർത്തുക;
    അവസ്ഥ 0b01:
        printf("R");
        നിർത്തുക;
    അവസ്ഥ 0b10:
        printf("W");
        നിർത്തുക;
    അവസ്ഥ 0b11:
        printf("RW");
        നിർത്തുക;
#അല്ലെങ്കിൽ
    അവസ്ഥ 0b00:
        നിർത്തുക;
    അവസ്ഥ 0b01:
        printf("RW");
    അവസ്ഥ 0b10:
        നിർത്തുക;
    അവസ്ഥ 0b11:
        printf("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }

    എങ്കിൽ (!NXN) {
        printf("X");
    }

    /* Only has effect if SMP enabled */
    എങ്കിൽ (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        printf(", uncached");
    }

    printf(")\n");
}

/* use when only have access to vptr of frames */
സ്ഥിതം ശൂന്യം _cap_frame_print_attrs_vptr(word_t vptr, vspace_വേര്_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    എങ്കിൽ (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_ഇടത് > PAGE_BITS)) {
        പ്രതിജ്ഞാ(0);
    }

    word_t table_index;
    തിരഞ്ഞെടുപ്പ് (ret.pt_Bits_ഇടത്) {

    അവസ്ഥ ARMHugePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
        നിർത്തുക;
    അവസ്ഥ ARMLargePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
        നിർത്തുക;
    അവസ്ഥ ARMSmallPage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3));
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        പ്രതിജ്ഞാ(0);

    }
    printf("frame_%p_%04lu ", ret.ptSlot, table_index);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

ശൂന്യം cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
സ്ഥിതം ശൂന്യം arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    വേണ്ടി (word_t i = 0; i < BIT(PT_INDEX_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        എങ്കിൽ (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            printf("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

സ്ഥിതം ശൂന്യം arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    printf("pd_%p_%04lu {\n", pudSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    വേണ്ടി (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        തിരഞ്ഞെടുപ്പ് (pte_ptr_get_pte_type(pdSlot)) {

        അവസ്ഥ pte_pte_page:
            printf("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            നിർത്തുക;

        അവസ്ഥ pte_pte_table:
            printf("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            നിർത്തുക;
        }
    }

    printf("}\n"); /* pd */

    വേണ്ടി (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;
        എങ്കിൽ (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

സ്ഥിതം ശൂന്യം arm64_cap_pud_print_slots(ശൂന്യം *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t index_bits = seL4_VSpaceIndexBits;
    printf("%p_pd {\n", pgdSlot_or_vspace);
#അല്ലെങ്കിൽ
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t index_bits = seL4_PageTableIndexBits;
    printf("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    വേണ്ടി (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;
        എങ്കിൽ (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            printf("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    printf("}\n"); /* pgd/pud */

    വേണ്ടി (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_INDEX(i, ULVL_FRM_ARM_PT_LVL(1));
        എങ്കിൽ (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

ശൂന്യം obj_vtable_print_slots(tcb_t *tcb)
{
    എങ്കിൽ (is_VTable_വേര്(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_വേര്_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#അല്ലെങ്കിൽ
        printf("%p_pd {\n", vspace);
        വേണ്ടി (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            എങ്കിൽ (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        printf("}\n"); /* pd */

        വേണ്ടി (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            എങ്കിൽ (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

ശൂന്യം print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ശൂന്യം print_cap_arch(cap_t cap)
{

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = ശൂന്യ;
        pte_t *pt = (pte_t *)find_ret.vspace_വേര്;
        word_t level;
        വേണ്ടി (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_INDEX(vptr, level);
            എങ്കിൽ (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                നിർത്തുക;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        എങ്കിൽ (pt != target_pt) {
            /* didn't find it */
            നിർത്തുക;
        }


        എങ്കിൽ (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_INDEX(vptr, level), (long unsigned പൂർണ്ണസംഖ്യ)asid);
        } അല്ലെങ്കിൽ {
            printf("pt_%p_%04lu\n", target_pt, GET_UPT_INDEX(vptr, level));
        }
        നിർത്തുക;
    }
    അവസ്ഥ cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        എങ്കിൽ (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_വേര്, (long unsigned പൂർണ്ണസംഖ്യ)asid);
        } അല്ലെങ്കിൽ {
            printf("%p_pd\n", find_ret.vspace_വേര്);
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
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_വേര്);
        നിർത്തുക;
    }
    അവസ്ഥ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ശൂന്യം *)cap_asid_pool_cap_get_capASIDPool(cap));
        നിർത്തുക;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap: {
        printf("%p_vcpu\n", (ശൂന്യം *)cap_vcpu_cap_get_capVCPUPtr(cap));
        നിർത്തുക;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ cap_io_space_cap: {
        printf("%p_io_space\n", (ശൂന്യം *)cap_io_space_cap_get_capModuleID(cap));
        നിർത്തുക;
    }
#endif
    സ്ഥിരസ്ഥിതി: {
        printf("[unknown cap %lu]\n", (long unsigned പൂർണ്ണസംഖ്യ)cap_get_capType(cap));
        നിർത്തുക;
    }
    }
}

ശൂന്യം print_object_arch(cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_frame_cap:
    അവസ്ഥ cap_page_table_cap:
    അവസ്ഥ cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        നിർത്തുക;

    അവസ്ഥ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ശൂന്യം *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        നിർത്തുക;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (ശൂന്യം *)cap_vcpu_cap_get_capVCPUPtr(cap));
        നിർത്തുക;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ cap_io_space_cap: {
        printf("%p_io_space = io_space ", (ശൂന്യം *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        നിർത്തുക;
    }
#endif
    സ്ഥിരസ്ഥിതി: {
        printf("[unknown object %lu]\n", (long unsigned പൂർണ്ണസംഖ്യ)cap_get_capType(cap));
        നിർത്തുക;
    }
    }
}

ശൂന്യം obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase)
{
    printf("(");

    /* VM size */
    തിരഞ്ഞെടുപ്പ് (frameSize) {
    അവസ്ഥ ARMHugePage:
        printf("1G");
        നിർത്തുക;
    അവസ്ഥ ARMLargePage:
        printf("2M");
        നിർത്തുക;
    അവസ്ഥ ARMSmallPage:
        printf("4k");
        നിർത്തുക;
    }

    printf(", paddr: 0x%p)\n", (ശൂന്യം *)frameBase);
}

ശൂന്യം arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    വേണ്ടി (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *ptSlot = pt + i;

        എങ്കിൽ (pte_4k_page_ptr_get_present(ptSlot)) {
            printf("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

ശൂന്യം arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    വേണ്ടി (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        എങ്കിൽ (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            printf("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        എങ്കിൽ (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            printf("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

ശൂന്യം arm64_obj_pud_print_slots(ശൂന്യം *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t index_bits = seL4_VSpaceIndexBits;
#അല്ലെങ്കിൽ
    word_t index_bits = seL4_PageTableIndexBits;
#endif
    വേണ്ടി (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;

        തിരഞ്ഞെടുപ്പ് (pte_ptr_get_pte_type(pudSlot)) {
        അവസ്ഥ pte_pte_page:
            printf("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            നിർത്തുക;

        അവസ്ഥ pte_pte_table: {
            printf("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

ശൂന്യം obj_tcb_print_vtable(tcb_t *tcb)
{
    എങ്കിൽ (is_VTable_വേര്(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_വേര്_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        printf("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#അല്ലെങ്കിൽ
        printf("%p_pd = pgd\n", vspace);
        വേണ്ടി (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            എങ്കിൽ (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

ശൂന്യം debug_capDL(ശൂന്യം)
{
    printf("arch aarch64\n");
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
