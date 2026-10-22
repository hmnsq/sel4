/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_DEBUG_BUILD

#अन्तर्भाव <arch/machine/capdl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_INDEX(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS))
#define PD_INDEX(vptr)  (vptr >> (PAGE_BITS + PT_INDEX_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    फर्काऊ tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

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
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#अन्यथा
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        रोक;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        रोक;
#endif
    स्वभाव:
        प्रतिज्ञा(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

स्थिर शून्य cap_frame_print_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#अन्यथा
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

स्थिर शून्य cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    म_लिखो("(");

    /* rights */
    विकल्प (AP) {
    पक्ष 0b00:
    पक्ष 0b01:
        रोक;
    पक्ष 0b10:
        म_लिखो("R");
        रोक;
    पक्ष 0b11:
        म_लिखो("RW");
    स्वभाव:
        रोक;
    }

    यदि (!XN) {
        म_लिखो("X");
    }

    यदि (!TEX) {
        म_लिखो(", uncached");
    }

    म_लिखो(")\n");
}

स्थिर शून्य arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_size_t page_size;
    word_t i = 0;
    जबसम्म (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        विकल्प (pte_ptr_get_pteType(pte)) {
        पक्ष pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            यदि (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_size = ARMLargePage;
            } अन्यथा {
                page_size = ARMSmallPage;
            }
#अन्यथा
            page_size = ARMSmallPage;
#endif
            म_लिखो("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            रोक;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        पक्ष pte_pte_large: {
            page_size = ARMLargePage;
            म_लिखो("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            रोक;
        }
#endif
        स्वभाव:
            page_size = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(page_size));
    }
}

शून्य obj_vtable_print_slots(tcb_t *tcb)
{
    यदि (is_Valid_VTable_जरा(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_size_t page_size;
        म_लिखो("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        जबसम्म (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            विकल्प (pde_ptr_get_pdeType(pde)) {
            पक्ष pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                यदि (pde_pde_section_ptr_get_size(pde)) {
                    page_size = ARMSuperSection;
                } अन्यथा {
                    page_size = ARMSection;
                }
#अन्यथा
                यदि (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_size = ARMSuperSection;
                } अन्यथा {
                    page_size = ARMSection;
                }
#endif
                म_लिखो("0x%lx: frame_%p_%04lu ", PD_INDEX(i), pde, PD_INDEX(i));
                cap_frame_print_attrs_pd(pde);
                रोक;
            }

            पक्ष pde_pde_coarse: {
                म_लिखो("0x%lx: pt_%p_%04lu\n", PD_INDEX(i), pde, PD_INDEX(i));
                page_size = ARMSection;
                रोक;
            }
            स्वभाव:
                page_size = ARMSection;
                रोक;
            }
            i += (1 << pageBitsForSize(page_size));
            यदि (i < (1 << pageBitsForSize(page_size))) {
                रोक; /* overflowed */
            }
        }
        म_लिखो("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        जबसम्म (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            यदि (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                म_लिखो("pt_%p_%04lu {\n", pde, PD_INDEX(i));
                arm32_cap_pt_print_slots(pt);
                म_लिखो("}\n"); /* pt */
            }
            i += (1 << pageBitsForSize(ARMSection));
            यदि (i < (1 << pageBitsForSize(ARMSection))) {
                रोक; /* overflowed */
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
        म_लिखो("frame_%p_%04lu ", pde, PD_INDEX(vptr));
        cap_frame_print_attrs_pd(pde);
        रोक;
    }
    पक्ष pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        विकल्प (pte_ptr_get_pteType(pte)) {
        पक्ष pte_pte_small: {
            म_लिखो("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            रोक;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        पक्ष pte_pte_large: {
            म_लिखो("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            रोक;
        }
#endif
        स्वभाव:
            प्रतिज्ञा(0);
        }
        रोक;
    }
    स्वभाव:
        प्रतिज्ञा(0);
    }
}

शून्य print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    म_लिखो("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

शून्य print_cap_arch(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        यदि (asid) {
            म_लिखो("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr), (दीर्घ अचिन्हित पूर्णाङ्क)asid);
        } अन्यथा {
            म_लिखो("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr));
        }
        रोक;
    }
    पक्ष cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        यदि (asid) {
            म_लिखो("%p_pd (asid: %lu)\n",
                   find_ret.pd, (दीर्घ अचिन्हित पूर्णाङ्क)asid);
        } अन्यथा {
            म_लिखो("%p_pd\n", find_ret.pd);
        }
        रोक;
    }
    पक्ष cap_asid_control_cap: {
        /* only one in the system */
        म_लिखो("asid_control\n");
        रोक;
    }
    पक्ष cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        प्रतिज्ञा(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        रोक;
    }
    पक्ष cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        प्रतिज्ञा(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        रोक;
    }
    पक्ष cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool\n", (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        रोक;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap: {
        म_लिखो("%p_vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        रोक;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap: {
        म_लिखो("%p_io_space\n", (शून्य *)cap_io_space_cap_get_capModuleID(cap));
        रोक;
    }
#endif
    स्वभाव: {
        म_लिखो("[unknown cap %u]\n", cap_get_capType(cap));
        रोक;
    }
    }
}

शून्य print_object_arch(cap_t cap)
{

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_frame_cap:
    पक्ष cap_small_frame_cap:
    पक्ष cap_page_table_cap:
    पक्ष cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        रोक;

    पक्ष cap_asid_pool_cap: {
        म_लिखो("%p_asid_pool = asid_pool ",
               (शून्य *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        रोक;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap: {
        म_लिखो("%p_vcpu = vcpu\n", (शून्य *)cap_vcpu_cap_get_capVCPUPtr(cap));
        रोक;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap: {
        म_लिखो("%p_io_space = io_space ", (शून्य *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        रोक;
    }
#endif
    स्वभाव: {
        म_लिखो("[unknown object %u]\n", cap_get_capType(cap));
        रोक;
    }
    }
}

स्थिर शून्य obj_frame_print_attrs(resolve_ret_t ret)
{
    म_लिखो("(");

    /* VM size */
    विकल्प (ret.frameSize) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष ARMSection:
        म_लिखो("2M");
        रोक;
    पक्ष ARMSuperSection:
        म_लिखो("32M");
        रोक;
#अन्यथा
    पक्ष ARMSection:
        म_लिखो("1M");
        रोक;
    पक्ष ARMSuperSection:
        म_लिखो("16M");
        रोक;
#endif
    पक्ष ARMLargePage:
        म_लिखो("64k");
        रोक;
    पक्ष ARMSmallPage:
        म_लिखो("4k");
        रोक;
    }

    म_लिखो(", paddr: %p)\n", (शून्य *)ret.frameBase);
}

स्थिर शून्य arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    जबसम्म (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        विकल्प (pte_ptr_get_pteType(pte)) {
        पक्ष pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            यदि (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } अन्यथा {
                ret.frameSize = ARMSmallPage;
            }
#अन्यथा
            ret.frameSize = ARMSmallPage;
#endif
            म_लिखो("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            रोक;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        पक्ष pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            म_लिखो("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            रोक;
        }
#endif
        स्वभाव:
            ret.frameSize = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(ret.frameSize));
    }
}

शून्य obj_tcb_print_vtable(tcb_t *tcb)
{
    यदि (is_Valid_VTable_जरा(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        म_लिखो("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        जबसम्म (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            विकल्प (pde_ptr_get_pdeType(pde)) {
            पक्ष pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                यदि (pde_pde_section_ptr_get_size(pde)) {
                    ret.frameSize = ARMSuperSection;
                } अन्यथा {
                    ret.frameSize = ARMSection;
                }
#अन्यथा
                यदि (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
                    ret.frameSize = ARMSuperSection;
                } अन्यथा {
                    ret.frameSize = ARMSection;
                }
#endif
                म_लिखो("frame_%p_%04lu = frame ", pde, PD_INDEX(i));
                obj_frame_print_attrs(ret);
                रोक;
            }

            पक्ष pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                म_लिखो("pt_%p_%04lu = pt\n", pde, PD_INDEX(i));
                arm32_obj_pt_print_slots(pt);
                ret.frameSize = ARMSection;
                रोक;
            }
            स्वभाव:
                ret.frameSize = ARMSection;
                रोक;
            }
            i += (1 << pageBitsForSize(ret.frameSize));
            यदि (i < (1 << pageBitsForSize(ret.frameSize))) {
                रोक; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

शून्य debug_capDL(शून्य)
{
    म_लिखो("arch aarch32\n");
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
