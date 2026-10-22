/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_DEBUG_BUILD

#অন্তর্ভাব <arch/machine/capdl.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    ফেরত tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

স্থির শূন্য obj_frame_print_attrs(vm_page_আকার_t frame_আকার, paddr_t frameBase);
স্থির শূন্য cap_frame_print_attrs_pt(pte_t *ptSlot);
স্থির শূন্য cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
স্থির শূন্য cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

স্থির শূন্য _cap_frame_print_attrs_vptr(word_t vptr, vspace_মূল_t *vspace_মূল_2);

স্থির শূন্য arm64_obj_pt_print_slots(pte_t *pdSlot);
স্থির শূন্য arm64_obj_pd_print_slots(pte_t *pudSlot);
স্থির শূন্য arm64_obj_pud_print_slots(শূন্য *pgdSlot_or_vspace);

স্থির শূন্য arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
স্থির শূন্য arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
স্থির শূন্য arm64_cap_pud_print_slots(শূন্য *pgdSlot_or_vspace, vptr_t vptr);

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
স্থির শূন্য cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

স্থির শূন্য cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    printf("(");

    /* rights */
    নির্বাচন (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র 0b00:
        বিরতি;
    ক্ষেত্র 0b01:
        printf("R");
        বিরতি;
    ক্ষেত্র 0b10:
        printf("W");
        বিরতি;
    ক্ষেত্র 0b11:
        printf("RW");
        বিরতি;
#অন্যথা
    ক্ষেত্র 0b00:
        বিরতি;
    ক্ষেত্র 0b01:
        printf("RW");
    ক্ষেত্র 0b10:
        বিরতি;
    ক্ষেত্র 0b11:
        printf("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    পূর্বনির্ধারিত:
        বিরতি;
    }

    যদি (!NXN) {
        printf("X");
    }

    /* Only has effect if SMP enabled */
    যদি (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        printf(", uncached");
    }

    printf(")\n");
}

/* use when only have access to vptr of frames */
স্থির শূন্য _cap_frame_print_attrs_vptr(word_t vptr, vspace_মূল_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    যদি (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_বাম > PAGE_BITS)) {
        প্রতিজ্ঞা(0);
    }

    word_t table_সূচক;
    নির্বাচন (ret.pt_Bits_বাম) {

    ক্ষেত্র ARMHugePage:
        table_সূচক = GET_UPT_সূচক(vptr, ULVL_FRM_ARM_PT_LVL(1));
        বিরতি;
    ক্ষেত্র ARMLargePage:
        table_সূচক = GET_UPT_সূচক(vptr, ULVL_FRM_ARM_PT_LVL(2));
        বিরতি;
    ক্ষেত্র ARMSmallPage:
        table_সূচক = GET_UPT_সূচক(vptr, ULVL_FRM_ARM_PT_LVL(3));
        বিরতি;
    পূর্বনির্ধারিত:
        প্রতিজ্ঞা(0);

    }
    printf("frame_%p_%04lu ", ret.ptSlot, table_সূচক);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

শূন্য cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
স্থির শূন্য arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_UPT_সূচক(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    জন্য (word_t i = 0; i < BIT(PT_সূচক_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        যদি (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            printf("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

স্থির শূন্য arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    printf("pd_%p_%04lu {\n", pudSlot, GET_UPT_সূচক(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    জন্য (word_t i = 0; i < BIT(PT_সূচক_BITS); i++) {
        pte_t *pdSlot = pd + i;

        নির্বাচন (pte_ptr_get_pte_type(pdSlot)) {

        ক্ষেত্র pte_pte_page:
            printf("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            বিরতি;

        ক্ষেত্র pte_pte_table:
            printf("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            বিরতি;
        }
    }

    printf("}\n"); /* pd */

    জন্য (word_t i = 0; i < BIT(PT_সূচক_BITS); i++) {
        pte_t *pdSlot = pd + i;
        যদি (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

স্থির শূন্য arm64_cap_pud_print_slots(শূন্য *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t সূচক_bits = se_L4_VSpace_সূচক_Bits;
    printf("%p_pd {\n", pgdSlot_or_vspace);
#অন্যথা
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t সূচক_bits = se_L4_Page_Table_সূচক_Bits;
    printf("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_সূচক(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    জন্য (word_t i = 0; i < BIT(সূচক_bits); i++) {
        pte_t *pudSlot = pud + i;
        যদি (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            printf("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    printf("}\n"); /* pgd/pud */

    জন্য (word_t i = 0; i < BIT(সূচক_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_সূচক(i, ULVL_FRM_ARM_PT_LVL(1));
        যদি (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

শূন্য obj_vtable_print_slots(tcb_t *tcb)
{
    যদি (is_VTable_মূল(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_মূল_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#অন্যথা
        printf("%p_pd {\n", vspace);
        জন্য (word_t i = 0; i < PT_সূচক_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            যদি (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        printf("}\n"); /* pd */

        জন্য (word_t i = 0; i < PT_সূচক_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            যদি (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

শূন্য print_ipc_বাফার_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

শূন্য print_cap_arch(cap_t cap)
{

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = ফাঁকা;
        pte_t *pt = (pte_t *)find_ret.vspace_মূল;
        word_t level;
        জন্য (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_সূচক(vptr, level);
            যদি (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                বিরতি;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        যদি (pt != target_pt) {
            /* didn't find it */
            বিরতি;
        }


        যদি (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_সূচক(vptr, level), (long unsigned পূর্ণসংখ্যা)asid);
        } অন্যথা {
            printf("pt_%p_%04lu\n", target_pt, GET_UPT_সূচক(vptr, level));
        }
        বিরতি;
    }
    ক্ষেত্র cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        যদি (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_মূল, (long unsigned পূর্ণসংখ্যা)asid);
        } অন্যথা {
            printf("%p_pd\n", find_ret.vspace_মূল);
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
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_মূল);
        বিরতি;
    }
    ক্ষেত্র cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (শূন্য *)cap_asid_pool_cap_get_capASIDPool(cap));
        বিরতি;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap: {
        printf("%p_vcpu\n", (শূন্য *)cap_vcpu_cap_get_capVCPUPtr(cap));
        বিরতি;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র cap_io_space_cap: {
        printf("%p_io_space\n", (শূন্য *)cap_io_space_cap_get_capModuleID(cap));
        বিরতি;
    }
#endif
    পূর্বনির্ধারিত: {
        printf("[unknown cap %lu]\n", (long unsigned পূর্ণসংখ্যা)cap_get_capType(cap));
        বিরতি;
    }
    }
}

শূন্য print_object_arch(cap_t cap)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_frame_cap:
    ক্ষেত্র cap_page_table_cap:
    ক্ষেত্র cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        বিরতি;

    ক্ষেত্র cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (শূন্য *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        বিরতি;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (শূন্য *)cap_vcpu_cap_get_capVCPUPtr(cap));
        বিরতি;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র cap_io_space_cap: {
        printf("%p_io_space = io_space ", (শূন্য *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        বিরতি;
    }
#endif
    পূর্বনির্ধারিত: {
        printf("[unknown object %lu]\n", (long unsigned পূর্ণসংখ্যা)cap_get_capType(cap));
        বিরতি;
    }
    }
}

শূন্য obj_frame_print_attrs(vm_page_আকার_t frame_আকার, paddr_t frameBase)
{
    printf("(");

    /* VM size */
    নির্বাচন (frame_আকার) {
    ক্ষেত্র ARMHugePage:
        printf("1G");
        বিরতি;
    ক্ষেত্র ARMLargePage:
        printf("2M");
        বিরতি;
    ক্ষেত্র ARMSmallPage:
        printf("4k");
        বিরতি;
    }

    printf(", paddr: 0x%p)\n", (শূন্য *)frameBase);
}

শূন্য arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    জন্য (word_t i = 0; i < BIT(PT_সূচক_BITS); i++) {
        pte_t *ptSlot = pt + i;

        যদি (pte_4k_page_ptr_get_present(ptSlot)) {
            printf("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

শূন্য arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    জন্য (word_t i = 0; i < BIT(PT_সূচক_BITS); i++) {
        pte_t *pdSlot = pd + i;

        যদি (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            printf("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        যদি (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            printf("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

শূন্য arm64_obj_pud_print_slots(শূন্য *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t সূচক_bits = se_L4_VSpace_সূচক_Bits;
#অন্যথা
    word_t সূচক_bits = se_L4_Page_Table_সূচক_Bits;
#endif
    জন্য (word_t i = 0; i < BIT(সূচক_bits); i++) {
        pte_t *pudSlot = pud + i;

        নির্বাচন (pte_ptr_get_pte_type(pudSlot)) {
        ক্ষেত্র pte_pte_page:
            printf("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            বিরতি;

        ক্ষেত্র pte_pte_table: {
            printf("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

শূন্য obj_tcb_print_vtable(tcb_t *tcb)
{
    যদি (is_VTable_মূল(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_মূল_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        printf("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#অন্যথা
        printf("%p_pd = pgd\n", vspace);
        জন্য (word_t i = 0; i < PT_সূচক_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            যদি (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

শূন্য debug_capDL(শূন্য)
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
