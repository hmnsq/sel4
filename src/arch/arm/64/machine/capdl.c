/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#ifdef CONFIG_DEBUG_BUILD

#شامل <arch/machine/capdl.h>
#شامل <string.h>
#شامل <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    بازگشت tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

ایستا تهی obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase);
ایستا تهی cap_frame_print_attrs_pt(pte_t *ptSlot);
ایستا تهی cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
ایستا تهی cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

ایستا تهی _cap_frame_print_attrs_vptr(word_t vptr, vspace_ریشه_t *vspace_ریشه_2);

ایستا تهی arm64_obj_pt_print_slots(pte_t *pdSlot);
ایستا تهی arm64_obj_pd_print_slots(pte_t *pudSlot);
ایستا تهی arm64_obj_pud_print_slots(تهی *pgdSlot_or_vspace);

ایستا تهی arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
ایستا تهی arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
ایستا تهی arm64_cap_pud_print_slots(تهی *pgdSlot_or_vspace, vptr_t vptr);

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
ایستا تهی cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

ایستا تهی cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    printf("(");

    /* rights */
    انتخاب (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت 0b00:
        بشکن;
    حالت 0b01:
        printf("R");
        بشکن;
    حالت 0b10:
        printf("W");
        بشکن;
    حالت 0b11:
        printf("RW");
        بشکن;
#وگرنه
    حالت 0b00:
        بشکن;
    حالت 0b01:
        printf("RW");
    حالت 0b10:
        بشکن;
    حالت 0b11:
        printf("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    پیشفرض:
        بشکن;
    }

    اگر (!NXN) {
        printf("X");
    }

    /* Only has effect if SMP enabled */
    اگر (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        printf(", uncached");
    }

    printf(")\n");
}

/* use when only have access to vptr of frames */
ایستا تهی _cap_frame_print_attrs_vptr(word_t vptr, vspace_ریشه_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    اگر (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_چپ > PAGE_BITS)) {
        أكد(0);
    }

    word_t table_index;
    انتخاب (ret.pt_Bits_چپ) {

    حالت ARMHugePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
        بشکن;
    حالت ARMLargePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
        بشکن;
    حالت ARMSmallPage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3));
        بشکن;
    پیشفرض:
        أكد(0);

    }
    printf("frame_%p_%04lu ", ret.ptSlot, table_index);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

تهی cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
ایستا تهی arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    برای (word_t i = 0; i < BIT(PT_INDEX_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        اگر (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            printf("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

ایستا تهی arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    printf("pd_%p_%04lu {\n", pudSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    برای (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        انتخاب (pte_ptr_get_pte_type(pdSlot)) {

        حالت pte_pte_page:
            printf("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            بشکن;

        حالت pte_pte_table:
            printf("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            بشکن;
        }
    }

    printf("}\n"); /* pd */

    برای (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;
        اگر (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ایستا تهی arm64_cap_pud_print_slots(تهی *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t index_bits = seL4_VSpaceIndexBits;
    printf("%p_pd {\n", pgdSlot_or_vspace);
#وگرنه
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t index_bits = seL4_PageTableIndexBits;
    printf("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    برای (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;
        اگر (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            printf("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    printf("}\n"); /* pgd/pud */

    برای (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_INDEX(i, ULVL_FRM_ARM_PT_LVL(1));
        اگر (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

تهی obj_vtable_print_slots(tcb_t *tcb)
{
    اگر (is_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ریشه_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#وگرنه
        printf("%p_pd {\n", vspace);
        برای (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            اگر (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        printf("}\n"); /* pd */

        برای (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            اگر (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

تهی print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

تهی print_cap_arch(cap_t cap)
{

    انتخاب (cap_get_capType(cap)) {
    حالت cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = پوچ;
        pte_t *pt = (pte_t *)find_ret.vspace_ریشه;
        word_t level;
        برای (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_INDEX(vptr, level);
            اگر (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                بشکن;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        اگر (pt != target_pt) {
            /* didn't find it */
            بشکن;
        }


        اگر (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_INDEX(vptr, level), (long unsigned صحیح)asid);
        } وگرنه {
            printf("pt_%p_%04lu\n", target_pt, GET_UPT_INDEX(vptr, level));
        }
        بشکن;
    }
    حالت cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        اگر (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_ریشه, (long unsigned صحیح)asid);
        } وگرنه {
            printf("%p_pd\n", find_ret.vspace_ریشه);
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
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ریشه);
        بشکن;
    }
    حالت cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (تهی *)cap_asid_pool_cap_get_capASIDPool(cap));
        بشکن;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap: {
        printf("%p_vcpu\n", (تهی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        بشکن;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    حالت cap_io_space_cap: {
        printf("%p_io_space\n", (تهی *)cap_io_space_cap_get_capModuleID(cap));
        بشکن;
    }
#endif
    پیشفرض: {
        printf("[unknown cap %lu]\n", (long unsigned صحیح)cap_get_capType(cap));
        بشکن;
    }
    }
}

تهی print_object_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_frame_cap:
    حالت cap_page_table_cap:
    حالت cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        بشکن;

    حالت cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (تهی *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        بشکن;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (تهی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        بشکن;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    حالت cap_io_space_cap: {
        printf("%p_io_space = io_space ", (تهی *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        بشکن;
    }
#endif
    پیشفرض: {
        printf("[unknown object %lu]\n", (long unsigned صحیح)cap_get_capType(cap));
        بشکن;
    }
    }
}

تهی obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase)
{
    printf("(");

    /* VM size */
    انتخاب (frameSize) {
    حالت ARMHugePage:
        printf("1G");
        بشکن;
    حالت ARMLargePage:
        printf("2M");
        بشکن;
    حالت ARMSmallPage:
        printf("4k");
        بشکن;
    }

    printf(", paddr: 0x%p)\n", (تهی *)frameBase);
}

تهی arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    برای (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *ptSlot = pt + i;

        اگر (pte_4k_page_ptr_get_present(ptSlot)) {
            printf("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

تهی arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    برای (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        اگر (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            printf("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        اگر (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            printf("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

تهی arm64_obj_pud_print_slots(تهی *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t index_bits = seL4_VSpaceIndexBits;
#وگرنه
    word_t index_bits = seL4_PageTableIndexBits;
#endif
    برای (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;

        انتخاب (pte_ptr_get_pte_type(pudSlot)) {
        حالت pte_pte_page:
            printf("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            بشکن;

        حالت pte_pte_table: {
            printf("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

تهی obj_tcb_print_vtable(tcb_t *tcb)
{
    اگر (is_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ریشه_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        printf("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#وگرنه
        printf("%p_pd = pgd\n", vspace);
        برای (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            اگر (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

تهی debug_capDL(تهی)
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
