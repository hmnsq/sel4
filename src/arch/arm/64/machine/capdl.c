/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_DEBUG_BUILD

#समावेश <arch/machine/capdl.h>
#समावेश <string.h>
#समावेश <kernel/cspace.h>

word_t get_tcb_sp(tcb_t *tcb)
{
    वापस tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#अगर_घोषित CONFIG_PRINTING

अटल व्योम obj_frame_print_attrs(vm_page_आकार_t frame_आकार, paddr_t frameBase);
अटल व्योम cap_frame_print_attrs_pt(pte_t *ptSlot);
अटल व्योम cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN);
अटल व्योम cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace);

अटल व्योम _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूल_t *vspace_मूल_2);

अटल व्योम arm64_obj_pt_print_slots(pte_t *pdSlot);
अटल व्योम arm64_obj_pd_print_slots(pte_t *pudSlot);
अटल व्योम arm64_obj_pud_print_slots(व्योम *pgdSlot_or_vspace);

अटल व्योम arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr);
अटल व्योम arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr);
अटल व्योम arm64_cap_pud_print_slots(व्योम *pgdSlot_or_vspace, vptr_t vptr);

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
अटल व्योम cap_frame_print_attrs_pt(pte_t *ptSlot)
{
    cap_frame_print_attrs_impl(pte_pte_page_ptr_get_SH(ptSlot),
                               pte_pte_page_ptr_get_AP(ptSlot),
                               pte_pte_page_ptr_get_UXN(ptSlot));
}

अटल व्योम cap_frame_print_attrs_impl(word_t SH, word_t AP, word_t NXN)
{
    म_लिखो("(");

    /* rights */
    चयन (AP) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल 0b00:
        अवरोध;
    हाल 0b01:
        म_लिखो("R");
        अवरोध;
    हाल 0b10:
        म_लिखो("W");
        अवरोध;
    हाल 0b11:
        म_लिखो("RW");
        अवरोध;
#अन्यथा
    हाल 0b00:
        अवरोध;
    हाल 0b01:
        म_लिखो("RW");
    हाल 0b10:
        अवरोध;
    हाल 0b11:
        म_लिखो("R");
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    शेष:
        अवरोध;
    }

    अगर (!NXN) {
        म_लिखो("X");
    }

    /* Only has effect if SMP enabled */
    अगर (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        म_लिखो(", uncached");
    }

    म_लिखो(")\n");
}

/* use when only have access to vptr of frames */
अटल व्योम _cap_frame_print_attrs_vptr(word_t vptr, vspace_मूल_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    अगर (!pte_ptr_get_valid(ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(ret.ptSlot) && ret.pt_Bits_बायाँ > PAGE_BITS)) {
        निश्चित(0);
    }

    word_t table_सूचकांक;
    चयन (ret.pt_Bits_बायाँ) {

    हाल ARMHugePage:
        table_सूचकांक = GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1));
        अवरोध;
    हाल ARMLargePage:
        table_सूचकांक = GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(2));
        अवरोध;
    हाल ARMSmallPage:
        table_सूचकांक = GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(3));
        अवरोध;
    शेष:
        निश्चित(0);

    }
    म_लिखो("frame_%p_%04lu ", ret.ptSlot, table_सूचकांक);
    cap_frame_print_attrs_pt(ret.ptSlot);
}

व्योम cap_frame_print_attrs_vptr(word_t vptr, cap_t vspace)
{
    _cap_frame_print_attrs_vptr(vptr, VSPACE_PTR(pptr_of_cap(vspace)));
}

/*
 * print object slots
 */
अटल व्योम arm64_cap_pt_print_slots(pte_t *pdSlot, vptr_t vptr)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));
    म_लिखो("pt_%p_%04lu {\n", pdSlot, GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    क्रम (word_t i = 0; i < BIT(PT_सूचकांक_BITS); i ++) {
        pte_t *ptSlot = pt + i;

        अगर (pte_4k_page_ptr_get_present(ptSlot)) {
            // print pte entries
            म_लिखो("0x%lx: frame_%p_%04lu", i, ptSlot, i);
            cap_frame_print_attrs_pt(ptSlot);
        }
    }
    म_लिखो("}\n"); /* pt */
}

अटल व्योम arm64_cap_pd_print_slots(pte_t *pudSlot, vptr_t vptr)
{
    म_लिखो("pd_%p_%04lu {\n", pudSlot, GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    क्रम (word_t i = 0; i < BIT(PT_सूचकांक_BITS); i++) {
        pte_t *pdSlot = pd + i;

        चयन (pte_ptr_get_pte_type(pdSlot)) {

        हाल pte_pte_page:
            म_लिखो("0x%lx: frame_%p_%04lu", i, pdSlot, i);
            cap_frame_print_attrs_pt(pdSlot);
            अवरोध;

        हाल pte_pte_table:
            म_लिखो("0x%lx: pt_%p_%04lu\n", i, pdSlot, i);
            अवरोध;
        }
    }

    म_लिखो("}\n"); /* pd */

    क्रम (word_t i = 0; i < BIT(PT_सूचकांक_BITS); i++) {
        pte_t *pdSlot = pd + i;
        अगर (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            arm64_cap_pt_print_slots(pdSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

अटल व्योम arm64_cap_pud_print_slots(व्योम *pgdSlot_or_vspace, vptr_t vptr)
{
#अगर_घोषित AARCH64_VSPACE_S2_START_L1
    pte_t *pud = pgdSlot_or_vspace;
    word_t सूचकांक_bits = se_L4_VSpace_सूचकांक_Bits;
    म_लिखो("%p_pd {\n", pgdSlot_or_vspace);
#अन्यथा
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
    word_t सूचकांक_bits = se_L4_Page_Table_सूचकांक_Bits;
    म_लिखो("pud_%p_%04lu {\n", pgdSlot_or_vspace, GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#पूर्ण_अगर

    क्रम (word_t i = 0; i < BIT(सूचकांक_bits); i++) {
        pte_t *pudSlot = pud + i;
        अगर (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            म_लिखो("0x%lx: pd_%p_%04lu\n", i, pudSlot, i);
        }
    }

    म_लिखो("}\n"); /* pgd/pud */

    क्रम (word_t i = 0; i < BIT(सूचकांक_bits); i++) {
        pte_t *pudSlot = pud + GET_UPT_सूचकांक(i, ULVL_FRM_ARM_PT_LVL(1));
        अगर (pte_ptr_get_pte_type(pudSlot) == pte_pte_table) {
            arm64_cap_pd_print_slots(pudSlot, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

व्योम obj_vtable_print_slots(tcb_t *tcb)
{
    अगर (is_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_मूल_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#अगर_घोषित AARCH64_VSPACE_S2_START_L1
        arm64_cap_pud_print_slots(vspace, 0);
#अन्यथा
        म_लिखो("%p_pd {\n", vspace);
        क्रम (word_t i = 0; i < PT_सूचकांक_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            अगर (pte_pte_table_ptr_get_present(ptSlot)) {
                म_लिखो("0x%lx: pud_%p_%04lu\n", i, ptSlot, i);
            }
        }
        म_लिखो("}\n"); /* pd */

        क्रम (word_t i = 0; i < PT_सूचकांक_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            अगर (pte_pte_table_ptr_get_present(ptSlot)) {
                arm64_cap_pud_print_slots(ptSlot, i * GET_ULVL_PGSIZE(0));
            }
        }
#पूर्ण_अगर
    }
}

व्योम print_ipc_बफर_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    म_लिखो("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

व्योम print_cap_arch(cap_t cap)
{

    चयन (cap_get_capType(cap)) {
    हाल cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        pte_t *target_pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        pte_t *ptSlot = शून्य;
        pte_t *pt = (pte_t *)find_ret.vspace_मूल;
        word_t level;
        क्रम (level = 0; level < UPT_LEVELS - 1 && pt != target_pt; level++) {
            ptSlot = pt + GET_UPT_सूचकांक(vptr, level);
            अगर (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
                /* couldn't find it */
                अवरोध;
            }
            pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
        }
        अगर (pt != target_pt) {
            /* didn't find it */
            अवरोध;
        }


        अगर (asid) {
            म_लिखो("pt_%p_%04lu (asid: %lu)\n",
                   target_pt, GET_UPT_सूचकांक(vptr, level), (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("pt_%p_%04lu\n", target_pt, GET_UPT_सूचकांक(vptr, level));
        }
        अवरोध;
    }
    हाल cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        अगर (asid) {
            म_लिखो("%p_pd (asid: %lu)\n",
                   find_ret.vspace_मूल, (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("%p_pd\n", find_ret.vspace_मूल);
        }
        अवरोध;
    }
    हाल cap_asid_control_cap: {
        /* only one in the system */
        म_लिखो("asid_control\n");
        अवरोध;
    }
    हाल cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(cap_frame_cap_get_capFMappedASID(cap));
        निश्चित(find_ret.status == EXCEPTION_NONE);
        _cap_frame_print_attrs_vptr(vptr, find_ret.vspace_मूल);
        अवरोध;
    }
    हाल cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool\n", (व्योम *)cap_asid_pool_cap_get_capASIDPool(cap));
        अवरोध;
    }
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap: {
        म_लिखो("%p_vcpu\n", (व्योम *)cap_vcpu_cap_get_capVCPUPtr(cap));
        अवरोध;
    }
#पूर्ण_अगर

        /* ARM specific caps */
#अगर_घोषित CONFIG_TK1_SMMU
    हाल cap_io_space_cap: {
        म_लिखो("%p_io_space\n", (व्योम *)cap_io_space_cap_get_capModuleID(cap));
        अवरोध;
    }
#पूर्ण_अगर
    शेष: {
        म_लिखो("[unknown cap %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        अवरोध;
    }
    }
}

व्योम print_object_arch(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_frame_cap:
    हाल cap_page_table_cap:
    हाल cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        अवरोध;

    हाल cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool = asid_pool ",
               (व्योम *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        अवरोध;
    }
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap: {
        म_लिखो("%p_vcpu = vcpu\n", (व्योम *)cap_vcpu_cap_get_capVCPUPtr(cap));
        अवरोध;
    }
#पूर्ण_अगर
        /* ARM specific objects */
#अगर_घोषित CONFIG_TK1_SMMU
    हाल cap_io_space_cap: {
        म_लिखो("%p_io_space = io_space ", (व्योम *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        अवरोध;
    }
#पूर्ण_अगर
    शेष: {
        म_लिखो("[unknown object %lu]\n", (दीर्घ अचिन्हित पूर्णांक)cap_get_capType(cap));
        अवरोध;
    }
    }
}

व्योम obj_frame_print_attrs(vm_page_आकार_t frame_आकार, paddr_t frameBase)
{
    म_लिखो("(");

    /* VM size */
    चयन (frame_आकार) {
    हाल ARMHugePage:
        म_लिखो("1G");
        अवरोध;
    हाल ARMLargePage:
        म_लिखो("2M");
        अवरोध;
    हाल ARMSmallPage:
        म_लिखो("4k");
        अवरोध;
    }

    म_लिखो(", paddr: 0x%p)\n", (व्योम *)frameBase);
}

व्योम arm64_obj_pt_print_slots(pte_t *pdSlot)
{
    pte_t *pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pdSlot));

    क्रम (word_t i = 0; i < BIT(PT_सूचकांक_BITS); i++) {
        pte_t *ptSlot = pt + i;

        अगर (pte_4k_page_ptr_get_present(ptSlot)) {
            म_लिखो("frame_%p_%04lu = frame ", ptSlot, i);
            obj_frame_print_attrs(ARMSmallPage, pte_page_ptr_get_page_base_address(ptSlot));
        }
    }
}

व्योम arm64_obj_pd_print_slots(pte_t *pudSlot)
{
    pte_t *pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pudSlot));

    क्रम (word_t i = 0; i < BIT(PT_सूचकांक_BITS); i++) {
        pte_t *pdSlot = pd + i;

        अगर (pte_ptr_get_pte_type(pdSlot) == pte_pte_page) {
            म_लिखो("frame_%p_%04lu = frame ", pdSlot, i);
            obj_frame_print_attrs(ARMLargePage, pte_page_ptr_get_page_base_address(pdSlot));
        }

        अगर (pte_ptr_get_pte_type(pdSlot) == pte_pte_table) {
            म_लिखो("pt_%p_%04lu = pt\n", pdSlot, i);
            arm64_obj_pt_print_slots(pdSlot);
        }
    }
}

व्योम arm64_obj_pud_print_slots(व्योम *pgdSlot_or_vspace)
{
    pte_t *pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pgdSlot_or_vspace));
#अगर_घोषित AARCH64_VSPACE_S2_START_L1
    word_t सूचकांक_bits = se_L4_VSpace_सूचकांक_Bits;
#अन्यथा
    word_t सूचकांक_bits = se_L4_Page_Table_सूचकांक_Bits;
#पूर्ण_अगर
    क्रम (word_t i = 0; i < BIT(सूचकांक_bits); i++) {
        pte_t *pudSlot = pud + i;

        चयन (pte_ptr_get_pte_type(pudSlot)) {
        हाल pte_pte_page:
            म_लिखो("frame_%p_%04lu = frame ", pudSlot, i);
            obj_frame_print_attrs(ARMHugePage, pte_page_ptr_get_page_base_address(pudSlot));
            अवरोध;

        हाल pte_pte_table: {
            म_लिखो("pd_%p_%04lu = pd\n", pudSlot, i);
            arm64_obj_pd_print_slots(pudSlot);

        }
        }
    }
}

व्योम obj_tcb_print_vtable(tcb_t *tcb)
{
    अगर (is_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_मूल_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#अगर_घोषित AARCH64_VSPACE_S2_START_L1
        म_लिखो("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#अन्यथा
        म_लिखो("%p_pd = pgd\n", vspace);
        क्रम (word_t i = 0; i < PT_सूचकांक_BITS; i++) {
            pte_t *ptSlot = vspace + i;
            अगर (pte_pte_table_ptr_get_present(ptSlot)) {
                म_लिखो("pud_%p_%04lu = pud\n", ptSlot, i);
                arm64_obj_pud_print_slots(ptSlot);
            }
        }
#पूर्ण_अगर
    }
}

#पूर्ण_अगर /* CONFIG_PRINTING */

व्योम debug_capDL(व्योम)
{
    म_लिखो("arch aarch64\n");
    म_लिखो("objects {\n");
#अगर_घोषित CONFIG_PRINTING
    print_objects();
#पूर्ण_अगर
    म_लिखो("}\n");

    म_लिखो("caps {\n");

    /* reset the seen list */
    reset_seen_list();

#अगर_घोषित CONFIG_PRINTING
    print_caps();
    म_लिखो("}\n");

    obj_irq_print_maps();
#पूर्ण_अगर /* CONFIG_PRINTING */
}

#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
