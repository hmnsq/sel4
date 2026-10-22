/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_INDEX(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS))
#define PD_INDEX(vptr)  (vptr >> (PAGE_BITS + PT_INDEX_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

ساكن فراغ obj_frame_اطبع_attrs(resolve_ret_t ret);
ساكن فراغ cap_frame_اطبع_attrs_pt(pte_t *pte);
ساكن فراغ cap_frame_اطبع_attrs_pd(pde_t *pde);
ساكن فراغ cap_frame_اطبع_attrs_impl(word_t AP, word_t XN, word_t TEX);
ساكن فراغ arm32_obj_pt_اطبع_slots(pte_t *pt);
ساكن فراغ arm32_cap_pt_اطبع_slots(pte_t *pt);

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
ساكن فراغ cap_frame_اطبع_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    بدل (pte_ptr_get_pteType(pte)) {
    حالة pte_pte_small:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#وإلا
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        اكسر;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        اكسر;
#endif
    افتراضي:
        أكد(!"should not happend");
    }
    cap_frame_اطبع_attrs_impl(AP, XN, TEX);
}

ساكن فراغ cap_frame_اطبع_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_اطبع_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#وإلا
    cap_frame_اطبع_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

ساكن فراغ cap_frame_اطبع_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    printf("(");

    /* rights */
    بدل (AP) {
    حالة 0b00:
    حالة 0b01:
        اكسر;
    حالة 0b10:
        printf("R");
        اكسر;
    حالة 0b11:
        printf("RW");
    افتراضي:
        اكسر;
    }

    إذا (!XN) {
        printf("X");
    }

    إذا (!TEX) {
        printf(", uncached");
    }

    printf(")\n");
}

ساكن فراغ arm32_cap_pt_اطبع_slots(pte_t *pt)
{
    vm_page_size_t page_size;
    word_t i = 0;
    طالما (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_size = ARMLargePage;
            } وإلا {
                page_size = ARMSmallPage;
            }
#وإلا
            page_size = ARMSmallPage;
#endif
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_اطبع_attrs_pt(pte);
            اكسر;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            page_size = ARMLargePage;
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_اطبع_attrs_pt(pte);
            اكسر;
        }
#endif
        افتراضي:
            page_size = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(page_size));
    }
}

فراغ obj_vtable_اطبع_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_جذر(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_size_t page_size;
        printf("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_size(pde)) {
                    page_size = ARMSuperSection;
                } وإلا {
                    page_size = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_size = ARMSuperSection;
                } وإلا {
                    page_size = ARMSection;
                }
#endif
                printf("0x%lx: frame_%p_%04lu ", PD_INDEX(i), pde, PD_INDEX(i));
                cap_frame_اطبع_attrs_pd(pde);
                اكسر;
            }

            حالة pde_pde_coarse: {
                printf("0x%lx: pt_%p_%04lu\n", PD_INDEX(i), pde, PD_INDEX(i));
                page_size = ARMSection;
                اكسر;
            }
            افتراضي:
                page_size = ARMSection;
                اكسر;
            }
            i += (1 << pageBitsForSize(page_size));
            إذا (i < (1 << pageBitsForSize(page_size))) {
                اكسر; /* overflowed */
            }
        }
        printf("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            إذا (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu {\n", pde, PD_INDEX(i));
                arm32_cap_pt_اطبع_slots(pt);
                printf("}\n"); /* pt */
            }
            i += (1 << pageBitsForSize(ARMSection));
            إذا (i < (1 << pageBitsForSize(ARMSection))) {
                اكسر; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_frame_اطبع_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section: {
        printf("frame_%p_%04lu ", pde, PD_INDEX(vptr));
        cap_frame_اطبع_attrs_pd(pde);
        اكسر;
    }
    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_اطبع_attrs_pt(pte);
            اكسر;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_اطبع_attrs_pt(pte);
            اكسر;
        }
#endif
        افتراضي:
            أكد(0);
        }
        اكسر;
    }
    افتراضي:
        أكد(0);
    }
}

فراغ اطبع_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    printf("ipc_buffer_slot: ");
    cap_frame_اطبع_attrs_vptr(vptr, find_ret.pd);
}

فراغ اطبع_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        إذا (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr), (long unsigned صحيح)asid);
        } وإلا {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr));
        }
        اكسر;
    }
    حالة cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        إذا (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.pd, (long unsigned صحيح)asid);
        } وإلا {
            printf("%p_pd\n", find_ret.pd);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        اكسر;
    }
    حالة cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        أكد(find_ret.status == EXCEPTION_NONE);
        cap_frame_اطبع_attrs_vptr(vptr, find_ret.pd);
        اكسر;
    }
    حالة cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        أكد(find_ret.status == EXCEPTION_NONE);
        cap_frame_اطبع_attrs_vptr(vptr, find_ret.pd);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        printf("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        printf("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#endif
    افتراضي: {
        printf("[unknown cap %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ اطبع_object_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_frame_cap:
    حالة cap_small_frame_cap:
    حالة cap_page_table_cap:
    حالة cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_اطبع_attrs(cap);
        اكسر;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        printf("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_اطبع_attrs(cap);
        اكسر;
    }
#endif
    افتراضي: {
        printf("[unknown object %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_frame_اطبع_attrs(resolve_ret_t ret)
{
    printf("(");

    /* VM size */
    بدل (ret.frameSize) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة ARMSection:
        printf("2M");
        اكسر;
    حالة ARMSuperSection:
        printf("32M");
        اكسر;
#وإلا
    حالة ARMSection:
        printf("1M");
        اكسر;
    حالة ARMSuperSection:
        printf("16M");
        اكسر;
#endif
    حالة ARMLargePage:
        printf("64k");
        اكسر;
    حالة ARMSmallPage:
        printf("4k");
        اكسر;
    }

    printf(", paddr: %p)\n", (فراغ *)ret.frameBase);
}

ساكن فراغ arm32_obj_pt_اطبع_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    طالما (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } وإلا {
                ret.frameSize = ARMSmallPage;
            }
#وإلا
            ret.frameSize = ARMSmallPage;
#endif
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_اطبع_attrs(ret);
            اكسر;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_اطبع_attrs(ret);
            اكسر;
        }
#endif
        افتراضي:
            ret.frameSize = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(ret.frameSize));
    }
}

فراغ obj_tcb_اطبع_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_جذر(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        printf("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_size(pde)) {
                    ret.frameSize = ARMSuperSection;
                } وإلا {
                    ret.frameSize = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
                    ret.frameSize = ARMSuperSection;
                } وإلا {
                    ret.frameSize = ARMSection;
                }
#endif
                printf("frame_%p_%04lu = frame ", pde, PD_INDEX(i));
                obj_frame_اطبع_attrs(ret);
                اكسر;
            }

            حالة pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu = pt\n", pde, PD_INDEX(i));
                arm32_obj_pt_اطبع_slots(pt);
                ret.frameSize = ARMSection;
                اكسر;
            }
            افتراضي:
                ret.frameSize = ARMSection;
                اكسر;
            }
            i += (1 << pageBitsForSize(ret.frameSize));
            إذا (i < (1 << pageBitsForSize(ret.frameSize))) {
                اكسر; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    printf("arch aarch32\n");
    printf("objects {\n");
#ifdef CONFIG_PRINTING
    اطبع_objects();
#endif
    printf("}\n");

    printf("caps {\n");

    /* reset the seen list */
    reset_seen_list();

#ifdef CONFIG_PRINTING
    اطبع_caps();
    printf("}\n");

    obj_irq_اطبع_maps();
#endif /* CONFIG_PRINTING */
}


#endif /* CONFIG_DEBUG_BUILD */
