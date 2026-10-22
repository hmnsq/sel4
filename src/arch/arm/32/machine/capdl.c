/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#അന്തര്ഭാവ <arch/machine/capdl.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_INDEX(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS))
#define PD_INDEX(vptr)  (vptr >> (PAGE_BITS + PT_INDEX_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    തിരികെ tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

സ്ഥിതം ശൂന്യം obj_frame_print_attrs(resolve_ret_t ret);
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_pt(pte_t *pte);
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_pd(pde_t *pde);
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
സ്ഥിതം ശൂന്യം arm32_obj_pt_print_slots(pte_t *pt);
സ്ഥിതം ശൂന്യം arm32_cap_pt_print_slots(pte_t *pt);

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
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    തിരഞ്ഞെടുപ്പ് (pte_ptr_get_pteType(pte)) {
    അവസ്ഥ pte_pte_small:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#അല്ലെങ്കിൽ
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        നിർത്തുക;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        നിർത്തുക;
#endif
    സ്ഥിരസ്ഥിതി:
        പ്രതിജ്ഞാ(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

സ്ഥിതം ശൂന്യം cap_frame_print_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#അല്ലെങ്കിൽ
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

സ്ഥിതം ശൂന്യം cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    printf("(");

    /* rights */
    തിരഞ്ഞെടുപ്പ് (AP) {
    അവസ്ഥ 0b00:
    അവസ്ഥ 0b01:
        നിർത്തുക;
    അവസ്ഥ 0b10:
        printf("R");
        നിർത്തുക;
    അവസ്ഥ 0b11:
        printf("RW");
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }

    എങ്കിൽ (!XN) {
        printf("X");
    }

    എങ്കിൽ (!TEX) {
        printf(", uncached");
    }

    printf(")\n");
}

സ്ഥിതം ശൂന്യം arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_size_t page_size;
    word_t i = 0;
    ആകുമ്പോൾ (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        തിരഞ്ഞെടുപ്പ് (pte_ptr_get_pteType(pte)) {
        അവസ്ഥ pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            എങ്കിൽ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_size = ARMLargePage;
            } അല്ലെങ്കിൽ {
                page_size = ARMSmallPage;
            }
#അല്ലെങ്കിൽ
            page_size = ARMSmallPage;
#endif
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            നിർത്തുക;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        അവസ്ഥ pte_pte_large: {
            page_size = ARMLargePage;
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            നിർത്തുക;
        }
#endif
        സ്ഥിരസ്ഥിതി:
            page_size = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(page_size));
    }
}

ശൂന്യം obj_vtable_print_slots(tcb_t *tcb)
{
    എങ്കിൽ (is_Valid_VTable_വേര്(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_size_t page_size;
        printf("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        ആകുമ്പോൾ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            തിരഞ്ഞെടുപ്പ് (pde_ptr_get_pdeType(pde)) {
            അവസ്ഥ pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                എങ്കിൽ (pde_pde_section_ptr_get_size(pde)) {
                    page_size = ARMSuperSection;
                } അല്ലെങ്കിൽ {
                    page_size = ARMSection;
                }
#അല്ലെങ്കിൽ
                എങ്കിൽ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_size = ARMSuperSection;
                } അല്ലെങ്കിൽ {
                    page_size = ARMSection;
                }
#endif
                printf("0x%lx: frame_%p_%04lu ", PD_INDEX(i), pde, PD_INDEX(i));
                cap_frame_print_attrs_pd(pde);
                നിർത്തുക;
            }

            അവസ്ഥ pde_pde_coarse: {
                printf("0x%lx: pt_%p_%04lu\n", PD_INDEX(i), pde, PD_INDEX(i));
                page_size = ARMSection;
                നിർത്തുക;
            }
            സ്ഥിരസ്ഥിതി:
                page_size = ARMSection;
                നിർത്തുക;
            }
            i += (1 << pageBitsForSize(page_size));
            എങ്കിൽ (i < (1 << pageBitsForSize(page_size))) {
                നിർത്തുക; /* overflowed */
            }
        }
        printf("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ആകുമ്പോൾ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            എങ്കിൽ (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu {\n", pde, PD_INDEX(i));
                arm32_cap_pt_print_slots(pt);
                printf("}\n"); /* pt */
            }
            i += (1 << pageBitsForSize(ARMSection));
            എങ്കിൽ (i < (1 << pageBitsForSize(ARMSection))) {
                നിർത്തുക; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
സ്ഥിതം ശൂന്യം cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    തിരഞ്ഞെടുപ്പ് (pde_ptr_get_pdeType(pde)) {
    അവസ്ഥ pde_pde_section: {
        printf("frame_%p_%04lu ", pde, PD_INDEX(vptr));
        cap_frame_print_attrs_pd(pde);
        നിർത്തുക;
    }
    അവസ്ഥ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        തിരഞ്ഞെടുപ്പ് (pte_ptr_get_pteType(pte)) {
        അവസ്ഥ pte_pte_small: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            നിർത്തുക;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        അവസ്ഥ pte_pte_large: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            നിർത്തുക;
        }
#endif
        സ്ഥിരസ്ഥിതി:
            പ്രതിജ്ഞാ(0);
        }
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        പ്രതിജ്ഞാ(0);
    }
}

ശൂന്യം print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

ശൂന്യം print_cap_arch(cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        എങ്കിൽ (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr), (long unsigned പൂർണ്ണസംഖ്യ)asid);
        } അല്ലെങ്കിൽ {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr));
        }
        നിർത്തുക;
    }
    അവസ്ഥ cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        എങ്കിൽ (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.pd, (long unsigned പൂർണ്ണസംഖ്യ)asid);
        } അല്ലെങ്കിൽ {
            printf("%p_pd\n", find_ret.pd);
        }
        നിർത്തുക;
    }
    അവസ്ഥ cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        നിർത്തുക;
    }
    അവസ്ഥ cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        പ്രതിജ്ഞാ(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        നിർത്തുക;
    }
    അവസ്ഥ cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        പ്രതിജ്ഞാ(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        നിർത്തുക;
    }
    അവസ്ഥ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ശൂന്യം *)cap_asid_pool_cap_get_capASIDPool(cap));
        നിർത്തുക;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap: {
        printf("%p_vcpu\n", (ശൂന്യം *)cap_vcpu_cap_get_capVCPUPtr(cap));
        നിർത്തുക;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ cap_io_space_cap: {
        printf("%p_io_space\n", (ശൂന്യം *)cap_io_space_cap_get_capModuleID(cap));
        നിർത്തുക;
    }
#endif
    സ്ഥിരസ്ഥിതി: {
        printf("[unknown cap %u]\n", cap_get_capType(cap));
        നിർത്തുക;
    }
    }
}

ശൂന്യം print_object_arch(cap_t cap)
{

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_frame_cap:
    അവസ്ഥ cap_small_frame_cap:
    അവസ്ഥ cap_page_table_cap:
    അവസ്ഥ cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        നിർത്തുക;

    അവസ്ഥ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ശൂന്യം *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        നിർത്തുക;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (ശൂന്യം *)cap_vcpu_cap_get_capVCPUPtr(cap));
        നിർത്തുക;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ cap_io_space_cap: {
        printf("%p_io_space = io_space ", (ശൂന്യം *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        നിർത്തുക;
    }
#endif
    സ്ഥിരസ്ഥിതി: {
        printf("[unknown object %u]\n", cap_get_capType(cap));
        നിർത്തുക;
    }
    }
}

സ്ഥിതം ശൂന്യം obj_frame_print_attrs(resolve_ret_t ret)
{
    printf("(");

    /* VM size */
    തിരഞ്ഞെടുപ്പ് (ret.frameSize) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ ARMSection:
        printf("2M");
        നിർത്തുക;
    അവസ്ഥ ARMSuperSection:
        printf("32M");
        നിർത്തുക;
#അല്ലെങ്കിൽ
    അവസ്ഥ ARMSection:
        printf("1M");
        നിർത്തുക;
    അവസ്ഥ ARMSuperSection:
        printf("16M");
        നിർത്തുക;
#endif
    അവസ്ഥ ARMLargePage:
        printf("64k");
        നിർത്തുക;
    അവസ്ഥ ARMSmallPage:
        printf("4k");
        നിർത്തുക;
    }

    printf(", paddr: %p)\n", (ശൂന്യം *)ret.frameBase);
}

സ്ഥിതം ശൂന്യം arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    ആകുമ്പോൾ (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        തിരഞ്ഞെടുപ്പ് (pte_ptr_get_pteType(pte)) {
        അവസ്ഥ pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            എങ്കിൽ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } അല്ലെങ്കിൽ {
                ret.frameSize = ARMSmallPage;
            }
#അല്ലെങ്കിൽ
            ret.frameSize = ARMSmallPage;
#endif
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            നിർത്തുക;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        അവസ്ഥ pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            നിർത്തുക;
        }
#endif
        സ്ഥിരസ്ഥിതി:
            ret.frameSize = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(ret.frameSize));
    }
}

ശൂന്യം obj_tcb_print_vtable(tcb_t *tcb)
{
    എങ്കിൽ (is_Valid_VTable_വേര്(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        printf("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        ആകുമ്പോൾ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            തിരഞ്ഞെടുപ്പ് (pde_ptr_get_pdeType(pde)) {
            അവസ്ഥ pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                എങ്കിൽ (pde_pde_section_ptr_get_size(pde)) {
                    ret.frameSize = ARMSuperSection;
                } അല്ലെങ്കിൽ {
                    ret.frameSize = ARMSection;
                }
#അല്ലെങ്കിൽ
                എങ്കിൽ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
                    ret.frameSize = ARMSuperSection;
                } അല്ലെങ്കിൽ {
                    ret.frameSize = ARMSection;
                }
#endif
                printf("frame_%p_%04lu = frame ", pde, PD_INDEX(i));
                obj_frame_print_attrs(ret);
                നിർത്തുക;
            }

            അവസ്ഥ pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu = pt\n", pde, PD_INDEX(i));
                arm32_obj_pt_print_slots(pt);
                ret.frameSize = ARMSection;
                നിർത്തുക;
            }
            സ്ഥിരസ്ഥിതി:
                ret.frameSize = ARMSection;
                നിർത്തുക;
            }
            i += (1 << pageBitsForSize(ret.frameSize));
            എങ്കിൽ (i < (1 << pageBitsForSize(ret.frameSize))) {
                നിർത്തുക; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

ശൂന്യം debug_capDL(ശൂന്യം)
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
