/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#અન્તર્ભાવ <arch/machine/capdl.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    પાછા tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

સ્થિર શૂન્ય obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase);
સ્થિર શૂન્ય cap_frame_print_attrs_pt(pte_t *ptSlot);
સ્થિર શૂન્ય cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
સ્થિર શૂન્ય cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

સ્થિર શૂન્ય _cap_frame_print_attrs_vptr(word_t vptr, vspace_મૂળ_t *vspace_મૂળ_2);

સ્થિર શૂન્ય arm64_obj_pt_print_slots(pte_t *pdSlot);
સ્થિર શૂન્ય arm64_obj_pd_print_slots(pte_t *pudSlot);
સ્થિર શૂન્ય arm64_obj_pud_print_slots(શૂન્ય *pgdSlot_or_vspace);

સ્થિર શૂન્ય arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
સ્થિર શૂન્ય arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
સ્થિર શૂન્ય arm64_cap_pud_print_slots(શૂન્ય *pgdSlot_or_vspace, vptr_t vptr);

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
સ્થિર શૂન્ય cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

સ્થિર શૂન્ય cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    printf("(");

    /* rights */
    પસંદગી (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ 0b00:
        તોડો;
    સ્થિતિ 0b01:
        printf("R");
        તોડો;
    સ્થિતિ 0b10:
        printf("W");
        તોડો;
    સ્થિતિ 0b11:
        printf("RW");
        તોડો;
#નહીંતર
    સ્થિતિ 0b00:
        તોડો;
    સ્થિતિ 0b01:
        printf("RW");
    સ્થિતિ 0b10:
        તોડો;
    સ્થિતિ 0b11:
        printf("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    મૂળભૂત:
        તોડો;
    }

    જો (!NXN) {
        printf("X");
    }

    /* Only has effect if SMP enabled */
    જો (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        printf(", uncached");
    }

    printf(")\n");
}

/* use when only have access to vptr of frames */
સ્થિર શૂન્ય _cap_frame_print_attrs_vptr(word_t vptr, vspace_મૂળ_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    જો (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_ડાબું > PAGE_BITS)) {
        પ્રતિજ્ઞા(0);
    }

    word_t table_index;
    પસંદગી (ret.pt_Bits_ડાબું) {

    સ્થિતિ ARMHugePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
        તોડો;
    સ્થિતિ ARMLargePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
        તોડો;
    સ્થિતિ ARMSmallPage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3));
        તોડો;
    મૂળભૂત:
        પ્રતિજ્ઞા(0);

    }
    printf("frame_%p_%04lu ", ret.ptSlot, table_index);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

શૂન્ય cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
સ્થિર શૂન્ય arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    printf("pt_%p_%04lu {\n", pdSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    માટે (word_t i = 0; i < BIT(PT_INDEX_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        જો (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            printf("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    printf("}\n"); /* pt */
}

સ્થિર શૂન્ય arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    printf("pd_%p_%04lu {\n", pudSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    માટે (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        પસંદગી (pte_ptr_get_pte_type(pdSlot)) {

        સ્થિતિ pte_pte_page:
            printf("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            તોડો;

        સ્થિતિ pte_pte_table:
            printf("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            તોડો;
        }
    }

    printf("}\n"); /* pd */

    માટે (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;
        જો (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

સ્થિર શૂન્ય arm64_cap_pud_print_slots(શૂન્ય *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t index_bits = seL4_VSpaceIndexBits;
    printf("%p_pd {\n", pgdSlot_or_vspace);
#નહીંતર
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t index_bits = seL4_PageTableIndexBits;
    printf("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    માટે (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;
        જો (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            printf("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    printf("}\n"); /* pgd/pud */

    માટે (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_INDEX(i, ULVL_FRM_ARM_PT_LVL(1));
        જો (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

શૂન્ય obj_vtable_print_slots(tcb_t *tcb)
{
    જો (is_VTable_મૂળ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_મૂળ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#નહીંતર
        printf("%p_pd {\n", vspace);
        માટે (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            જો (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        printf("}\n"); /* pd */

        માટે (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            જો (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

શૂન્ય print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

શૂન્ય print_cap_arch(cap_t cap)
{

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = ખાલી;
        pte_t *pt = (pte_t *)find_ret.vspace_મૂળ;
        word_t level;
        માટે (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_INDEX(vptr, level);
            જો (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                તોડો;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        જો (pt != target_pt) {
            /* didn't find it */
            તોડો;
        }


        જો (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_INDEX(vptr, level), (long unsigned પૂર્ણાંક)asid);
        } નહીંતર {
            printf("pt_%p_%04lu\n", target_pt, GET_UPT_INDEX(vptr, level));
        }
        તોડો;
    }
    સ્થિતિ cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        જો (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.vspace_મૂળ, (long unsigned પૂર્ણાંક)asid);
        } નહીંતર {
            printf("%p_pd\n", find_ret.vspace_મૂળ);
        }
        તોડો;
    }
    સ્થિતિ cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        તોડો;
    }
    સ્થિતિ cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));
        પ્રતિજ્ઞા(find_ret.status == EXCEPTION_NONE);
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_મૂળ);
        તોડો;
    }
    સ્થિતિ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (શૂન્ય *)cap_asid_pool_cap_get_capASIDPool(cap));
        તોડો;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ cap_vcpu_cap: {
        printf("%p_vcpu\n", (શૂન્ય *)cap_vcpu_cap_get_capVCPUPtr(cap));
        તોડો;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    સ્થિતિ cap_io_space_cap: {
        printf("%p_io_space\n", (શૂન્ય *)cap_io_space_cap_get_capModuleID(cap));
        તોડો;
    }
#endif
    મૂળભૂત: {
        printf("[unknown cap %lu]\n", (long unsigned પૂર્ણાંક)cap_get_capType(cap));
        તોડો;
    }
    }
}

શૂન્ય print_object_arch(cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_frame_cap:
    સ્થિતિ cap_page_table_cap:
    સ્થિતિ cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        તોડો;

    સ્થિતિ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (શૂન્ય *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        તોડો;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (શૂન્ય *)cap_vcpu_cap_get_capVCPUPtr(cap));
        તોડો;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    સ્થિતિ cap_io_space_cap: {
        printf("%p_io_space = io_space ", (શૂન્ય *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        તોડો;
    }
#endif
    મૂળભૂત: {
        printf("[unknown object %lu]\n", (long unsigned પૂર્ણાંક)cap_get_capType(cap));
        તોડો;
    }
    }
}

શૂન્ય obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase)
{
    printf("(");

    /* VM size */
    પસંદગી (frameSize) {
    સ્થિતિ ARMHugePage:
        printf("1G");
        તોડો;
    સ્થિતિ ARMLargePage:
        printf("2M");
        તોડો;
    સ્થિતિ ARMSmallPage:
        printf("4k");
        તોડો;
    }

    printf(", paddr: 0x%p)\n", (શૂન્ય *)frameBase);
}

શૂન્ય arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    માટે (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *ptSlot = pt + i;

        જો (pte_4k_page_ptr_get_present(ptSlot)) {
            printf("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

શૂન્ય arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    માટે (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        જો (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            printf("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        જો (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            printf("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

શૂન્ય arm64_obj_pud_print_slots(શૂન્ય *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t index_bits = seL4_VSpaceIndexBits;
#નહીંતર
    word_t index_bits = seL4_PageTableIndexBits;
#endif
    માટે (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;

        પસંદગી (pte_ptr_get_pte_type(pudSlot)) {
        સ્થિતિ pte_pte_page:
            printf("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            તોડો;

        સ્થિતિ pte_pte_table: {
            printf("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

શૂન્ય obj_tcb_print_vtable(tcb_t *tcb)
{
    જો (is_VTable_મૂળ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_મૂળ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        printf("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#નહીંતર
        printf("%p_pd = pgd\n", vspace);
        માટે (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            જો (pte_pte_table_ptr_get_present(ptSlot)) {
                printf("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

શૂન્ય debug_capDL(શૂન્ય)
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
