/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    ಪ್ರತ್ಯಾಗಮ tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

ಸ್ಥಿರ ಶೂನ್ಯ obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase);
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_pt(pte_t *ptSlot);
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

ಸ್ಥಿರ ಶೂನ್ಯ _cap_frame_print_attrs_vptr(word_t vptr, vspace_ಬೇರು_t *vspace_ಬೇರು_2);

ಸ್ಥಿರ ಶೂನ್ಯ arm64_obj_pt_print_slots(pte_t *pdSlot);
ಸ್ಥಿರ ಶೂನ್ಯ arm64_obj_pd_print_slots(pte_t *pudSlot);
ಸ್ಥಿರ ಶೂನ್ಯ arm64_obj_pud_print_slots(ಶೂನ್ಯ *pgdSlot_or_vspace);

ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pud_print_slots(ಶೂನ್ಯ *pgdSlot_or_vspace, vptr_t vptr);

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
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    printf("(");

    /* rights */
    ವಿಕಲ್ಪ (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ 0b00:
        ಭಙ್ಗ;
    ಪಕ್ಷ 0b01:
        printf("R");
        ಭಙ್ಗ;
    ಪಕ್ಷ 0b10:
        printf("W");
        ಭಙ್ಗ;
    ಪಕ್ಷ 0b11:
        printf("RW");
        ಭಙ್ಗ;
#ಅನ್ಯಥಾ
    ಪಕ್ಷ 0b00:
        ಭಙ್ಗ;
    ಪಕ್ಷ 0b01:
        printf("RW");
    ಪಕ್ಷ 0b10:
        ಭಙ್ಗ;
    ಪಕ್ಷ 0b11:
        printf("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    ಯದಿ (!NXN) {
        printf("X");
    }

    /* Only has effect if SMP enabled */
    ಯದಿ (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        printf(", uncached");
    }

    printf(")\n");
}

/* use when only have access to vptr of frames */
ಸ್ಥಿರ ಶೂನ್ಯ _cap_frame_print_attrs_vptr(word_t vptr, vspace_ಬೇರು_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    ಯದಿ (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_ಎಡ > PAGE_BITS)) {
        ಪ್ರತಿಜ್ಞಾ(0);
    }

    word_t table_index;
    ವಿಕಲ್ಪ (ret.pt_Bits_ಎಡ) {

    ಪಕ್ಷ ARMHugePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMLargePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMSmallPage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3));
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        ಪ್ರತಿಜ್ಞಾ(0);

    }
    printf("frame_%p_%04lu ", ret.ptSlot, table_index);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

ಶೂನ್ಯ cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    ಪ್ರತಿ (word_t i = 0; i < BIT(PT_INDEX_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        ಯದಿ (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            printf("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    printf("pd_%p_%04lu {\n", pudSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    ಪ್ರತಿ (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        ವಿಕಲ್ಪ (pte_ptr_get_pte_type(pdSlot)) {

        ಪಕ್ಷ pte_pte_page:
            printf("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            ಭಙ್ಗ;

        ಪಕ್ಷ pte_pte_table:
            printf("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            ಭಙ್ಗ;
        }
    }

    printf("}\n"); /* pd */

    ಪ್ರತಿ (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;
        ಯದಿ (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ arm64_cap_pud_print_slots(ಶೂನ್ಯ *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t index_bits = seL4_VSpaceIndexBits;
    printf("%p_pd {\n", pgdSlot_or_vspace);
#ಅನ್ಯಥಾ
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t index_bits = seL4_PageTableIndexBits;
    printf("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    ಪ್ರತಿ (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;
        ಯದಿ (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            printf("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    printf("}\n"); /* pgd/pud */

    ಪ್ರತಿ (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_INDEX(i, ULVL_FRM_ARM_PT_LVL(1));
        ಯದಿ (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

ಶೂನ್ಯ obj_vtable_print_slots(tcb_t *tcb)
{
    ಯದಿ (is_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ಬೇರು_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#ಅನ್ಯಥಾ
        printf("%p_pd {\n", vspace);
        ಪ್ರತಿ (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            ಯದಿ (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        printf("}\n"); /* pd */

        ಪ್ರತಿ (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            ಯದಿ (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

ಶೂನ್ಯ print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ಶೂನ್ಯ print_cap_arch(cap_t cap)
{

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = ರಿಕ್ತ;
        pte_t *pt = (pte_t *)find_ret.vspace_ಬೇರು;
        word_t level;
        ಪ್ರತಿ (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_INDEX(vptr, level);
            ಯದಿ (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                ಭಙ್ಗ;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        ಯದಿ (pt != target_pt) {
            /* didn't find it */
            ಭಙ್ಗ;
        }


        ಯದಿ (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_INDEX(vptr, level), (long unsigned ಪೂರ್ಣಾಙ್ಕ)asid);
        } ಅನ್ಯಥಾ {
            printf("pt_%p_%04lu\n", target_pt, GET_UPT_INDEX(vptr, level));
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        ಯದಿ (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_ಬೇರು, (long unsigned ಪೂರ್ಣಾಙ್ಕ)asid);
        } ಅನ್ಯಥಾ {
            printf("%p_pd\n", find_ret.vspace_ಬೇರು);
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
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_ಬೇರು);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ಭಙ್ಗ;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap: {
        printf("%p_vcpu\n", (ಶೂನ್ಯ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ಭಙ್ಗ;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ cap_io_space_cap: {
        printf("%p_io_space\n", (ಶೂನ್ಯ *)cap_io_space_cap_get_capModuleID(cap));
        ಭಙ್ಗ;
    }
#endif
    ಸ್ವಭಾವ: {
        printf("[unknown cap %lu]\n", (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_get_capType(cap));
        ಭಙ್ಗ;
    }
    }
}

ಶೂನ್ಯ print_object_arch(cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_frame_cap:
    ಪಕ್ಷ cap_page_table_cap:
    ಪಕ್ಷ cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ಭಙ್ಗ;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (ಶೂನ್ಯ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ಭಙ್ಗ;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ cap_io_space_cap: {
        printf("%p_io_space = io_space ", (ಶೂನ್ಯ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        ಭಙ್ಗ;
    }
#endif
    ಸ್ವಭಾವ: {
        printf("[unknown object %lu]\n", (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_get_capType(cap));
        ಭಙ್ಗ;
    }
    }
}

ಶೂನ್ಯ obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase)
{
    printf("(");

    /* VM size */
    ವಿಕಲ್ಪ (frameSize) {
    ಪಕ್ಷ ARMHugePage:
        printf("1G");
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMLargePage:
        printf("2M");
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMSmallPage:
        printf("4k");
        ಭಙ್ಗ;
    }

    printf(", paddr: 0x%p)\n", (ಶೂನ್ಯ *)frameBase);
}

ಶೂನ್ಯ arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    ಪ್ರತಿ (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *ptSlot = pt + i;

        ಯದಿ (pte_4k_page_ptr_get_present(ptSlot)) {
            printf("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

ಶೂನ್ಯ arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    ಪ್ರತಿ (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        ಯದಿ (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            printf("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        ಯದಿ (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            printf("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

ಶೂನ್ಯ arm64_obj_pud_print_slots(ಶೂನ್ಯ *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t index_bits = seL4_VSpaceIndexBits;
#ಅನ್ಯಥಾ
    word_t index_bits = seL4_PageTableIndexBits;
#endif
    ಪ್ರತಿ (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;

        ವಿಕಲ್ಪ (pte_ptr_get_pte_type(pudSlot)) {
        ಪಕ್ಷ pte_pte_page:
            printf("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            ಭಙ್ಗ;

        ಪಕ್ಷ pte_pte_table: {
            printf("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

ಶೂನ್ಯ obj_tcb_print_vtable(tcb_t *tcb)
{
    ಯದಿ (is_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ಬೇರು_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        printf("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#ಅನ್ಯಥಾ
        printf("%p_pd = pgd\n", vspace);
        ಪ್ರತಿ (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            ಯದಿ (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

ಶೂನ್ಯ debug_capDL(ಶೂನ್ಯ)
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
