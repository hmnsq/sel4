/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#அந்தர்பாவ <arch/machine/capdl.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_INDEX(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS))
#define PD_INDEX(vptr)  (vptr >> (PAGE_BITS + PT_INDEX_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    திரும்பு tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

நிலையான வெறுமை obj_frame_print_attrs(resolve_ret_t ret);
நிலையான வெறுமை cap_frame_print_attrs_pt(pte_t *pte);
நிலையான வெறுமை cap_frame_print_attrs_pd(pde_t *pde);
நிலையான வெறுமை cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
நிலையான வெறுமை arm32_obj_pt_print_slots(pte_t *pt);
நிலையான வெறுமை arm32_cap_pt_print_slots(pte_t *pt);

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
நிலையான வெறுமை cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    தேர்வு (pte_ptr_get_pteType(pte)) {
    நிலை pte_pte_small:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#இல்லையெனில்
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        நிறுத்து;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        நிறுத்து;
#endif
    இயல்பு:
        ப்ரதிஜ்ஞா(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

நிலையான வெறுமை cap_frame_print_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#இல்லையெனில்
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

நிலையான வெறுமை cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    printf("(");

    /* rights */
    தேர்வு (AP) {
    நிலை 0b00:
    நிலை 0b01:
        நிறுத்து;
    நிலை 0b10:
        printf("R");
        நிறுத்து;
    நிலை 0b11:
        printf("RW");
    இயல்பு:
        நிறுத்து;
    }

    எனில் (!XN) {
        printf("X");
    }

    எனில் (!TEX) {
        printf(", uncached");
    }

    printf(")\n");
}

நிலையான வெறுமை arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_size_t page_size;
    word_t i = 0;
    போது (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        தேர்வு (pte_ptr_get_pteType(pte)) {
        நிலை pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            எனில் (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_size = ARMLargePage;
            } இல்லையெனில் {
                page_size = ARMSmallPage;
            }
#இல்லையெனில்
            page_size = ARMSmallPage;
#endif
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            நிறுத்து;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        நிலை pte_pte_large: {
            page_size = ARMLargePage;
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            நிறுத்து;
        }
#endif
        இயல்பு:
            page_size = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(page_size));
    }
}

வெறுமை obj_vtable_print_slots(tcb_t *tcb)
{
    எனில் (is_Valid_VTable_வேர்(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_size_t page_size;
        printf("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        போது (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            தேர்வு (pde_ptr_get_pdeType(pde)) {
            நிலை pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                எனில் (pde_pde_section_ptr_get_size(pde)) {
                    page_size = ARMSuperSection;
                } இல்லையெனில் {
                    page_size = ARMSection;
                }
#இல்லையெனில்
                எனில் (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_size = ARMSuperSection;
                } இல்லையெனில் {
                    page_size = ARMSection;
                }
#endif
                printf("0x%lx: frame_%p_%04lu ", PD_INDEX(i), pde, PD_INDEX(i));
                cap_frame_print_attrs_pd(pde);
                நிறுத்து;
            }

            நிலை pde_pde_coarse: {
                printf("0x%lx: pt_%p_%04lu\n", PD_INDEX(i), pde, PD_INDEX(i));
                page_size = ARMSection;
                நிறுத்து;
            }
            இயல்பு:
                page_size = ARMSection;
                நிறுத்து;
            }
            i += (1 << pageBitsForSize(page_size));
            எனில் (i < (1 << pageBitsForSize(page_size))) {
                நிறுத்து; /* overflowed */
            }
        }
        printf("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        போது (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            எனில் (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu {\n", pde, PD_INDEX(i));
                arm32_cap_pt_print_slots(pt);
                printf("}\n"); /* pt */
            }
            i += (1 << pageBitsForSize(ARMSection));
            எனில் (i < (1 << pageBitsForSize(ARMSection))) {
                நிறுத்து; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
நிலையான வெறுமை cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    தேர்வு (pde_ptr_get_pdeType(pde)) {
    நிலை pde_pde_section: {
        printf("frame_%p_%04lu ", pde, PD_INDEX(vptr));
        cap_frame_print_attrs_pd(pde);
        நிறுத்து;
    }
    நிலை pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        தேர்வு (pte_ptr_get_pteType(pte)) {
        நிலை pte_pte_small: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            நிறுத்து;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        நிலை pte_pte_large: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            நிறுத்து;
        }
#endif
        இயல்பு:
            ப்ரதிஜ்ஞா(0);
        }
        நிறுத்து;
    }
    இயல்பு:
        ப்ரதிஜ்ஞா(0);
    }
}

வெறுமை print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

வெறுமை print_cap_arch(cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        எனில் (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr), (long unsigned முழு)asid);
        } இல்லையெனில் {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr));
        }
        நிறுத்து;
    }
    நிலை cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        எனில் (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.pd, (long unsigned முழு)asid);
        } இல்லையெனில் {
            printf("%p_pd\n", find_ret.pd);
        }
        நிறுத்து;
    }
    நிலை cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        நிறுத்து;
    }
    நிலை cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        ப்ரதிஜ்ஞா(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        நிறுத்து;
    }
    நிலை cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        ப்ரதிஜ்ஞா(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        நிறுத்து;
    }
    நிலை cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (வெறுமை *)cap_asid_pool_cap_get_capASIDPool(cap));
        நிறுத்து;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap: {
        printf("%p_vcpu\n", (வெறுமை *)cap_vcpu_cap_get_capVCPUPtr(cap));
        நிறுத்து;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    நிலை cap_io_space_cap: {
        printf("%p_io_space\n", (வெறுமை *)cap_io_space_cap_get_capModuleID(cap));
        நிறுத்து;
    }
#endif
    இயல்பு: {
        printf("[unknown cap %u]\n", cap_get_capType(cap));
        நிறுத்து;
    }
    }
}

வெறுமை print_object_arch(cap_t cap)
{

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_frame_cap:
    நிலை cap_small_frame_cap:
    நிலை cap_page_table_cap:
    நிலை cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        நிறுத்து;

    நிலை cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (வெறுமை *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        நிறுத்து;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (வெறுமை *)cap_vcpu_cap_get_capVCPUPtr(cap));
        நிறுத்து;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    நிலை cap_io_space_cap: {
        printf("%p_io_space = io_space ", (வெறுமை *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        நிறுத்து;
    }
#endif
    இயல்பு: {
        printf("[unknown object %u]\n", cap_get_capType(cap));
        நிறுத்து;
    }
    }
}

நிலையான வெறுமை obj_frame_print_attrs(resolve_ret_t ret)
{
    printf("(");

    /* VM size */
    தேர்வு (ret.frameSize) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை ARMSection:
        printf("2M");
        நிறுத்து;
    நிலை ARMSuperSection:
        printf("32M");
        நிறுத்து;
#இல்லையெனில்
    நிலை ARMSection:
        printf("1M");
        நிறுத்து;
    நிலை ARMSuperSection:
        printf("16M");
        நிறுத்து;
#endif
    நிலை ARMLargePage:
        printf("64k");
        நிறுத்து;
    நிலை ARMSmallPage:
        printf("4k");
        நிறுத்து;
    }

    printf(", paddr: %p)\n", (வெறுமை *)ret.frameBase);
}

நிலையான வெறுமை arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    போது (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        தேர்வு (pte_ptr_get_pteType(pte)) {
        நிலை pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            எனில் (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } இல்லையெனில் {
                ret.frameSize = ARMSmallPage;
            }
#இல்லையெனில்
            ret.frameSize = ARMSmallPage;
#endif
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            நிறுத்து;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        நிலை pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            நிறுத்து;
        }
#endif
        இயல்பு:
            ret.frameSize = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(ret.frameSize));
    }
}

வெறுமை obj_tcb_print_vtable(tcb_t *tcb)
{
    எனில் (is_Valid_VTable_வேர்(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        printf("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        போது (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            தேர்வு (pde_ptr_get_pdeType(pde)) {
            நிலை pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                எனில் (pde_pde_section_ptr_get_size(pde)) {
                    ret.frameSize = ARMSuperSection;
                } இல்லையெனில் {
                    ret.frameSize = ARMSection;
                }
#இல்லையெனில்
                எனில் (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
                    ret.frameSize = ARMSuperSection;
                } இல்லையெனில் {
                    ret.frameSize = ARMSection;
                }
#endif
                printf("frame_%p_%04lu = frame ", pde, PD_INDEX(i));
                obj_frame_print_attrs(ret);
                நிறுத்து;
            }

            நிலை pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu = pt\n", pde, PD_INDEX(i));
                arm32_obj_pt_print_slots(pt);
                ret.frameSize = ARMSection;
                நிறுத்து;
            }
            இயல்பு:
                ret.frameSize = ARMSection;
                நிறுத்து;
            }
            i += (1 << pageBitsForSize(ret.frameSize));
            எனில் (i < (1 << pageBitsForSize(ret.frameSize))) {
                நிறுத்து; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

வெறுமை debug_capDL(வெறுமை)
{
    printf("arch aarch32\n");
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
