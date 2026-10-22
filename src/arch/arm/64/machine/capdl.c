/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#அந்தர்பாவ <arch/machine/capdl.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    திரும்பு tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

நிலையான வெறுமை obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase);
நிலையான வெறுமை cap_frame_print_attrs_pt(pte_t *ptSlot);
நிலையான வெறுமை cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
நிலையான வெறுமை cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

நிலையான வெறுமை _cap_frame_print_attrs_vptr(word_t vptr, vspace_வேர்_t *vspace_வேர்_2);

நிலையான வெறுமை arm64_obj_pt_print_slots(pte_t *pdSlot);
நிலையான வெறுமை arm64_obj_pd_print_slots(pte_t *pudSlot);
நிலையான வெறுமை arm64_obj_pud_print_slots(வெறுமை *pgdSlot_or_vspace);

நிலையான வெறுமை arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
நிலையான வெறுமை arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
நிலையான வெறுமை arm64_cap_pud_print_slots(வெறுமை *pgdSlot_or_vspace, vptr_t vptr);

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
நிலையான வெறுமை cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

நிலையான வெறுமை cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    printf("(");

    /* rights */
    தேர்வு (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை 0b00:
        நிறுத்து;
    நிலை 0b01:
        printf("R");
        நிறுத்து;
    நிலை 0b10:
        printf("W");
        நிறுத்து;
    நிலை 0b11:
        printf("RW");
        நிறுத்து;
#இல்லையெனில்
    நிலை 0b00:
        நிறுத்து;
    நிலை 0b01:
        printf("RW");
    நிலை 0b10:
        நிறுத்து;
    நிலை 0b11:
        printf("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    இயல்பு:
        நிறுத்து;
    }

    எனில் (!NXN) {
        printf("X");
    }

    /* Only has effect if SMP enabled */
    எனில் (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        printf(", uncached");
    }

    printf(")\n");
}

/* use when only have access to vptr of frames */
நிலையான வெறுமை _cap_frame_print_attrs_vptr(word_t vptr, vspace_வேர்_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    எனில் (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_இடது > PAGE_BITS)) {
        ப்ரதிஜ்ஞா(0);
    }

    word_t table_index;
    தேர்வு (ret.pt_Bits_இடது) {

    நிலை ARMHugePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
        நிறுத்து;
    நிலை ARMLargePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
        நிறுத்து;
    நிலை ARMSmallPage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3));
        நிறுத்து;
    இயல்பு:
        ப்ரதிஜ்ஞா(0);

    }
    printf("frame_%p_%04lu ", ret.ptSlot, table_index);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

வெறுமை cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
நிலையான வெறுமை arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    க்கு (word_t i = 0; i < BIT(PT_INDEX_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        எனில் (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            printf("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

நிலையான வெறுமை arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    printf("pd_%p_%04lu {\n", pudSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    க்கு (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        தேர்வு (pte_ptr_get_pte_type(pdSlot)) {

        நிலை pte_pte_page:
            printf("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            நிறுத்து;

        நிலை pte_pte_table:
            printf("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            நிறுத்து;
        }
    }

    printf("}\n"); /* pd */

    க்கு (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;
        எனில் (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

நிலையான வெறுமை arm64_cap_pud_print_slots(வெறுமை *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t index_bits = seL4_VSpaceIndexBits;
    printf("%p_pd {\n", pgdSlot_or_vspace);
#இல்லையெனில்
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t index_bits = seL4_PageTableIndexBits;
    printf("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    க்கு (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;
        எனில் (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            printf("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    printf("}\n"); /* pgd/pud */

    க்கு (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_INDEX(i, ULVL_FRM_ARM_PT_LVL(1));
        எனில் (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

வெறுமை obj_vtable_print_slots(tcb_t *tcb)
{
    எனில் (is_VTable_வேர்(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_வேர்_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#இல்லையெனில்
        printf("%p_pd {\n", vspace);
        க்கு (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            எனில் (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        printf("}\n"); /* pd */

        க்கு (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            எனில் (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

வெறுமை print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

வெறுமை print_cap_arch(cap_t cap)
{

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = வெற்று;
        pte_t *pt = (pte_t *)find_ret.vspace_வேர்;
        word_t level;
        க்கு (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_INDEX(vptr, level);
            எனில் (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                நிறுத்து;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        எனில் (pt != target_pt) {
            /* didn't find it */
            நிறுத்து;
        }


        எனில் (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_INDEX(vptr, level), (long unsigned முழு)asid);
        } இல்லையெனில் {
            printf("pt_%p_%04lu\n", target_pt, GET_UPT_INDEX(vptr, level));
        }
        நிறுத்து;
    }
    நிலை cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        எனில் (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_வேர், (long unsigned முழு)asid);
        } இல்லையெனில் {
            printf("%p_pd\n", find_ret.vspace_வேர்);
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
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_வேர்);
        நிறுத்து;
    }
    நிலை cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (வெறுமை *)cap_asid_pool_cap_get_capASIDPool(cap));
        நிறுத்து;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap: {
        printf("%p_vcpu\n", (வெறுமை *)cap_vcpu_cap_get_capVCPUPtr(cap));
        நிறுத்து;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    நிலை cap_io_space_cap: {
        printf("%p_io_space\n", (வெறுமை *)cap_io_space_cap_get_capModuleID(cap));
        நிறுத்து;
    }
#endif
    இயல்பு: {
        printf("[unknown cap %lu]\n", (long unsigned முழு)cap_get_capType(cap));
        நிறுத்து;
    }
    }
}

வெறுமை print_object_arch(cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_frame_cap:
    நிலை cap_page_table_cap:
    நிலை cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        நிறுத்து;

    நிலை cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (வெறுமை *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        நிறுத்து;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (வெறுமை *)cap_vcpu_cap_get_capVCPUPtr(cap));
        நிறுத்து;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    நிலை cap_io_space_cap: {
        printf("%p_io_space = io_space ", (வெறுமை *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        நிறுத்து;
    }
#endif
    இயல்பு: {
        printf("[unknown object %lu]\n", (long unsigned முழு)cap_get_capType(cap));
        நிறுத்து;
    }
    }
}

வெறுமை obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase)
{
    printf("(");

    /* VM size */
    தேர்வு (frameSize) {
    நிலை ARMHugePage:
        printf("1G");
        நிறுத்து;
    நிலை ARMLargePage:
        printf("2M");
        நிறுத்து;
    நிலை ARMSmallPage:
        printf("4k");
        நிறுத்து;
    }

    printf(", paddr: 0x%p)\n", (வெறுமை *)frameBase);
}

வெறுமை arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    க்கு (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *ptSlot = pt + i;

        எனில் (pte_4k_page_ptr_get_present(ptSlot)) {
            printf("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

வெறுமை arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    க்கு (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        எனில் (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            printf("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        எனில் (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            printf("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

வெறுமை arm64_obj_pud_print_slots(வெறுமை *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t index_bits = seL4_VSpaceIndexBits;
#இல்லையெனில்
    word_t index_bits = seL4_PageTableIndexBits;
#endif
    க்கு (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;

        தேர்வு (pte_ptr_get_pte_type(pudSlot)) {
        நிலை pte_pte_page:
            printf("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            நிறுத்து;

        நிலை pte_pte_table: {
            printf("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

வெறுமை obj_tcb_print_vtable(tcb_t *tcb)
{
    எனில் (is_VTable_வேர்(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_வேர்_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        printf("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#இல்லையெனில்
        printf("%p_pd = pgd\n", vspace);
        க்கு (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            எனில் (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

வெறுமை debug_capDL(வெறுமை)
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
