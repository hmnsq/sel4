/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_INDEX(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS))
#define PD_INDEX(vptr)  (vptr >> (PAGE_BITS + PT_INDEX_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    ಪ್ರತ್ಯಾಗಮ tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

ಸ್ಥಿರ ಶೂನ್ಯ obj_frame_print_attrs(resolve_ret_t ret);
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_pt(pte_t *pte);
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_pd(pde_t *pde);
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
ಸ್ಥಿರ ಶೂನ್ಯ arm32_obj_pt_print_slots(pte_t *pt);
ಸ್ಥಿರ ಶೂನ್ಯ arm32_cap_pt_print_slots(pte_t *pt);

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
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    ವಿಕಲ್ಪ (pte_ptr_get_pteType(pte)) {
    ಪಕ್ಷ pte_pte_small:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#ಅನ್ಯಥಾ
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        ಭಙ್ಗ;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        ಭಙ್ಗ;
#endif
    ಸ್ವಭಾವ:
        ಪ್ರತಿಜ್ಞಾ(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#ಅನ್ಯಥಾ
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    printf("(");

    /* rights */
    ವಿಕಲ್ಪ (AP) {
    ಪಕ್ಷ 0b00:
    ಪಕ್ಷ 0b01:
        ಭಙ್ಗ;
    ಪಕ್ಷ 0b10:
        printf("R");
        ಭಙ್ಗ;
    ಪಕ್ಷ 0b11:
        printf("RW");
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    ಯದಿ (!XN) {
        printf("X");
    }

    ಯದಿ (!TEX) {
        printf(", uncached");
    }

    printf(")\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_size_t page_size;
    word_t i = 0;
    ಯಾವತ್ (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ವಿಕಲ್ಪ (pte_ptr_get_pteType(pte)) {
        ಪಕ್ಷ pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            ಯದಿ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_size = ARMLargePage;
            } ಅನ್ಯಥಾ {
                page_size = ARMSmallPage;
            }
#ಅನ್ಯಥಾ
            page_size = ARMSmallPage;
#endif
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            ಭಙ್ಗ;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಪಕ್ಷ pte_pte_large: {
            page_size = ARMLargePage;
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            ಭಙ್ಗ;
        }
#endif
        ಸ್ವಭಾವ:
            page_size = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(page_size));
    }
}

ಶೂನ್ಯ obj_vtable_print_slots(tcb_t *tcb)
{
    ಯದಿ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_size_t page_size;
        printf("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        ಯಾವತ್ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ವಿಕಲ್ಪ (pde_ptr_get_pdeType(pde)) {
            ಪಕ್ಷ pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                ಯದಿ (pde_pde_section_ptr_get_size(pde)) {
                    page_size = ARMSuperSection;
                } ಅನ್ಯಥಾ {
                    page_size = ARMSection;
                }
#ಅನ್ಯಥಾ
                ಯದಿ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_size = ARMSuperSection;
                } ಅನ್ಯಥಾ {
                    page_size = ARMSection;
                }
#endif
                printf("0x%lx: frame_%p_%04lu ", PD_INDEX(i), pde, PD_INDEX(i));
                cap_frame_print_attrs_pd(pde);
                ಭಙ್ಗ;
            }

            ಪಕ್ಷ pde_pde_coarse: {
                printf("0x%lx: pt_%p_%04lu\n", PD_INDEX(i), pde, PD_INDEX(i));
                page_size = ARMSection;
                ಭಙ್ಗ;
            }
            ಸ್ವಭಾವ:
                page_size = ARMSection;
                ಭಙ್ಗ;
            }
            i += (1 << pageBitsForSize(page_size));
            ಯದಿ (i < (1 << pageBitsForSize(page_size))) {
                ಭಙ್ಗ; /* overflowed */
            }
        }
        printf("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ಯಾವತ್ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ಯದಿ (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu {\n", pde, PD_INDEX(i));
                arm32_cap_pt_print_slots(pt);
                printf("}\n"); /* pt */
            }
            i += (1 << pageBitsForSize(ARMSection));
            ಯದಿ (i < (1 << pageBitsForSize(ARMSection))) {
                ಭಙ್ಗ; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ಸ್ಥಿರ ಶೂನ್ಯ cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    ವಿಕಲ್ಪ (pde_ptr_get_pdeType(pde)) {
    ಪಕ್ಷ pde_pde_section: {
        printf("frame_%p_%04lu ", pde, PD_INDEX(vptr));
        cap_frame_print_attrs_pd(pde);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        ವಿಕಲ್ಪ (pte_ptr_get_pteType(pte)) {
        ಪಕ್ಷ pte_pte_small: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            ಭಙ್ಗ;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಪಕ್ಷ pte_pte_large: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            ಭಙ್ಗ;
        }
#endif
        ಸ್ವಭಾವ:
            ಪ್ರತಿಜ್ಞಾ(0);
        }
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        ಪ್ರತಿಜ್ಞಾ(0);
    }
}

ಶೂನ್ಯ print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

ಶೂನ್ಯ print_cap_arch(cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        ಯದಿ (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr), (long unsigned ಪೂರ್ಣಾಙ್ಕ)asid);
        } ಅನ್ಯಥಾ {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr));
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        ಯದಿ (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.pd, (long unsigned ಪೂರ್ಣಾಙ್ಕ)asid);
        } ಅನ್ಯಥಾ {
            printf("%p_pd\n", find_ret.pd);
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        ಪ್ರತಿಜ್ಞಾ(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        ಪ್ರತಿಜ್ಞಾ(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ಭಙ್ಗ;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap: {
        printf("%p_vcpu\n", (ಶೂನ್ಯ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ಭಙ್ಗ;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ cap_io_space_cap: {
        printf("%p_io_space\n", (ಶೂನ್ಯ *)cap_io_space_cap_get_capModuleID(cap));
        ಭಙ್ಗ;
    }
#endif
    ಸ್ವಭಾವ: {
        printf("[unknown cap %u]\n", cap_get_capType(cap));
        ಭಙ್ಗ;
    }
    }
}

ಶೂನ್ಯ print_object_arch(cap_t cap)
{

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_frame_cap:
    ಪಕ್ಷ cap_small_frame_cap:
    ಪಕ್ಷ cap_page_table_cap:
    ಪಕ್ಷ cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (ಶೂನ್ಯ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ಭಙ್ಗ;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (ಶೂನ್ಯ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ಭಙ್ಗ;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ cap_io_space_cap: {
        printf("%p_io_space = io_space ", (ಶೂನ್ಯ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        ಭಙ್ಗ;
    }
#endif
    ಸ್ವಭಾವ: {
        printf("[unknown object %u]\n", cap_get_capType(cap));
        ಭಙ್ಗ;
    }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ obj_frame_print_attrs(resolve_ret_t ret)
{
    printf("(");

    /* VM size */
    ವಿಕಲ್ಪ (ret.frameSize) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ ARMSection:
        printf("2M");
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMSuperSection:
        printf("32M");
        ಭಙ್ಗ;
#ಅನ್ಯಥಾ
    ಪಕ್ಷ ARMSection:
        printf("1M");
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMSuperSection:
        printf("16M");
        ಭಙ್ಗ;
#endif
    ಪಕ್ಷ ARMLargePage:
        printf("64k");
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMSmallPage:
        printf("4k");
        ಭಙ್ಗ;
    }

    printf(", paddr: %p)\n", (ಶೂನ್ಯ *)ret.frameBase);
}

ಸ್ಥಿರ ಶೂನ್ಯ arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    ಯಾವತ್ (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ವಿಕಲ್ಪ (pte_ptr_get_pteType(pte)) {
        ಪಕ್ಷ pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            ಯದಿ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } ಅನ್ಯಥಾ {
                ret.frameSize = ARMSmallPage;
            }
#ಅನ್ಯಥಾ
            ret.frameSize = ARMSmallPage;
#endif
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            ಭಙ್ಗ;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಪಕ್ಷ pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            ಭಙ್ಗ;
        }
#endif
        ಸ್ವಭಾವ:
            ret.frameSize = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(ret.frameSize));
    }
}

ಶೂನ್ಯ obj_tcb_print_vtable(tcb_t *tcb)
{
    ಯದಿ (is_Valid_VTable_ಬೇರು(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        printf("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        ಯಾವತ್ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ವಿಕಲ್ಪ (pde_ptr_get_pdeType(pde)) {
            ಪಕ್ಷ pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                ಯದಿ (pde_pde_section_ptr_get_size(pde)) {
                    ret.frameSize = ARMSuperSection;
                } ಅನ್ಯಥಾ {
                    ret.frameSize = ARMSection;
                }
#ಅನ್ಯಥಾ
                ಯದಿ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
                    ret.frameSize = ARMSuperSection;
                } ಅನ್ಯಥಾ {
                    ret.frameSize = ARMSection;
                }
#endif
                printf("frame_%p_%04lu = frame ", pde, PD_INDEX(i));
                obj_frame_print_attrs(ret);
                ಭಙ್ಗ;
            }

            ಪಕ್ಷ pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu = pt\n", pde, PD_INDEX(i));
                arm32_obj_pt_print_slots(pt);
                ret.frameSize = ARMSection;
                ಭಙ್ಗ;
            }
            ಸ್ವಭಾವ:
                ret.frameSize = ARMSection;
                ಭಙ್ಗ;
            }
            i += (1 << pageBitsForSize(ret.frameSize));
            ಯದಿ (i < (1 << pageBitsForSize(ret.frameSize))) {
                ಭಙ್ಗ; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

ಶೂನ್ಯ debug_capDL(ಶೂನ್ಯ)
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
