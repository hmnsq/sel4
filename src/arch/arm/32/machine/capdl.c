/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD

#अन्तर्भाव <arch/machine/capdl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>

#परिभाषय MAX_UL          0xffffffff
#परिभाषय PT_सूचकांक(vptr)  ((vptr >> PAGE_द्वयङ्काः) & आवरणम्(PT_सूचकांक_द्वयङ्काः))
#परिभाषय PD_सूचकांक(vptr)  (vptr >> (PAGE_द्वयङ्काः + PT_सूचकांक_द्वयङ्काः))

word_t get_tcb_sp(tcb_t *tcb)
{
    प्रत्यागम tcb->tcbArch.tcbContext.registers[SP];
}

#यदि_परिभाषितम् CONFIG_PRINTING

स्थिर शून्य obj_frame_print_attrs(resolve_ret_t ret);
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *pte);
स्थिर शून्य cap_frame_print_attrs_pd(pde_t *pde);
स्थिर शून्य cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
स्थिर शून्य arm32_obj_pt_print_slots(pte_t *pt);
स्थिर शून्य arm32_cap_pt_print_slots(pte_t *pt);

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
स्थिर शून्य cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    विकल्प (pte_ptr_get_pteType(pte)) {
    पक्ष pte_pte_small:
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#अन्यथा
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#यदि_अन्तः
        XN = pte_pte_small_ptr_get_XN(pte);
        भङ्ग;

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        भङ्ग;
#यदि_अन्तः
    स्वभाव:
        प्रतिज्ञा(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

स्थिर शून्य cap_frame_print_attrs_pd(pde_t *pde)
{
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#अन्यथा
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#यदि_अन्तः
}

स्थिर शून्य cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    प्रारूप_लिख("(");

    /* rights */
    विकल्प (AP) {
    पक्ष 0b00:
    पक्ष 0b01:
        भङ्ग;
    पक्ष 0b10:
        प्रारूप_लिख("R");
        भङ्ग;
    पक्ष 0b11:
        प्रारूप_लिख("RW");
    स्वभाव:
        भङ्ग;
    }

    यदि (!XN) {
        प्रारूप_लिख("X");
    }

    यदि (!TEX) {
        प्रारूप_लिख(", uncached");
    }

    प्रारूप_लिख(")\n");
}

स्थिर शून्य arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_आकार_t page_आकार;
    word_t i = 0;
    यावत् (i < BIT(PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        विकल्प (pte_ptr_get_pteType(pte)) {
        पक्ष pte_pte_small: {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
            यदि (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_आकार = ARMLargePage;
            } अन्यथा {
                page_आकार = ARMSmallPage;
            }
#अन्यथा
            page_आकार = ARMSmallPage;
#यदि_अन्तः
            प्रारूप_लिख("0x%lx: frame_%p_%04lu ", PT_सूचकांक(i), pte, PT_सूचकांक(i));
            cap_frame_print_attrs_pt(pte);
            भङ्ग;
        }
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        पक्ष pte_pte_large: {
            page_आकार = ARMLargePage;
            प्रारूप_लिख("0x%lx: frame_%p_%04lu ", PT_सूचकांक(i), pte, PT_सूचकांक(i));
            cap_frame_print_attrs_pt(pte);
            भङ्ग;
        }
#यदि_अन्तः
        स्वभाव:
            page_आकार = ARMSmallPage;
        }
        i += (1 << page_द्वयङ्काः_For_आकार(page_आकार));
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    यदि (is_Valid_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        सङ्कलय_गन्तव्यम्_आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_सम्बन्धि_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_आकार_t page_आकार;
        प्रारूप_लिख("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        यावत् (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            विकल्प (pde_ptr_get_pdeType(pde)) {
            पक्ष pde_pde_section: {
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                यदि (pde_pde_section_ptr_get_आकार(pde)) {
                    page_आकार = ARMSuperSection;
                } अन्यथा {
                    page_आकार = ARMSection;
                }
#अन्यथा
                यदि (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_आकार = ARMSuperSection;
                } अन्यथा {
                    page_आकार = ARMSection;
                }
#यदि_अन्तः
                प्रारूप_लिख("0x%lx: frame_%p_%04lu ", PD_सूचकांक(i), pde, PD_सूचकांक(i));
                cap_frame_print_attrs_pd(pde);
                भङ्ग;
            }

            पक्ष pde_pde_coarse: {
                प्रारूप_लिख("0x%lx: pt_%p_%04lu\n", PD_सूचकांक(i), pde, PD_सूचकांक(i));
                page_आकार = ARMSection;
                भङ्ग;
            }
            स्वभाव:
                page_आकार = ARMSection;
                भङ्ग;
            }
            i += (1 << page_द्वयङ्काः_For_आकार(page_आकार));
            यदि (i < (1 << page_द्वयङ्काः_For_आकार(page_आकार))) {
                भङ्ग; /* overflowed */
            }
        }
        प्रारूप_लिख("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        यावत् (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            यदि (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_स्थानाङ्कः(pde));
                प्रारूप_लिख("pt_%p_%04lu {\n", pde, PD_सूचकांक(i));
                arm32_cap_pt_print_slots(pt);
                प्रारूप_लिख("}\n"); /* pt */
            }
            i += (1 << page_द्वयङ्काः_For_आकार(ARMSection));
            यदि (i < (1 << page_द्वयङ्काः_For_आकार(ARMSection))) {
                भङ्ग; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
स्थिर शून्य cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    विकल्प (pde_ptr_get_pdeType(pde)) {
    पक्ष pde_pde_section: {
        प्रारूप_लिख("frame_%p_%04lu ", pde, PD_सूचकांक(vptr));
        cap_frame_print_attrs_pd(pde);
        भङ्ग;
    }
    पक्ष pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_स्थानाङ्कः(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        विकल्प (pte_ptr_get_pteType(pte)) {
        पक्ष pte_pte_small: {
            प्रारूप_लिख("frame_%p_%04lu ", pte, PT_सूचकांक(vptr));
            cap_frame_print_attrs_pt(pte);
            भङ्ग;
        }

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        पक्ष pte_pte_large: {
            प्रारूप_लिख("frame_%p_%04lu ", pte, PT_सूचकांक(vptr));
            cap_frame_print_attrs_pt(pte);
            भङ्ग;
        }
#यदि_अन्तः
        स्वभाव:
            प्रतिज्ञा(0);
        }
        भङ्ग;
    }
    स्वभाव:
        प्रतिज्ञा(0);
    }
}

शून्य print_ipc_बफर_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    प्रारूप_लिख("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

शून्य print_cap_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_सारणी_cap: {
        asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap);
        यदि (asid) {
            प्रारूप_लिख("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_सूचकांक(vptr), (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_सूचकांक(vptr));
        }
        भङ्ग;
    }
    पक्ष cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        यदि (asid) {
            प्रारूप_लिख("%p_pd (asid: %lu)\n",
                   find_ret.pd, (दीर्घ अचिह्नित पूर्णाङ्क)asid);
        } अन्यथा {
            प्रारूप_लिख("%p_pd\n", find_ret.pd);
        }
        भङ्ग;
    }
    पक्ष cap_asid_control_cap: {
        /* only one in the system */
        प्रारूप_लिख("asid_control\n");
        भङ्ग;
    }
    पक्ष cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        प्रतिज्ञा(find_ret.स्थितिः == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        भङ्ग;
    }
    पक्ष cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        प्रतिज्ञा(find_ret.स्थितिः == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
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
        प्रारूप_लिख("[unknown cap %u]\n", cap_get_capType(cap));
        भङ्ग;
    }
    }
}

शून्य print_object_arch(cap_t cap)
{

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_frame_cap:
    पक्ष cap_small_frame_cap:
    पक्ष cap_page_सारणी_cap:
    पक्ष cap_page_directory_cap:
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
        प्रारूप_लिख("[unknown object %u]\n", cap_get_capType(cap));
        भङ्ग;
    }
    }
}

स्थिर शून्य obj_frame_print_attrs(resolve_ret_t ret)
{
    प्रारूप_लिख("(");

    /* VM size */
    विकल्प (ret.frame_आकार) {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष ARMSection:
        प्रारूप_लिख("2M");
        भङ्ग;
    पक्ष ARMSuperSection:
        प्रारूप_लिख("32M");
        भङ्ग;
#अन्यथा
    पक्ष ARMSection:
        प्रारूप_लिख("1M");
        भङ्ग;
    पक्ष ARMSuperSection:
        प्रारूप_लिख("16M");
        भङ्ग;
#यदि_अन्तः
    पक्ष ARMLargePage:
        प्रारूप_लिख("64k");
        भङ्ग;
    पक्ष ARMSmallPage:
        प्रारूप_लिख("4k");
        भङ्ग;
    }

    प्रारूप_लिख(", paddr: %p)\n", (शून्य *)ret.frame_आधारः);
}

स्थिर शून्य arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    यावत् (i < BIT(PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        विकल्प (pte_ptr_get_pteType(pte)) {
        पक्ष pte_pte_small: {
            ret.frame_आधारः = pte_pte_small_ptr_get_स्थानाङ्कः(pte);
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
            यदि (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frame_आधारः &= ~आवरणम्(page_द्वयङ्काः_For_आकार(ARMLargePage));
                ret.frame_आकार = ARMLargePage;
            } अन्यथा {
                ret.frame_आकार = ARMSmallPage;
            }
#अन्यथा
            ret.frame_आकार = ARMSmallPage;
#यदि_अन्तः
            प्रारूप_लिख("frame_%p_%04lu = frame ", pte, PT_सूचकांक(i));
            obj_frame_print_attrs(ret);
            भङ्ग;
        }
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        पक्ष pte_pte_large: {
            ret.frame_आधारः = pte_pte_large_ptr_get_स्थानाङ्कः(pte);
            ret.frame_आकार = ARMLargePage;
            प्रारूप_लिख("frame_%p_%04lu = frame ", pte, PT_सूचकांक(i));
            obj_frame_print_attrs(ret);
            भङ्ग;
        }
#यदि_अन्तः
        स्वभाव:
            ret.frame_आकार = ARMSmallPage;
        }
        i += (1 << page_द्वयङ्काः_For_आकार(ret.frame_आकार));
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    यदि (is_Valid_VTable_मूलम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        सङ्कलय_गन्तव्यम्_आलोकितम्(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_सम्बन्धि_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        प्रारूप_लिख("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        यावत् (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            विकल्प (pde_ptr_get_pdeType(pde)) {
            पक्ष pde_pde_section: {
                ret.frame_आधारः = pde_pde_section_ptr_get_स्थानाङ्कः(pde);
#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                यदि (pde_pde_section_ptr_get_आकार(pde)) {
                    ret.frame_आकार = ARMSuperSection;
                } अन्यथा {
                    ret.frame_आकार = ARMSection;
                }
#अन्यथा
                यदि (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frame_आधारः &= ~आवरणम्(page_द्वयङ्काः_For_आकार(ARMSuperSection));
                    ret.frame_आकार = ARMSuperSection;
                } अन्यथा {
                    ret.frame_आकार = ARMSection;
                }
#यदि_अन्तः
                प्रारूप_लिख("frame_%p_%04lu = frame ", pde, PD_सूचकांक(i));
                obj_frame_print_attrs(ret);
                भङ्ग;
            }

            पक्ष pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_स्थानाङ्कः(pde));
                प्रारूप_लिख("pt_%p_%04lu = pt\n", pde, PD_सूचकांक(i));
                arm32_obj_pt_print_slots(pt);
                ret.frame_आकार = ARMSection;
                भङ्ग;
            }
            स्वभाव:
                ret.frame_आकार = ARMSection;
                भङ्ग;
            }
            i += (1 << page_द्वयङ्काः_For_आकार(ret.frame_आकार));
            यदि (i < (1 << page_द्वयङ्काः_For_आकार(ret.frame_आकार))) {
                भङ्ग; /* overflowed */
            }
        }
    }
}

#यदि_अन्तः /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    प्रारूप_लिख("arch aarch32\n");
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
