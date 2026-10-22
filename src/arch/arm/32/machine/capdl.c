/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ଅନ୍ତର୍ଭାଵ <arch/machine/capdl.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_INDEX(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS))
#define PD_INDEX(vptr)  (vptr >> (PAGE_BITS + PT_INDEX_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    ଫେରନ୍ତୁ tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

ସ୍ଥିର ଶୂନ୍ୟ obj_frame_print_attrs(resolve_ret_t ret);
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_pt(pte_t *pte);
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_pd(pde_t *pde);
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
ସ୍ଥିର ଶୂନ୍ୟ arm32_obj_pt_print_slots(pte_t *pt);
ସ୍ଥିର ଶୂନ୍ୟ arm32_cap_pt_print_slots(pte_t *pt);

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
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    ବାଛ (pte_ptr_get_pteType(pte)) {
    ସ୍ଥିତି pte_pte_small:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#ଅନ୍ୟଥା
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        ଭାଙ୍ଗ;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        ଭାଙ୍ଗ;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ପ୍ରତିଜ୍ଞା(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#ଅନ୍ୟଥା
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    printf("(");

    /* rights */
    ବାଛ (AP) {
    ସ୍ଥିତି 0b00:
    ସ୍ଥିତି 0b01:
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 0b10:
        printf("R");
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 0b11:
        printf("RW");
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    ଯଦି (!XN) {
        printf("X");
    }

    ଯଦି (!TEX) {
        printf(", uncached");
    }

    printf(")\n");
}

ସ୍ଥିର ଶୂନ୍ୟ arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_size_t page_size;
    word_t i = 0;
    ଯେତେବେଳେ (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ବାଛ (pte_ptr_get_pteType(pte)) {
        ସ୍ଥିତି pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            ଯଦି (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_size = ARMLargePage;
            } ଅନ୍ୟଥା {
                page_size = ARMSmallPage;
            }
#ଅନ୍ୟଥା
            page_size = ARMSmallPage;
#endif
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            ଭାଙ୍ଗ;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ସ୍ଥିତି pte_pte_large: {
            page_size = ARMLargePage;
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            ଭାଙ୍ଗ;
        }
#endif
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            page_size = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(page_size));
    }
}

ଶୂନ୍ୟ obj_vtable_print_slots(tcb_t *tcb)
{
    ଯଦି (is_Valid_VTable_ମୂଳ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_size_t page_size;
        printf("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        ଯେତେବେଳେ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ବାଛ (pde_ptr_get_pdeType(pde)) {
            ସ୍ଥିତି pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                ଯଦି (pde_pde_section_ptr_get_size(pde)) {
                    page_size = ARMSuperSection;
                } ଅନ୍ୟଥା {
                    page_size = ARMSection;
                }
#ଅନ୍ୟଥା
                ଯଦି (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_size = ARMSuperSection;
                } ଅନ୍ୟଥା {
                    page_size = ARMSection;
                }
#endif
                printf("0x%lx: frame_%p_%04lu ", PD_INDEX(i), pde, PD_INDEX(i));
                cap_frame_print_attrs_pd(pde);
                ଭାଙ୍ଗ;
            }

            ସ୍ଥିତି pde_pde_coarse: {
                printf("0x%lx: pt_%p_%04lu\n", PD_INDEX(i), pde, PD_INDEX(i));
                page_size = ARMSection;
                ଭାଙ୍ଗ;
            }
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                page_size = ARMSection;
                ଭାଙ୍ଗ;
            }
            i += (1 << pageBitsForSize(page_size));
            ଯଦି (i < (1 << pageBitsForSize(page_size))) {
                ଭାଙ୍ଗ; /* overflowed */
            }
        }
        printf("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ଯେତେବେଳେ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ଯଦି (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu {\n", pde, PD_INDEX(i));
                arm32_cap_pt_print_slots(pt);
                printf("}\n"); /* pt */
            }
            i += (1 << pageBitsForSize(ARMSection));
            ଯଦି (i < (1 << pageBitsForSize(ARMSection))) {
                ଭାଙ୍ଗ; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ସ୍ଥିର ଶୂନ୍ୟ cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    ବାଛ (pde_ptr_get_pdeType(pde)) {
    ସ୍ଥିତି pde_pde_section: {
        printf("frame_%p_%04lu ", pde, PD_INDEX(vptr));
        cap_frame_print_attrs_pd(pde);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        ବାଛ (pte_ptr_get_pteType(pte)) {
        ସ୍ଥିତି pte_pte_small: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            ଭାଙ୍ଗ;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ସ୍ଥିତି pte_pte_large: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            ଭାଙ୍ଗ;
        }
#endif
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ପ୍ରତିଜ୍ଞା(0);
        }
        ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ପ୍ରତିଜ୍ଞା(0);
    }
}

ଶୂନ୍ୟ print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

ଶୂନ୍ୟ print_cap_arch(cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        ଯଦି (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr), (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)asid);
        } ଅନ୍ୟଥା {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr));
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        ଯଦି (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.pd, (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)asid);
        } ଅନ୍ୟଥା {
            printf("%p_pd\n", find_ret.pd);
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        ପ୍ରତିଜ୍ଞା(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        ପ୍ରତିଜ୍ଞା(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ଶୂନ୍ୟ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ଭାଙ୍ଗ;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap: {
        printf("%p_vcpu\n", (ଶୂନ୍ୟ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ଭାଙ୍ଗ;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି cap_io_space_cap: {
        printf("%p_io_space\n", (ଶୂନ୍ୟ *)cap_io_space_cap_get_capModuleID(cap));
        ଭାଙ୍ଗ;
    }
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
        printf("[unknown cap %u]\n", cap_get_capType(cap));
        ଭାଙ୍ଗ;
    }
    }
}

ଶୂନ୍ୟ print_object_arch(cap_t cap)
{

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_frame_cap:
    ସ୍ଥିତି cap_small_frame_cap:
    ସ୍ଥିତି cap_page_table_cap:
    ସ୍ଥିତି cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ଶୂନ୍ୟ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (ଶୂନ୍ୟ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ଭାଙ୍ଗ;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି cap_io_space_cap: {
        printf("%p_io_space = io_space ", (ଶୂନ୍ୟ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
        printf("[unknown object %u]\n", cap_get_capType(cap));
        ଭାଙ୍ଗ;
    }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ obj_frame_print_attrs(resolve_ret_t ret)
{
    printf("(");

    /* VM size */
    ବାଛ (ret.frameSize) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି ARMSection:
        printf("2M");
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ARMSuperSection:
        printf("32M");
        ଭାଙ୍ଗ;
#ଅନ୍ୟଥା
    ସ୍ଥିତି ARMSection:
        printf("1M");
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ARMSuperSection:
        printf("16M");
        ଭାଙ୍ଗ;
#endif
    ସ୍ଥିତି ARMLargePage:
        printf("64k");
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ARMSmallPage:
        printf("4k");
        ଭାଙ୍ଗ;
    }

    printf(", paddr: %p)\n", (ଶୂନ୍ୟ *)ret.frameBase);
}

ସ୍ଥିର ଶୂନ୍ୟ arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    ଯେତେବେଳେ (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ବାଛ (pte_ptr_get_pteType(pte)) {
        ସ୍ଥିତି pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            ଯଦି (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } ଅନ୍ୟଥା {
                ret.frameSize = ARMSmallPage;
            }
#ଅନ୍ୟଥା
            ret.frameSize = ARMSmallPage;
#endif
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            ଭାଙ୍ଗ;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ସ୍ଥିତି pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            ଭାଙ୍ଗ;
        }
#endif
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ret.frameSize = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(ret.frameSize));
    }
}

ଶୂନ୍ୟ obj_tcb_print_vtable(tcb_t *tcb)
{
    ଯଦି (is_Valid_VTable_ମୂଳ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        printf("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        ଯେତେବେଳେ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ବାଛ (pde_ptr_get_pdeType(pde)) {
            ସ୍ଥିତି pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                ଯଦି (pde_pde_section_ptr_get_size(pde)) {
                    ret.frameSize = ARMSuperSection;
                } ଅନ୍ୟଥା {
                    ret.frameSize = ARMSection;
                }
#ଅନ୍ୟଥା
                ଯଦି (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
                    ret.frameSize = ARMSuperSection;
                } ଅନ୍ୟଥା {
                    ret.frameSize = ARMSection;
                }
#endif
                printf("frame_%p_%04lu = frame ", pde, PD_INDEX(i));
                obj_frame_print_attrs(ret);
                ଭାଙ୍ଗ;
            }

            ସ୍ଥିତି pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu = pt\n", pde, PD_INDEX(i));
                arm32_obj_pt_print_slots(pt);
                ret.frameSize = ARMSection;
                ଭାଙ୍ଗ;
            }
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ret.frameSize = ARMSection;
                ଭାଙ୍ଗ;
            }
            i += (1 << pageBitsForSize(ret.frameSize));
            ଯଦି (i < (1 << pageBitsForSize(ret.frameSize))) {
                ଭାଙ୍ଗ; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

ଶୂନ୍ୟ debug_capDL(ଶୂନ୍ୟ)
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
