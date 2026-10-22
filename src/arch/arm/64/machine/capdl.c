/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ଅନ୍ତର୍ଭାଵ <arch/machine/capdl.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    ଫେରନ୍ତୁ tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

ସ୍ଥିର ଶୂନ୍ୟ obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase);
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_pt(pte_t *ptSlot);
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

ସ୍ଥିର ଶୂନ୍ୟ _cap_frame_print_attrs_vptr(word_t vptr, vspace_ମୂଳ_t *vspace_ମୂଳ_2);

ସ୍ଥିର ଶୂନ୍ୟ arm64_obj_pt_print_slots(pte_t *pdSlot);
ସ୍ଥିର ଶୂନ୍ୟ arm64_obj_pd_print_slots(pte_t *pudSlot);
ସ୍ଥିର ଶୂନ୍ୟ arm64_obj_pud_print_slots(ଶୂନ୍ୟ *pgdSlot_or_vspace);

ସ୍ଥିର ଶୂନ୍ୟ arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
ସ୍ଥିର ଶୂନ୍ୟ arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
ସ୍ଥିର ଶୂନ୍ୟ arm64_cap_pud_print_slots(ଶୂନ୍ୟ *pgdSlot_or_vspace, vptr_t vptr);

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
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    printf("(");

    /* rights */
    ବାଛ (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି 0b00:
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 0b01:
        printf("R");
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 0b10:
        printf("W");
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 0b11:
        printf("RW");
        ଭାଙ୍ଗ;
#ଅନ୍ୟଥା
    ସ୍ଥିତି 0b00:
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 0b01:
        printf("RW");
    ସ୍ଥିତି 0b10:
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 0b11:
        printf("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    ଯଦି (!NXN) {
        printf("X");
    }

    /* Only has effect if SMP enabled */
    ଯଦି (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        printf(", uncached");
    }

    printf(")\n");
}

/* use when only have access to vptr of frames */
ସ୍ଥିର ଶୂନ୍ୟ _cap_frame_print_attrs_vptr(word_t vptr, vspace_ମୂଳ_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    ଯଦି (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_ବାମ > PAGE_BITS)) {
        ପ୍ରତିଜ୍ଞା(0);
    }

    word_t table_index;
    ବାଛ (ret.pt_Bits_ବାମ) {

    ସ୍ଥିତି ARMHugePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ARMLargePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ARMSmallPage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3));
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ପ୍ରତିଜ୍ଞା(0);

    }
    printf("frame_%p_%04lu ", ret.ptSlot, table_index);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

ଶୂନ୍ୟ cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
ସ୍ଥିର ଶୂନ୍ୟ arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    ପାଇଁ (word_t i = 0; i < BIT(PT_INDEX_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        ଯଦି (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            printf("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

ସ୍ଥିର ଶୂନ୍ୟ arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    printf("pd_%p_%04lu {\n", pudSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    ପାଇଁ (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        ବାଛ (pte_ptr_get_pte_type(pdSlot)) {

        ସ୍ଥିତି pte_pte_page:
            printf("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି pte_pte_table:
            printf("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            ଭାଙ୍ଗ;
        }
    }

    printf("}\n"); /* pd */

    ପାଇଁ (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;
        ଯଦି (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ arm64_cap_pud_print_slots(ଶୂନ୍ୟ *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t index_bits = seL4_VSpaceIndexBits;
    printf("%p_pd {\n", pgdSlot_or_vspace);
#ଅନ୍ୟଥା
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t index_bits = seL4_PageTableIndexBits;
    printf("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    ପାଇଁ (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;
        ଯଦି (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            printf("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    printf("}\n"); /* pgd/pud */

    ପାଇଁ (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_INDEX(i, ULVL_FRM_ARM_PT_LVL(1));
        ଯଦି (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

ଶୂନ୍ୟ obj_vtable_print_slots(tcb_t *tcb)
{
    ଯଦି (is_VTable_ମୂଳ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ମୂଳ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#ଅନ୍ୟଥା
        printf("%p_pd {\n", vspace);
        ପାଇଁ (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            ଯଦି (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        printf("}\n"); /* pd */

        ପାଇଁ (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            ଯଦି (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

ଶୂନ୍ୟ print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ଶୂନ୍ୟ print_cap_arch(cap_t cap)
{

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = ଖାଲି;
        pte_t *pt = (pte_t *)find_ret.vspace_ମୂଳ;
        word_t level;
        ପାଇଁ (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_INDEX(vptr, level);
            ଯଦି (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                ଭାଙ୍ଗ;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        ଯଦି (pt != target_pt) {
            /* didn't find it */
            ଭାଙ୍ଗ;
        }


        ଯଦି (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_INDEX(vptr, level), (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)asid);
        } ଅନ୍ୟଥା {
            printf("pt_%p_%04lu\n", target_pt, GET_UPT_INDEX(vptr, level));
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        ଯଦି (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_ମୂଳ, (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)asid);
        } ଅନ୍ୟଥା {
            printf("%p_pd\n", find_ret.vspace_ମୂଳ);
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
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ମୂଳ);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ଶୂନ୍ୟ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ଭାଙ୍ଗ;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap: {
        printf("%p_vcpu\n", (ଶୂନ୍ୟ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ଭାଙ୍ଗ;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି cap_io_space_cap: {
        printf("%p_io_space\n", (ଶୂନ୍ୟ *)cap_io_space_cap_get_capModuleID(cap));
        ଭାଙ୍ଗ;
    }
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
        printf("[unknown cap %lu]\n", (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_get_capType(cap));
        ଭାଙ୍ଗ;
    }
    }
}

ଶୂନ୍ୟ print_object_arch(cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_frame_cap:
    ସ୍ଥିତି cap_page_table_cap:
    ସ୍ଥିତି cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ଶୂନ୍ୟ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (ଶୂନ୍ୟ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ଭାଙ୍ଗ;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି cap_io_space_cap: {
        printf("%p_io_space = io_space ", (ଶୂନ୍ୟ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
        printf("[unknown object %lu]\n", (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_get_capType(cap));
        ଭାଙ୍ଗ;
    }
    }
}

ଶୂନ୍ୟ obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase)
{
    printf("(");

    /* VM size */
    ବାଛ (frameSize) {
    ସ୍ଥିତି ARMHugePage:
        printf("1G");
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ARMLargePage:
        printf("2M");
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ARMSmallPage:
        printf("4k");
        ଭାଙ୍ଗ;
    }

    printf(", paddr: 0x%p)\n", (ଶୂନ୍ୟ *)frameBase);
}

ଶୂନ୍ୟ arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    ପାଇଁ (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *ptSlot = pt + i;

        ଯଦି (pte_4k_page_ptr_get_present(ptSlot)) {
            printf("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

ଶୂନ୍ୟ arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    ପାଇଁ (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        ଯଦି (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            printf("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        ଯଦି (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            printf("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

ଶୂନ୍ୟ arm64_obj_pud_print_slots(ଶୂନ୍ୟ *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t index_bits = seL4_VSpaceIndexBits;
#ଅନ୍ୟଥା
    word_t index_bits = seL4_PageTableIndexBits;
#endif
    ପାଇଁ (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;

        ବାଛ (pte_ptr_get_pte_type(pudSlot)) {
        ସ୍ଥିତି pte_pte_page:
            printf("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            ଭାଙ୍ଗ;

        ସ୍ଥିତି pte_pte_table: {
            printf("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

ଶୂନ୍ୟ obj_tcb_print_vtable(tcb_t *tcb)
{
    ଯଦି (is_VTable_ମୂଳ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ମୂଳ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        printf("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#ଅନ୍ୟଥା
        printf("%p_pd = pgd\n", vspace);
        ପାଇଁ (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            ଯଦି (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

ଶୂନ୍ୟ debug_capDL(ଶୂନ୍ୟ)
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
