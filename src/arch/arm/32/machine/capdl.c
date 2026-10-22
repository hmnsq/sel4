/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#ifdef CONFIG_DEBUG_BUILD

#شامل <arch/machine/capdl.h>
#شامل <string.h>
#شامل <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_INDEX(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS))
#define PD_INDEX(vptr)  (vptr >> (PAGE_BITS + PT_INDEX_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    بازگشت tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

ایستا تهی obj_frame_print_attrs(resolve_ret_t ret);
ایستا تهی cap_frame_print_attrs_pt(pte_t *pte);
ایستا تهی cap_frame_print_attrs_pd(pde_t *pde);
ایستا تهی cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
ایستا تهی arm32_obj_pt_print_slots(pte_t *pt);
ایستا تهی arm32_cap_pt_print_slots(pte_t *pt);

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
ایستا تهی cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    انتخاب (pte_ptr_get_pteType(pte)) {
    حالت pte_pte_small:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#وگرنه
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        بشکن;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        بشکن;
#endif
    پیشفرض:
        أكد(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

ایستا تهی cap_frame_print_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#وگرنه
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

ایستا تهی cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    printf("(");

    /* rights */
    انتخاب (AP) {
    حالت 0b00:
    حالت 0b01:
        بشکن;
    حالت 0b10:
        printf("R");
        بشکن;
    حالت 0b11:
        printf("RW");
    پیشفرض:
        بشکن;
    }

    اگر (!XN) {
        printf("X");
    }

    اگر (!TEX) {
        printf(", uncached");
    }

    printf(")\n");
}

ایستا تهی arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_size_t page_size;
    word_t i = 0;
    وقتی (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        انتخاب (pte_ptr_get_pteType(pte)) {
        حالت pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            اگر (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_size = ARMLargePage;
            } وگرنه {
                page_size = ARMSmallPage;
            }
#وگرنه
            page_size = ARMSmallPage;
#endif
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            بشکن;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        حالت pte_pte_large: {
            page_size = ARMLargePage;
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            بشکن;
        }
#endif
        پیشفرض:
            page_size = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(page_size));
    }
}

تهی obj_vtable_print_slots(tcb_t *tcb)
{
    اگر (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_size_t page_size;
        printf("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        وقتی (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            انتخاب (pde_ptr_get_pdeType(pde)) {
            حالت pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                اگر (pde_pde_section_ptr_get_size(pde)) {
                    page_size = ARMSuperSection;
                } وگرنه {
                    page_size = ARMSection;
                }
#وگرنه
                اگر (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_size = ARMSuperSection;
                } وگرنه {
                    page_size = ARMSection;
                }
#endif
                printf("0x%lx: frame_%p_%04lu ", PD_INDEX(i), pde, PD_INDEX(i));
                cap_frame_print_attrs_pd(pde);
                بشکن;
            }

            حالت pde_pde_coarse: {
                printf("0x%lx: pt_%p_%04lu\n", PD_INDEX(i), pde, PD_INDEX(i));
                page_size = ARMSection;
                بشکن;
            }
            پیشفرض:
                page_size = ARMSection;
                بشکن;
            }
            i += (1 << pageBitsForSize(page_size));
            اگر (i < (1 << pageBitsForSize(page_size))) {
                بشکن; /* overflowed */
            }
        }
        printf("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        وقتی (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            اگر (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu {\n", pde, PD_INDEX(i));
                arm32_cap_pt_print_slots(pt);
                printf("}\n"); /* pt */
            }
            i += (1 << pageBitsForSize(ARMSection));
            اگر (i < (1 << pageBitsForSize(ARMSection))) {
                بشکن; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ایستا تهی cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    انتخاب (pde_ptr_get_pdeType(pde)) {
    حالت pde_pde_section: {
        printf("frame_%p_%04lu ", pde, PD_INDEX(vptr));
        cap_frame_print_attrs_pd(pde);
        بشکن;
    }
    حالت pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        انتخاب (pte_ptr_get_pteType(pte)) {
        حالت pte_pte_small: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            بشکن;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        حالت pte_pte_large: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            بشکن;
        }
#endif
        پیشفرض:
            أكد(0);
        }
        بشکن;
    }
    پیشفرض:
        أكد(0);
    }
}

تهی print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

تهی print_cap_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        اگر (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr), (long unsigned صحیح)asid);
        } وگرنه {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr));
        }
        بشکن;
    }
    حالت cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        اگر (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.pd, (long unsigned صحیح)asid);
        } وگرنه {
            printf("%p_pd\n", find_ret.pd);
        }
        بشکن;
    }
    حالت cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        بشکن;
    }
    حالت cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        أكد(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        بشکن;
    }
    حالت cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        أكد(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        بشکن;
    }
    حالت cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (تهی *)cap_asid_pool_cap_get_capASIDPool(cap));
        بشکن;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap: {
        printf("%p_vcpu\n", (تهی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        بشکن;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    حالت cap_io_space_cap: {
        printf("%p_io_space\n", (تهی *)cap_io_space_cap_get_capModuleID(cap));
        بشکن;
    }
#endif
    پیشفرض: {
        printf("[unknown cap %u]\n", cap_get_capType(cap));
        بشکن;
    }
    }
}

تهی print_object_arch(cap_t cap)
{

    انتخاب (cap_get_capType(cap)) {
    حالت cap_frame_cap:
    حالت cap_small_frame_cap:
    حالت cap_page_table_cap:
    حالت cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        بشکن;

    حالت cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (تهی *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        بشکن;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (تهی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        بشکن;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    حالت cap_io_space_cap: {
        printf("%p_io_space = io_space ", (تهی *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        بشکن;
    }
#endif
    پیشفرض: {
        printf("[unknown object %u]\n", cap_get_capType(cap));
        بشکن;
    }
    }
}

ایستا تهی obj_frame_print_attrs(resolve_ret_t ret)
{
    printf("(");

    /* VM size */
    انتخاب (ret.frameSize) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت ARMSection:
        printf("2M");
        بشکن;
    حالت ARMSuperSection:
        printf("32M");
        بشکن;
#وگرنه
    حالت ARMSection:
        printf("1M");
        بشکن;
    حالت ARMSuperSection:
        printf("16M");
        بشکن;
#endif
    حالت ARMLargePage:
        printf("64k");
        بشکن;
    حالت ARMSmallPage:
        printf("4k");
        بشکن;
    }

    printf(", paddr: %p)\n", (تهی *)ret.frameBase);
}

ایستا تهی arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    وقتی (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        انتخاب (pte_ptr_get_pteType(pte)) {
        حالت pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            اگر (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } وگرنه {
                ret.frameSize = ARMSmallPage;
            }
#وگرنه
            ret.frameSize = ARMSmallPage;
#endif
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            بشکن;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        حالت pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            بشکن;
        }
#endif
        پیشفرض:
            ret.frameSize = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(ret.frameSize));
    }
}

تهی obj_tcb_print_vtable(tcb_t *tcb)
{
    اگر (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        printf("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        وقتی (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            انتخاب (pde_ptr_get_pdeType(pde)) {
            حالت pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                اگر (pde_pde_section_ptr_get_size(pde)) {
                    ret.frameSize = ARMSuperSection;
                } وگرنه {
                    ret.frameSize = ARMSection;
                }
#وگرنه
                اگر (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
                    ret.frameSize = ARMSuperSection;
                } وگرنه {
                    ret.frameSize = ARMSection;
                }
#endif
                printf("frame_%p_%04lu = frame ", pde, PD_INDEX(i));
                obj_frame_print_attrs(ret);
                بشکن;
            }

            حالت pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu = pt\n", pde, PD_INDEX(i));
                arm32_obj_pt_print_slots(pt);
                ret.frameSize = ARMSection;
                بشکن;
            }
            پیشفرض:
                ret.frameSize = ARMSection;
                بشکن;
            }
            i += (1 << pageBitsForSize(ret.frameSize));
            اگر (i < (1 << pageBitsForSize(ret.frameSize))) {
                بشکن; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

تهی debug_capDL(تهی)
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
