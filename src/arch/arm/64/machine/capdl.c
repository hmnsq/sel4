/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_DEBUG_BUILD

#अन्तर्भाव <arch/machine/capdl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    परत tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ifdef CONFIG_PRINTING

स्थिर शून्य obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase);
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot);
स्थिर शून्य cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

स्थिर शून्य _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूळ_t *vspace_मूळ_2);

स्थिर शून्य arm64_obj_pt_print_slots(pte_t *pdSlot);
स्थिर शून्य arm64_obj_pd_print_slots(pte_t *pudSlot);
स्थिर शून्य arm64_obj_pud_print_slots(शून्य *pgdSlot_or_vspace);

स्थिर शून्य arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
स्थिर शून्य arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
स्थिर शून्य arm64_cap_pud_print_slots(शून्य *pgdSlot_or_vspace, vptr_t vptr);

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
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

स्थिर शून्य cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    म_लिखो("(");

    /* rights */
    निवड (AP) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती 0b00:
        थांबा;
    स्थिती 0b01:
        म_लिखो("R");
        थांबा;
    स्थिती 0b10:
        म_लिखो("W");
        थांबा;
    स्थिती 0b11:
        म_लिखो("RW");
        थांबा;
#अन्यथा
    स्थिती 0b00:
        थांबा;
    स्थिती 0b01:
        म_लिखो("RW");
    स्थिती 0b10:
        थांबा;
    स्थिती 0b11:
        म_लिखो("R");
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    पूर्वनिर्धारित:
        थांबा;
    }

    जर (!NXN) {
        म_लिखो("X");
    }

    /* Only has effect if SMP enabled */
    जर (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        म_लिखो(", uncached");
    }

    म_लिखो(")\n");
}

/* use when only have access to vptr of frames */
स्थिर शून्य _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूळ_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    जर (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_डावा > PAGE_BITS)) {
        प्रतिज्ञा(0);
    }

    word_t table_index;
    निवड (ret.pt_Bits_डावा) {

    स्थिती ARMHugePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
        थांबा;
    स्थिती ARMLargePage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
        थांबा;
    स्थिती ARMSmallPage:
        table_index = GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3));
        थांबा;
    पूर्वनिर्धारित:
        प्रतिज्ञा(0);

    }
    म_लिखो("frame_%p_%04lu ", ret.ptSlot, table_index);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

शून्य cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
स्थिर शून्य arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    म_लिखो("pt_%p_%04lu {\n", pdSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    साठी (word_t i = 0; i < BIT(PT_INDEX_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        जर (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            म_लिखो("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    म_लिखो("}\n"); /* pt */
}

स्थिर शून्य arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    म_लिखो("pd_%p_%04lu {\n", pudSlot, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    साठी (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        निवड (pte_ptr_get_pte_type(pdSlot)) {

        स्थिती pte_pte_page:
            म_लिखो("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            थांबा;

        स्थिती pte_pte_table:
            म_लिखो("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            थांबा;
        }
    }

    म_लिखो("}\n"); /* pd */

    साठी (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;
        जर (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

स्थिर शून्य arm64_cap_pud_print_slots(शून्य *pgdSlot_or_vspace, vptr_t vptr)
{
#ifdef AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t index_bits = seL4_VSpaceIndexBits;
    म_लिखो("%p_pd {\n", pgdSlot_or_vspace);
#अन्यथा
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t index_bits = seL4_PageTableIndexBits;
    म_लिखो("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#endif

    साठी (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;
        जर (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            म_लिखो("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    म_लिखो("}\n"); /* pgd/pud */

    साठी (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_INDEX(i, ULVL_FRM_ARM_PT_LVL(1));
        जर (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    जर (is_VTable_मूळ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_मूळ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ifdef AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#अन्यथा
        म_लिखो("%p_pd {\n", vspace);
        साठी (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            जर (pte_pte_table_ptr_get_present(ptSlot)) {
                म_लिखो("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        म_लिखो("}\n"); /* pd */

        साठी (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            जर (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#endif
    }
}

शून्य print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    म_लिखो("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

शून्य print_cap_arch(cap_t cap)
{

    निवड (cap_get_capType(cap)) {
    स्थिती cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = रिक्त;
        pte_t *pt = (pte_t *)find_ret.vspace_मूळ;
        word_t level;
        साठी (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_INDEX(vptr, level);
            जर (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                थांबा;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        जर (pt != target_pt) {
            /* didn't find it */
            थांबा;
        }


        जर (asid) {
            म_लिखो("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_INDEX(vptr, level), (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("pt_%p_%04lu\n", target_pt, GET_UPT_INDEX(vptr, level));
        }
        थांबा;
    }
    स्थिती cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        जर (asid) {
            म_लिखो("%p_pd (asid: %lu)\n",
                   find_ret.vspace_मूळ, (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("%p_pd\n", find_ret.vspace_मूळ);
        }
        थांबा;
    }
    स्थिती cap_asid_control_cap: {
        /* only one in the system */
        म_लिखो("asid_control\n");
        थांबा;
    }
    स्थिती cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));
        प्रतिज्ञा(find_ret.status == EXCEPTION_NONE);
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूळ);
        थांबा;
    }
    स्थिती cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool\n", (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        थांबा;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती cap_vcpu_cap: {
        म_लिखो("%p_vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        थांबा;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    स्थिती cap_io_space_cap: {
        म_लिखो("%p_io_space\n", (शून्य *)cap_io_space_cap_get_capModuleID(cap));
        थांबा;
    }
#endif
    पूर्वनिर्धारित: {
        म_लिखो("[unknown cap %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        थांबा;
    }
    }
}

शून्य print_object_arch(cap_t cap)
{
    निवड (cap_get_capType(cap)) {
    स्थिती cap_frame_cap:
    स्थिती cap_page_table_cap:
    स्थिती cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        थांबा;

    स्थिती cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool = asid_pool ",
               (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        थांबा;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती cap_vcpu_cap: {
        म_लिखो("%p_vcpu = vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        थांबा;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    स्थिती cap_io_space_cap: {
        म_लिखो("%p_io_space = io_space ", (शून्य *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        थांबा;
    }
#endif
    पूर्वनिर्धारित: {
        म_लिखो("[unknown object %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        थांबा;
    }
    }
}

शून्य obj_frame_print_attrs(vm_page_size_t frameSize, paddr_t frameBase)
{
    म_लिखो("(");

    /* VM size */
    निवड (frameSize) {
    स्थिती ARMHugePage:
        म_लिखो("1G");
        थांबा;
    स्थिती ARMLargePage:
        म_लिखो("2M");
        थांबा;
    स्थिती ARMSmallPage:
        म_लिखो("4k");
        थांबा;
    }

    म_लिखो(", paddr: 0x%p)\n", (शून्य *)frameBase);
}

शून्य arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    साठी (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *ptSlot = pt + i;

        जर (pte_4k_page_ptr_get_present(ptSlot)) {
            म_लिखो("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

शून्य arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    साठी (word_t i = 0; i < BIT(PT_INDEX_BITS); i++) {
        pte_t *pdSlot = pd + i;

        जर (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            म_लिखो("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        जर (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            म_लिखो("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

शून्य arm64_obj_pud_print_slots(शून्य *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#ifdef AARCH64_VSPACE_S2_START_L1
    word_t index_bits = seL4_VSpaceIndexBits;
#अन्यथा
    word_t index_bits = seL4_PageTableIndexBits;
#endif
    साठी (word_t i = 0; i < BIT(index_bits); i++) {
        pte_t *pudSlot = pud + i;

        निवड (pte_ptr_get_pte_type(pudSlot)) {
        स्थिती pte_pte_page:
            म_लिखो("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            थांबा;

        स्थिती pte_pte_table: {
            म_लिखो("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    जर (is_VTable_मूळ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_मूळ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ifdef AARCH64_VSPACE_S2_START_L1
        म_लिखो("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#अन्यथा
        म_लिखो("%p_pd = pgd\n", vspace);
        साठी (word_t i = 0; i < PT_INDEX_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            जर (pte_pte_table_ptr_get_present(ptSlot)) {
                म_लिखो("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#endif
    }
}

#endif /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    म_लिखो("arch aarch64\n");
    म_लिखो("objects {\n");
#ifdef CONFIG_PRINTING
    print_objects();
#endif
    म_लिखो("}\n");

    म_लिखो("caps {\n");

    /* reset the seen list */
    reset_seen_list();

#ifdef CONFIG_PRINTING
    print_caps();
    म_लिखो("}\n");

    obj_irq_print_maps();
#endif /* CONFIG_PRINTING */
}

#endif /* CONFIG_DEBUG_BUILD */
