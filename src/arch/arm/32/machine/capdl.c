/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_DEBUG_BUILD

#समावेश <arch/machine/capdl.h>
#समावेश <string.h>
#समावेश <kernel/cspace.h>

#घोषणा MAX_UL          0xffffffff
#घोषणा PT_सूचकांक(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_सूचकांक_BITS))
#घोषणा PD_सूचकांक(vptr)  (vptr >> (PAGE_BITS + PT_सूचकांक_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    वापस tcb->tcbArch.tcbContext.registers[SP];
}

#अगर_घोषित CONFIG_PRINTING

अटल व्योम obj_frame_print_attrs(resolve_ret_t ret);
अटल व्योम cap_frame_print_attrs_pt(pte_t *pte);
अटल व्योम cap_frame_print_attrs_pd(pde_t *pde);
अटल व्योम cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
अटल व्योम arm32_obj_pt_print_slots(pte_t *pt);
अटल व्योम arm32_cap_pt_print_slots(pte_t *pt);

/*
 * Caps
 */

/*
 * AP   S   R   Privileged permissions  User permissions
 * 00   0   0   No access               No access
 * 00   1   0   Read-only               No access
 * 00   0   1   Read-only               Read-only
 * 00   1   1   Unpredictable           Unpredictable
 * 01   x   x   Read/write              No access
 * 10   x   x   Read/write              Read-only
 * 11   x   x   Read/write              Read/write
 */
/* use when only have access to pte of frames */
अटल व्योम cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    चयन (pte_ptr_get_pteType(pte)) {
    हाल pte_pte_small:
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#अन्यथा
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#पूर्ण_अगर
        XN = pte_pte_small_ptr_get_XN(pte);
        अवरोध;

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        अवरोध;
#पूर्ण_अगर
    शेष:
        निश्चित(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

अटल व्योम cap_frame_print_attrs_pd(pde_t *pde)
{
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#अन्यथा
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#पूर्ण_अगर
}

अटल व्योम cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    म_लिखो("(");

    /* rights */
    चयन (AP) {
    हाल 0b00:
    हाल 0b01:
        अवरोध;
    हाल 0b10:
        म_लिखो("R");
        अवरोध;
    हाल 0b11:
        म_लिखो("RW");
    शेष:
        अवरोध;
    }

    अगर (!XN) {
        म_लिखो("X");
    }

    अगर (!TEX) {
        म_लिखो(", uncached");
    }

    म_लिखो(")\n");
}

अटल व्योम arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_आकार_t page_आकार;
    word_t i = 0;
    जबतक (i < BIT(PT_सूचकांक_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        चयन (pte_ptr_get_pteType(pte)) {
        हाल pte_pte_small: {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
            अगर (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_आकार = ARMLargePage;
            } अन्यथा {
                page_आकार = ARMSmallPage;
            }
#अन्यथा
            page_आकार = ARMSmallPage;
#पूर्ण_अगर
            म_लिखो("0x%lx: frame_%p_%04lu ", PT_सूचकांक(i), pte, PT_सूचकांक(i));
            cap_frame_print_attrs_pt(pte);
            अवरोध;
        }
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        हाल pte_pte_large: {
            page_आकार = ARMLargePage;
            म_लिखो("0x%lx: frame_%p_%04lu ", PT_सूचकांक(i), pte, PT_सूचकांक(i));
            cap_frame_print_attrs_pt(pte);
            अवरोध;
        }
#पूर्ण_अगर
        शेष:
            page_आकार = ARMSmallPage;
        }
        i += (1 << page_Bits_For_आकार(page_आकार));
    }
}

व्योम obj_vtable_print_slots(tcb_t *tcb)
{
    अगर (is_Valid_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_आकार_t page_आकार;
        म_लिखो("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        जबतक (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            चयन (pde_ptr_get_pdeType(pde)) {
            हाल pde_pde_section: {
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                अगर (pde_pde_section_ptr_get_आकार(pde)) {
                    page_आकार = ARMSuperSection;
                } अन्यथा {
                    page_आकार = ARMSection;
                }
#अन्यथा
                अगर (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_आकार = ARMSuperSection;
                } अन्यथा {
                    page_आकार = ARMSection;
                }
#पूर्ण_अगर
                म_लिखो("0x%lx: frame_%p_%04lu ", PD_सूचकांक(i), pde, PD_सूचकांक(i));
                cap_frame_print_attrs_pd(pde);
                अवरोध;
            }

            हाल pde_pde_coarse: {
                म_लिखो("0x%lx: pt_%p_%04lu\n", PD_सूचकांक(i), pde, PD_सूचकांक(i));
                page_आकार = ARMSection;
                अवरोध;
            }
            शेष:
                page_आकार = ARMSection;
                अवरोध;
            }
            i += (1 << page_Bits_For_आकार(page_आकार));
            अगर (i < (1 << page_Bits_For_आकार(page_आकार))) {
                अवरोध; /* overflowed */
            }
        }
        म_लिखो("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        जबतक (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            अगर (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                म_लिखो("pt_%p_%04lu {\n", pde, PD_सूचकांक(i));
                arm32_cap_pt_print_slots(pt);
                म_लिखो("}\n"); /* pt */
            }
            i += (1 << page_Bits_For_आकार(ARMSection));
            अगर (i < (1 << page_Bits_For_आकार(ARMSection))) {
                अवरोध; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
अटल व्योम cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    चयन (pde_ptr_get_pdeType(pde)) {
    हाल pde_pde_section: {
        म_लिखो("frame_%p_%04lu ", pde, PD_सूचकांक(vptr));
        cap_frame_print_attrs_pd(pde);
        अवरोध;
    }
    हाल pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        चयन (pte_ptr_get_pteType(pte)) {
        हाल pte_pte_small: {
            म_लिखो("frame_%p_%04lu ", pte, PT_सूचकांक(vptr));
            cap_frame_print_attrs_pt(pte);
            अवरोध;
        }

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        हाल pte_pte_large: {
            म_लिखो("frame_%p_%04lu ", pte, PT_सूचकांक(vptr));
            cap_frame_print_attrs_pt(pte);
            अवरोध;
        }
#पूर्ण_अगर
        शेष:
            निश्चित(0);
        }
        अवरोध;
    }
    शेष:
        निश्चित(0);
    }
}

व्योम print_ipc_बफर_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    म_लिखो("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

व्योम print_cap_arch(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        अगर (asid) {
            म_लिखो("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_सूचकांक(vptr), (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_सूचकांक(vptr));
        }
        अवरोध;
    }
    हाल cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        अगर (asid) {
            म_लिखो("%p_pd (asid: %lu)\n",
                   find_ret.pd, (दीर्घ अचिन्हित पूर्णांक)asid);
        } अन्यथा {
            म_लिखो("%p_pd\n", find_ret.pd);
        }
        अवरोध;
    }
    हाल cap_asid_control_cap: {
        /* only one in the system */
        म_लिखो("asid_control\n");
        अवरोध;
    }
    हाल cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        निश्चित(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        अवरोध;
    }
    हाल cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        निश्चित(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
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
        म_लिखो("[unknown cap %u]\n", cap_get_capType(cap));
        अवरोध;
    }
    }
}

व्योम print_object_arch(cap_t cap)
{

    चयन (cap_get_capType(cap)) {
    हाल cap_frame_cap:
    हाल cap_small_frame_cap:
    हाल cap_page_table_cap:
    हाल cap_page_directory_cap:
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
        म_लिखो("[unknown object %u]\n", cap_get_capType(cap));
        अवरोध;
    }
    }
}

अटल व्योम obj_frame_print_attrs(resolve_ret_t ret)
{
    म_लिखो("(");

    /* VM size */
    चयन (ret.frame_आकार) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल ARMSection:
        म_लिखो("2M");
        अवरोध;
    हाल ARMSuperSection:
        म_लिखो("32M");
        अवरोध;
#अन्यथा
    हाल ARMSection:
        म_लिखो("1M");
        अवरोध;
    हाल ARMSuperSection:
        म_लिखो("16M");
        अवरोध;
#पूर्ण_अगर
    हाल ARMLargePage:
        म_लिखो("64k");
        अवरोध;
    हाल ARMSmallPage:
        म_लिखो("4k");
        अवरोध;
    }

    म_लिखो(", paddr: %p)\n", (व्योम *)ret.frameBase);
}

अटल व्योम arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    जबतक (i < BIT(PT_सूचकांक_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        चयन (pte_ptr_get_pteType(pte)) {
        हाल pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
            अगर (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(page_Bits_For_आकार(ARMLargePage));
                ret.frame_आकार = ARMLargePage;
            } अन्यथा {
                ret.frame_आकार = ARMSmallPage;
            }
#अन्यथा
            ret.frame_आकार = ARMSmallPage;
#पूर्ण_अगर
            म_लिखो("frame_%p_%04lu = frame ", pte, PT_सूचकांक(i));
            obj_frame_print_attrs(ret);
            अवरोध;
        }
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        हाल pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frame_आकार = ARMLargePage;
            म_लिखो("frame_%p_%04lu = frame ", pte, PT_सूचकांक(i));
            obj_frame_print_attrs(ret);
            अवरोध;
        }
#पूर्ण_अगर
        शेष:
            ret.frame_आकार = ARMSmallPage;
        }
        i += (1 << page_Bits_For_आकार(ret.frame_आकार));
    }
}

व्योम obj_tcb_print_vtable(tcb_t *tcb)
{
    अगर (is_Valid_VTable_मूल(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        म_लिखो("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        जबतक (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            चयन (pde_ptr_get_pdeType(pde)) {
            हाल pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                अगर (pde_pde_section_ptr_get_आकार(pde)) {
                    ret.frame_आकार = ARMSuperSection;
                } अन्यथा {
                    ret.frame_आकार = ARMSection;
                }
#अन्यथा
                अगर (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(page_Bits_For_आकार(ARMSuperSection));
                    ret.frame_आकार = ARMSuperSection;
                } अन्यथा {
                    ret.frame_आकार = ARMSection;
                }
#पूर्ण_अगर
                म_लिखो("frame_%p_%04lu = frame ", pde, PD_सूचकांक(i));
                obj_frame_print_attrs(ret);
                अवरोध;
            }

            हाल pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                म_लिखो("pt_%p_%04lu = pt\n", pde, PD_सूचकांक(i));
                arm32_obj_pt_print_slots(pt);
                ret.frame_आकार = ARMSection;
                अवरोध;
            }
            शेष:
                ret.frame_आकार = ARMSection;
                अवरोध;
            }
            i += (1 << page_Bits_For_आकार(ret.frame_आकार));
            अगर (i < (1 << page_Bits_For_आकार(ret.frame_आकार))) {
                अवरोध; /* overflowed */
            }
        }
    }
}

#पूर्ण_अगर /* CONFIG_PRINTING */

व्योम debug_capDL(व्योम)
{
    म_लिखो("arch aarch32\n");
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
