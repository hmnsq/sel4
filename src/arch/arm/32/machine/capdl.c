/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_DEBUG_BUILD

#অন্তর্ভাব <arch/machine/capdl.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_সূচক(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_সূচক_BITS))
#define PD_সূচক(vptr)  (vptr >> (PAGE_BITS + PT_সূচক_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    ফেরত tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

স্থির শূন্য obj_frame_print_attrs(resolve_ret_t ret);
স্থির শূন্য cap_frame_print_attrs_pt(pte_t *pte);
স্থির শূন্য cap_frame_print_attrs_pd(pde_t *pde);
স্থির শূন্য cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
স্থির শূন্য arm32_obj_pt_print_slots(pte_t *pt);
স্থির শূন্য arm32_cap_pt_print_slots(pte_t *pt);

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
স্থির শূন্য cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    নির্বাচন (pte_ptr_get_pteType(pte)) {
    ক্ষেত্র pte_pte_small:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#অন্যথা
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        বিরতি;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        বিরতি;
#endif
    পূর্বনির্ধারিত:
        প্রতিজ্ঞা(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

স্থির শূন্য cap_frame_print_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#অন্যথা
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

স্থির শূন্য cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    printf("(");

    /* rights */
    নির্বাচন (AP) {
    ক্ষেত্র 0b00:
    ক্ষেত্র 0b01:
        বিরতি;
    ক্ষেত্র 0b10:
        printf("R");
        বিরতি;
    ক্ষেত্র 0b11:
        printf("RW");
    পূর্বনির্ধারিত:
        বিরতি;
    }

    যদি (!XN) {
        printf("X");
    }

    যদি (!TEX) {
        printf(", uncached");
    }

    printf(")\n");
}

স্থির শূন্য arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_আকার_t page_আকার;
    word_t i = 0;
    যখন (i < BIT(PT_সূচক_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        নির্বাচন (pte_ptr_get_pteType(pte)) {
        ক্ষেত্র pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            যদি (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_আকার = ARMLargePage;
            } অন্যথা {
                page_আকার = ARMSmallPage;
            }
#অন্যথা
            page_আকার = ARMSmallPage;
#endif
            printf("0x%lx: frame_%p_%04lu ", PT_সূচক(i), pte, PT_সূচক(i));
            cap_frame_print_attrs_pt(pte);
            বিরতি;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ক্ষেত্র pte_pte_large: {
            page_আকার = ARMLargePage;
            printf("0x%lx: frame_%p_%04lu ", PT_সূচক(i), pte, PT_সূচক(i));
            cap_frame_print_attrs_pt(pte);
            বিরতি;
        }
#endif
        পূর্বনির্ধারিত:
            page_আকার = ARMSmallPage;
        }
        i += (1 << page_Bits_For_আকার(page_আকার));
    }
}

শূন্য obj_vtable_print_slots(tcb_t *tcb)
{
    যদি (is_Valid_VTable_মূল(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_আকার_t page_আকার;
        printf("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        যখন (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            নির্বাচন (pde_ptr_get_pdeType(pde)) {
            ক্ষেত্র pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                যদি (pde_pde_section_ptr_get_আকার(pde)) {
                    page_আকার = ARMSuperSection;
                } অন্যথা {
                    page_আকার = ARMSection;
                }
#অন্যথা
                যদি (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_আকার = ARMSuperSection;
                } অন্যথা {
                    page_আকার = ARMSection;
                }
#endif
                printf("0x%lx: frame_%p_%04lu ", PD_সূচক(i), pde, PD_সূচক(i));
                cap_frame_print_attrs_pd(pde);
                বিরতি;
            }

            ক্ষেত্র pde_pde_coarse: {
                printf("0x%lx: pt_%p_%04lu\n", PD_সূচক(i), pde, PD_সূচক(i));
                page_আকার = ARMSection;
                বিরতি;
            }
            পূর্বনির্ধারিত:
                page_আকার = ARMSection;
                বিরতি;
            }
            i += (1 << page_Bits_For_আকার(page_আকার));
            যদি (i < (1 << page_Bits_For_আকার(page_আকার))) {
                বিরতি; /* overflowed */
            }
        }
        printf("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        যখন (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            যদি (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu {\n", pde, PD_সূচক(i));
                arm32_cap_pt_print_slots(pt);
                printf("}\n"); /* pt */
            }
            i += (1 << page_Bits_For_আকার(ARMSection));
            যদি (i < (1 << page_Bits_For_আকার(ARMSection))) {
                বিরতি; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
স্থির শূন্য cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    নির্বাচন (pde_ptr_get_pdeType(pde)) {
    ক্ষেত্র pde_pde_section: {
        printf("frame_%p_%04lu ", pde, PD_সূচক(vptr));
        cap_frame_print_attrs_pd(pde);
        বিরতি;
    }
    ক্ষেত্র pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        নির্বাচন (pte_ptr_get_pteType(pte)) {
        ক্ষেত্র pte_pte_small: {
            printf("frame_%p_%04lu ", pte, PT_সূচক(vptr));
            cap_frame_print_attrs_pt(pte);
            বিরতি;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ক্ষেত্র pte_pte_large: {
            printf("frame_%p_%04lu ", pte, PT_সূচক(vptr));
            cap_frame_print_attrs_pt(pte);
            বিরতি;
        }
#endif
        পূর্বনির্ধারিত:
            প্রতিজ্ঞা(0);
        }
        বিরতি;
    }
    পূর্বনির্ধারিত:
        প্রতিজ্ঞা(0);
    }
}

শূন্য print_ipc_বাফার_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

শূন্য print_cap_arch(cap_t cap)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        যদি (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_সূচক(vptr), (long unsigned পূর্ণসংখ্যা)asid);
        } অন্যথা {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_সূচক(vptr));
        }
        বিরতি;
    }
    ক্ষেত্র cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        যদি (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.pd, (long unsigned পূর্ণসংখ্যা)asid);
        } অন্যথা {
            printf("%p_pd\n", find_ret.pd);
        }
        বিরতি;
    }
    ক্ষেত্র cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        বিরতি;
    }
    ক্ষেত্র cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        প্রতিজ্ঞা(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        বিরতি;
    }
    ক্ষেত্র cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        প্রতিজ্ঞা(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        বিরতি;
    }
    ক্ষেত্র cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (শূন্য *)cap_asid_pool_cap_get_capASIDPool(cap));
        বিরতি;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap: {
        printf("%p_vcpu\n", (শূন্য *)cap_vcpu_cap_get_capVCPUPtr(cap));
        বিরতি;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র cap_io_space_cap: {
        printf("%p_io_space\n", (শূন্য *)cap_io_space_cap_get_capModuleID(cap));
        বিরতি;
    }
#endif
    পূর্বনির্ধারিত: {
        printf("[unknown cap %u]\n", cap_get_capType(cap));
        বিরতি;
    }
    }
}

শূন্য print_object_arch(cap_t cap)
{

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_frame_cap:
    ক্ষেত্র cap_small_frame_cap:
    ক্ষেত্র cap_page_table_cap:
    ক্ষেত্র cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        বিরতি;

    ক্ষেত্র cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (শূন্য *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        বিরতি;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (শূন্য *)cap_vcpu_cap_get_capVCPUPtr(cap));
        বিরতি;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র cap_io_space_cap: {
        printf("%p_io_space = io_space ", (শূন্য *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        বিরতি;
    }
#endif
    পূর্বনির্ধারিত: {
        printf("[unknown object %u]\n", cap_get_capType(cap));
        বিরতি;
    }
    }
}

স্থির শূন্য obj_frame_print_attrs(resolve_ret_t ret)
{
    printf("(");

    /* VM size */
    নির্বাচন (ret.frame_আকার) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র ARMSection:
        printf("2M");
        বিরতি;
    ক্ষেত্র ARMSuperSection:
        printf("32M");
        বিরতি;
#অন্যথা
    ক্ষেত্র ARMSection:
        printf("1M");
        বিরতি;
    ক্ষেত্র ARMSuperSection:
        printf("16M");
        বিরতি;
#endif
    ক্ষেত্র ARMLargePage:
        printf("64k");
        বিরতি;
    ক্ষেত্র ARMSmallPage:
        printf("4k");
        বিরতি;
    }

    printf(", paddr: %p)\n", (শূন্য *)ret.frameBase);
}

স্থির শূন্য arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    যখন (i < BIT(PT_সূচক_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        নির্বাচন (pte_ptr_get_pteType(pte)) {
        ক্ষেত্র pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            যদি (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(page_Bits_For_আকার(ARMLargePage));
                ret.frame_আকার = ARMLargePage;
            } অন্যথা {
                ret.frame_আকার = ARMSmallPage;
            }
#অন্যথা
            ret.frame_আকার = ARMSmallPage;
#endif
            printf("frame_%p_%04lu = frame ", pte, PT_সূচক(i));
            obj_frame_print_attrs(ret);
            বিরতি;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ক্ষেত্র pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frame_আকার = ARMLargePage;
            printf("frame_%p_%04lu = frame ", pte, PT_সূচক(i));
            obj_frame_print_attrs(ret);
            বিরতি;
        }
#endif
        পূর্বনির্ধারিত:
            ret.frame_আকার = ARMSmallPage;
        }
        i += (1 << page_Bits_For_আকার(ret.frame_আকার));
    }
}

শূন্য obj_tcb_print_vtable(tcb_t *tcb)
{
    যদি (is_Valid_VTable_মূল(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        printf("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        যখন (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            নির্বাচন (pde_ptr_get_pdeType(pde)) {
            ক্ষেত্র pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                যদি (pde_pde_section_ptr_get_আকার(pde)) {
                    ret.frame_আকার = ARMSuperSection;
                } অন্যথা {
                    ret.frame_আকার = ARMSection;
                }
#অন্যথা
                যদি (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(page_Bits_For_আকার(ARMSuperSection));
                    ret.frame_আকার = ARMSuperSection;
                } অন্যথা {
                    ret.frame_আকার = ARMSection;
                }
#endif
                printf("frame_%p_%04lu = frame ", pde, PD_সূচক(i));
                obj_frame_print_attrs(ret);
                বিরতি;
            }

            ক্ষেত্র pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu = pt\n", pde, PD_সূচক(i));
                arm32_obj_pt_print_slots(pt);
                ret.frame_আকার = ARMSection;
                বিরতি;
            }
            পূর্বনির্ধারিত:
                ret.frame_আকার = ARMSection;
                বিরতি;
            }
            i += (1 << page_Bits_For_আকার(ret.frame_আকার));
            যদি (i < (1 << page_Bits_For_আকার(ret.frame_আকার))) {
                বিরতি; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

শূন্য debug_capDL(শূন্য)
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
