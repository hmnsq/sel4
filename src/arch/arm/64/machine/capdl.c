/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

ساكن فراغ obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase);
ساكن فراغ cap_frame_print_attrs_pt(pte_t *ptSlot);
ساكن فراغ cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
ساكن فراغ cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

ساكن فراغ _cap_frame_print_attrs_vptr(word_t vptr, vspace_root_t *vspaceRoot);

ساكن فراغ arm64_obj_pt_print_slots(pte_t *pdSlot);
ساكن فراغ arm64_obj_pd_print_slots(pte_t *pudSlot);
ساكن فراغ arm64_obj_pud_print_slots(فراغ *pgdSlot_or_vspace);

ساكن فراغ arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
ساكن فراغ arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgdSlot_or_vspace, vptr_t vptr);

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
ساكن فراغ cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

ساكن فراغ cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    printf("(");

    /* rights */
    بدل (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة 0b00:
        اكسر;
    حالة 0b01:
        printf("R");
        اكسر;
    حالة 0b10:
        printf("W");
        اكسر;
    حالة 0b11:
        printf("RW");
        اكسر;
#وإلا
    حالة 0b00:
        اكسر;
    حالة 0b01:
        printf("RW");
    حالة 0b10:
        اكسر;
    حالة 0b11:
        printf("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    افتراضي:
        اكسر;
    }

    𐤀𐤌 (!NXN) {
        printf("X");
    }

    /* Only has effect if SMP enabled */
    𐤀𐤌 (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        printf(", uncached");
    }

    printf(")\n");
}

/* use when only have access to vptr of frames */
ساكن فراغ _cap_frame_print_attrs_vptr(word_t vptr, vspace_root_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    𐤀𐤌 (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.ptBitsLeft > PAGE_BITS)) {
        أكد(0);
    }

    word_t table_index;
    بدل (ret.ptBitsLeft) {

    حالة ARMHugePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
        اكسر;
    حالة ARMLargePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
        اكسر;
    حالة ARMSmallPage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3));
        اكسر;
    افتراضي:
        أكد(0);

    }
    printf("frame_%p_%04lu ", ret.ptSlot, table_index);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

فراغ cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
ساكن فراغ arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    لكل (word_t i = 0; i < BIT(PT_INDEX_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        𐤀𐤌 (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            printf("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

ساكن فراغ arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    printf("pd_%p_%04lu {\n", pudSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    لكل (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        بدل (pte_ptr_get_pte_type(pdSlot)) {

        حالة pte_pte_page:
            printf("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            اكسر;

        حالة pte_pte_table:
            printf("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            اكسر;
        }
    }

    printf("}\n"); /* pd */

    لكل (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;
        𐤀𐤌 (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t index_bits = seL4_VSpaceIndexBits;
    printf("%p_pd {\n", pgdSlot_or_vspace);
#وإلا
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t index_bits = seL4_PageTableIndexBits;
    printf("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    لكل (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;
        𐤀𐤌 (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            printf("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    printf("}\n"); /* pgd/pud */

    لكل (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_INDEX(i, ULVL_FRM_ARM_PT_LVL(1));
        𐤀𐤌 (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    𐤀𐤌 (isVTableRoot(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_root_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#وإلا
        printf("%p_pd {\n", vspace);
        لكل (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            𐤀𐤌 (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        printf("}\n"); /* pd */

        لكل (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            𐤀𐤌 (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

فراغ print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

فراغ print_cap_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = عدم;
        pte_t *pt = (pte_t *)find_ret.vspace_root;
        word_t level;
        لكل (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_INDEX(vptr, level);
            𐤀𐤌 (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                اكسر;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        𐤀𐤌 (pt != target_pt) {
            /* didn't find it */
            اكسر;
        }


        𐤀𐤌 (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_INDEX(vptr, level), (long unsigned صحيح)asid);
        } وإلا {
            printf("pt_%p_%04lu\n", target_pt, GET_UPT_INDEX(vptr, level));
        }
        اكسر;
    }
    حالة cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        𐤀𐤌 (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_root, (long unsigned صحيح)asid);
        } وإلا {
            printf("%p_pd\n", find_ret.vspace_root);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        اكسر;
    }
    حالة cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));
        أكد(find_ret.status == EXCEPTION_NONE);
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_root);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        printf("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        printf("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#endif
    افتراضي: {
        printf("[unknown cap %lu]\n", (long unsigned صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_frame_cap:
    حالة cap_page_table_cap:
    حالة cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        printf("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#endif
    افتراضي: {
        printf("[unknown object %lu]\n", (long unsigned صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase)
{
    printf("(");

    /* VM size */
    بدل (frameSize) {
    حالة ARMHugePage:
        printf("1G");
        اكسر;
    حالة ARMLargePage:
        printf("2M");
        اكسر;
    حالة ARMSmallPage:
        printf("4k");
        اكسر;
    }

    printf(", paddr: 0x%p)\n", (فراغ *)frameBase);
}

فراغ arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    لكل (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *ptSlot = pt + i;

        𐤀𐤌 (pte_4k_page_ptr_get_present(ptSlot)) {
            printf("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

فراغ arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    لكل (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        𐤀𐤌 (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            printf("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        𐤀𐤌 (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            printf("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

فراغ arm64_obj_pud_print_slots(فراغ *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t index_bits = seL4_VSpaceIndexBits;
#وإلا
    word_t index_bits = seL4_PageTableIndexBits;
#endif
    لكل (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;

        بدل (pte_ptr_get_pte_type(pudSlot)) {
        حالة pte_pte_page:
            printf("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            اكسر;

        حالة pte_pte_table: {
            printf("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    𐤀𐤌 (isVTableRoot(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_root_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        printf("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#وإلا
        printf("%p_pd = pgd\n", vspace);
        لكل (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            𐤀𐤌 (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
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
