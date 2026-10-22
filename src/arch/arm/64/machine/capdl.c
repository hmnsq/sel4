/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD

#अन्तर्भाव <arch/machine/capdl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    प्रत्यागम tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#यदि_परिभाषितम् CONFIG_PRINTING

स्थिर शून्य obj_frame_print_attrs(vm_page_आकार_t frame_आकार, paddr_t frame_आधारः);
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *ptSlot);
स्थिर शून्य cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

स्थिर शून्य _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूलम्_t *vspace_मूलम्_2);

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
    प्रारूप_लिख("(");

    /* rights */
    विकल्प (AP) {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष 0b00:
        भङ्ग;
    पक्ष 0b01:
        प्रारूप_लिख("R");
        भङ्ग;
    पक्ष 0b10:
        प्रारूप_लिख("W");
        भङ्ग;
    पक्ष 0b11:
        प्रारूप_लिख("RW");
        भङ्ग;
#अन्यथा
    पक्ष 0b00:
        भङ्ग;
    पक्ष 0b01:
        प्रारूप_लिख("RW");
    पक्ष 0b10:
        भङ्ग;
    पक्ष 0b11:
        प्रारूप_लिख("R");
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    स्वभाव:
        भङ्ग;
    }

    यदि (!NXN) {
        प्रारूप_लिख("X");
    }

    /* Only has effect if SMP enabled */
    यदि (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        प्रारूप_लिख(", uncached");
    }

    प्रारूप_लिख(")\n");
}

/* use when only have access to vptr of frames */
स्थिर शून्य _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूलम्_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    यदि (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_सारणी_ptr_get_present(ret.ptSlot) && ret.pt_द्वयङ्काः_वामः > PAGE_द्वयङ्काः)) {
        प्रतिज्ञा(0);
    }

    word_t सारणी_सूचकांक;
    विकल्प (ret.pt_द्वयङ्काः_वामः) {

    पक्ष ARMHugePage:
        सारणी_सूचकांक = GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1));
        भङ्ग;
    पक्ष ARMLargePage:
        सारणी_सूचकांक = GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(2));
        भङ्ग;
    पक्ष ARMSmallPage:
        सारणी_सूचकांक = GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(3));
        भङ्ग;
    स्वभाव:
        प्रतिज्ञा(0);

    }
    प्रारूप_लिख("frame_%p_%04lu ", ret.ptSlot, सारणी_सूचकांक);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

शून्य cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_सम्बन्धि_cap(vspace)));
}

/*
 * print object slots
 */
स्थिर शून्य arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pdSlot));
    प्रारूप_लिख("pt_%p_%04lu {\n", pdSlot, GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    प्रति (word_t i = 0; i < BIT(PT_सूचकांक_द्वयङ्काः); i ++) {
        pte_t *ptSlot = pt + i;

        यदि (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            प्रारूप_लिख("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    प्रारूप_लिख("}\n"); /* pt */
}

स्थिर शून्य arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    प्रारूप_लिख("pd_%p_%04lu {\n", pudSlot, GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pudSlot));

    प्रति (word_t i = 0; i < BIT(PT_सूचकांक_द्वयङ्काः); i++) {
        pte_t *pdSlot = pd + i;

        विकल्प (pte_ptr_get_pte_type(pdSlot)) {

        पक्ष pte_pte_page:
            प्रारूप_लिख("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            भङ्ग;

        पक्ष pte_pte_सारणी:
            प्रारूप_लिख("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            भङ्ग;
        }
    }

    प्रारूप_लिख("}\n"); /* pd */

    प्रति (word_t i = 0; i < BIT(PT_सूचकांक_द्वयङ्काः); i++) {
        pte_t *pdSlot = pd + i;
        यदि (pte_ptr_get_pte_type(pdSlot) == pte_pte_सारणी) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

स्थिर शून्य arm64_cap_pud_print_slots(शून्य *pgdSlot_or_vspace, vptr_t vptr)
{
#यदि_परिभाषितम् AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t सूचकांक_द्वयङ्काः = se_L4_VSpace_सूचकांक_द्वयङ्काः;
    प्रारूप_लिख("%p_pd {\n", pgdSlot_or_vspace);
#अन्यथा
    pte_t *pud = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pgdSlot_or_vspace));
    word_t सूचकांक_द्वयङ्काः = se_L4_Page_सारणी_सूचकांक_द्वयङ्काः;
    प्रारूप_लिख("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#यदि_अन्तः

    प्रति (word_t i = 0; i < BIT(सूचकांक_द्वयङ्काः); i++) {
        pte_t *pudSlot = pud + i;
        यदि (pte_ptr_get_pte_type(pudSlot) == pte_pte_सारणी) {
            प्रारूप_लिख("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    प्रारूप_लिख("}\n"); /* pgd/pud */

    प्रति (word_t i = 0; i < BIT(सूचकांक_द्वयङ्काः); i++) {
        pte_t *pudSlot = pud + GET_UPT_सूचकांक(i, ULVL_FRM_ARM_PT_LVL(1));
        यदि (pte_ptr_get_pte_type(pudSlot) == pte_pte_सारणी) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    यदि (is_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        सङ्कलय_गन्तव्यम्_आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_मूलम्_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#यदि_परिभाषितम् AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#अन्यथा
        प्रारूप_लिख("%p_pd {\n", vspace);
        प्रति (word_t i = 0; i < PT_सूचकांक_द्वयङ्काः; i++) {
            pte_t *ptSlot = vspace + i;
            यदि (pte_pte_सारणी_ptr_get_present(ptSlot)) {
                प्रारूप_लिख("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        प्रारूप_लिख("}\n"); /* pd */

        प्रति (word_t i = 0; i < PT_सूचकांक_द्वयङ्काः; i++) {
            pte_t *ptSlot = vspace + i;
            यदि (pte_pte_सारणी_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#यदि_अन्तः
    }
}

शून्य print_ipc_बफर_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    प्रारूप_लिख("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

शून्य print_cap_arch(cap_t cap)
{

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_सारणी_cap: {
        asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap);
        pte_t *target_pt = PT_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = रिक्त;
        pte_t *pt = (pte_t *)find_ret.vspace_मूलम्;
        word_t level;
        प्रति (level = 0; level < UPT_स्तराः - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_सूचकांक(vptr, level);
            यदि (unlikely(!pte_pte_सारणी_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                भङ्ग;
            }
            pt = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(ptSlot));
        }
        यदि (pt != target_pt) {
            /* didn't find it */
            भङ्ग;
        }


        यदि (asid) {
            प्रारूप_लिख("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_सूचकांक(vptr, level), (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("pt_%p_%04lu\n", target_pt, GET_UPT_सूचकांक(vptr, level));
        }
        भङ्ग;
    }
    पक्ष cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        यदि (asid) {
            प्रारूप_लिख("%p_pd (asid: %lu)\n",
                   find_ret.vspace_मूलम्, (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("%p_pd\n", find_ret.vspace_मूलम्);
        }
        भङ्ग;
    }
    पक्ष cap_asid_control_cap: {
        /* only one in the system */
        प्रारूप_लिख("asid_control\n");
        भङ्ग;
    }
    पक्ष cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));
        प्रतिज्ञा(find_ret.स्थितिः == EXCEPTION_NONE);
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूलम्);
        भङ्ग;
    }
    पक्ष cap_asid_pool_cap: {
        प्रारूप_लिख("%p_asid_pool\n", (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        भङ्ग;
    }
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap: {
        प्रारूप_लिख("%p_vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        भङ्ग;
    }
#यदि_अन्तः

        /* ARM specific caps */
#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap: {
        प्रारूप_लिख("%p_io_space\n", (शून्य *)cap_io_space_cap_get_capModuleID(cap));
        भङ्ग;
    }
#यदि_अन्तः
    स्वभाव: {
        प्रारूप_लिख("[unknown cap %lu]\n", (दीर्घ अचिह्नित पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

शून्य print_object_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_frame_cap:
    पक्ष cap_page_सारणी_cap:
    पक्ष cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        भङ्ग;

    पक्ष cap_asid_pool_cap: {
        प्रारूप_लिख("%p_asid_pool = asid_pool ",
               (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        भङ्ग;
    }
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap: {
        प्रारूप_लिख("%p_vcpu = vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        भङ्ग;
    }
#यदि_अन्तः
        /* ARM specific objects */
#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap: {
        प्रारूप_लिख("%p_io_space = io_space ", (शून्य *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        भङ्ग;
    }
#यदि_अन्तः
    स्वभाव: {
        प्रारूप_लिख("[unknown object %lu]\n", (दीर्घ अचिह्नित पूर्णाङ्क)cap_get_capType(cap));
        भङ्ग;
    }
    }
}

शून्य obj_frame_print_attrs(vm_page_आकार_t frame_आकार, paddr_t frame_आधारः)
{
    प्रारूप_लिख("(");

    /* VM size */
    विकल्प (frame_आकार) {
    पक्ष ARMHugePage:
        प्रारूप_लिख("1G");
        भङ्ग;
    पक्ष ARMLargePage:
        प्रारूप_लिख("2M");
        भङ्ग;
    पक्ष ARMSmallPage:
        प्रारूप_लिख("4k");
        भङ्ग;
    }

    प्रारूप_लिख(", paddr: 0x%p)\n", (शून्य *)frame_आधारः);
}

शून्य arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pdSlot));

    प्रति (word_t i = 0; i < BIT(PT_सूचकांक_द्वयङ्काः); i++) {
        pte_t *ptSlot = pt + i;

        यदि (pte_4k_page_ptr_get_present(ptSlot)) {
            प्रारूप_लिख("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_आधारः_स्थानाङ्कः(ptSlot));
        }
    }
}

शून्य arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pudSlot));

    प्रति (word_t i = 0; i < BIT(PT_सूचकांक_द्वयङ्काः); i++) {
        pte_t *pdSlot = pd + i;

        यदि (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            प्रारूप_लिख("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_आधारः_स्थानाङ्कः(pdSlot));
        }

        यदि (pte_ptr_get_pte_type(pdSlot) == pte_pte_सारणी) {
            प्रारूप_लिख("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

शून्य arm64_obj_pud_print_slots(शून्य *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pgdSlot_or_vspace));
#यदि_परिभाषितम् AARCH64_VSPACE_S2_START_L1
    word_t सूचकांक_द्वयङ्काः = se_L4_VSpace_सूचकांक_द्वयङ्काः;
#अन्यथा
    word_t सूचकांक_द्वयङ्काः = se_L4_Page_सारणी_सूचकांक_द्वयङ्काः;
#यदि_अन्तः
    प्रति (word_t i = 0; i < BIT(सूचकांक_द्वयङ्काः); i++) {
        pte_t *pudSlot = pud + i;

        विकल्प (pte_ptr_get_pte_type(pudSlot)) {
        पक्ष pte_pte_page:
            प्रारूप_लिख("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_आधारः_स्थानाङ्कः(pudSlot));
            भङ्ग;

        पक्ष pte_pte_सारणी: {
            प्रारूप_लिख("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    यदि (is_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        सङ्कलय_गन्तव्यम्_आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_मूलम्_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#यदि_परिभाषितम् AARCH64_VSPACE_S2_START_L1
        प्रारूप_लिख("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#अन्यथा
        प्रारूप_लिख("%p_pd = pgd\n", vspace);
        प्रति (word_t i = 0; i < PT_सूचकांक_द्वयङ्काः; i++) {
            pte_t *ptSlot = vspace + i;
            यदि (pte_pte_सारणी_ptr_get_present(ptSlot)) {
                प्रारूप_लिख("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#यदि_अन्तः
    }
}

#यदि_अन्तः /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    प्रारूप_लिख("arch aarch64\n");
    प्रारूप_लिख("objects {\n");
#यदि_परिभाषितम् CONFIG_PRINTING
    print_objects();
#यदि_अन्तः
    प्रारूप_लिख("}\n");

    प्रारूप_लिख("caps {\n");

    /* reset the seen list */
    reset_आलोकितम्_list();

#यदि_परिभाषितम् CONFIG_PRINTING
    print_caps();
    प्रारूप_लिख("}\n");

    obj_irq_print_maps();
#यदि_अन्तः /* CONFIG_PRINTING */
}

#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
