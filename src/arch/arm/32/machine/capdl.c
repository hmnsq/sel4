/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#અન્તર્ભાવ <arch/machine/capdl.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <kernel/cspace.h>

#define MAX_UL          0xffffffff
#define PT_INDEX(vptr)  ((vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS))
#define PD_INDEX(vptr)  (vptr >> (PAGE_BITS + PT_INDEX_BITS))

word_t get_tcb_sp(tcb_t *tcb)
{
    પાછા tcb->tcbArch.tcbContext.registers[SP];
}

#ifdef CONFIG_PRINTING

સ્થિર શૂન્ય obj_frame_print_attrs(resolve_ret_t ret);
સ્થિર શૂન્ય cap_frame_print_attrs_pt(pte_t *pte);
સ્થિર શૂન્ય cap_frame_print_attrs_pd(pde_t *pde);
સ્થિર શૂન્ય cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX);
સ્થિર શૂન્ય arm32_obj_pt_print_slots(pte_t *pt);
સ્થિર શૂન્ય arm32_cap_pt_print_slots(pte_t *pt);

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
સ્થિર શૂન્ય cap_frame_print_attrs_pt(pte_t *pte)
{
    word_t AP, XN, TEX;
    પસંદગી (pte_ptr_get_pteType(pte)) {
    સ્થિતિ pte_pte_small:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#નહીંતર
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#endif
        XN = pte_pte_small_ptr_get_XN(pte);
        તોડો;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        તોડો;
#endif
    મૂળભૂત:
        પ્રતિજ્ઞા(!"should not happend");
    }
    cap_frame_print_attrs_impl(AP, XN, TEX);
}

સ્થિર શૂન્ય cap_frame_print_attrs_pd(pde_t *pde)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#નહીંતર
    cap_frame_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#endif
}

સ્થિર શૂન્ય cap_frame_print_attrs_impl(word_t AP, word_t XN, word_t TEX)
{
    printf("(");

    /* rights */
    પસંદગી (AP) {
    સ્થિતિ 0b00:
    સ્થિતિ 0b01:
        તોડો;
    સ્થિતિ 0b10:
        printf("R");
        તોડો;
    સ્થિતિ 0b11:
        printf("RW");
    મૂળભૂત:
        તોડો;
    }

    જો (!XN) {
        printf("X");
    }

    જો (!TEX) {
        printf(", uncached");
    }

    printf(")\n");
}

સ્થિર શૂન્ય arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_page_size_t page_size;
    word_t i = 0;
    જ્યારે (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        પસંદગી (pte_ptr_get_pteType(pte)) {
        સ્થિતિ pte_pte_small: {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            જો (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                page_size = ARMLargePage;
            } નહીંતર {
                page_size = ARMSmallPage;
            }
#નહીંતર
            page_size = ARMSmallPage;
#endif
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            તોડો;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        સ્થિતિ pte_pte_large: {
            page_size = ARMLargePage;
            printf("0x%lx: frame_%p_%04lu ", PT_INDEX(i), pte, PT_INDEX(i));
            cap_frame_print_attrs_pt(pte);
            તોડો;
        }
#endif
        મૂળભૂત:
            page_size = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(page_size));
    }
}

શૂન્ય obj_vtable_print_slots(tcb_t *tcb)
{
    જો (is_Valid_VTable_મૂળ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_page_size_t page_size;
        printf("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        જ્યારે (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            પસંદગી (pde_ptr_get_pdeType(pde)) {
            સ્થિતિ pde_pde_section: {
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                જો (pde_pde_section_ptr_get_size(pde)) {
                    page_size = ARMSuperSection;
                } નહીંતર {
                    page_size = ARMSection;
                }
#નહીંતર
                જો (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    page_size = ARMSuperSection;
                } નહીંતર {
                    page_size = ARMSection;
                }
#endif
                printf("0x%lx: frame_%p_%04lu ", PD_INDEX(i), pde, PD_INDEX(i));
                cap_frame_print_attrs_pd(pde);
                તોડો;
            }

            સ્થિતિ pde_pde_coarse: {
                printf("0x%lx: pt_%p_%04lu\n", PD_INDEX(i), pde, PD_INDEX(i));
                page_size = ARMSection;
                તોડો;
            }
            મૂળભૂત:
                page_size = ARMSection;
                તોડો;
            }
            i += (1 << pageBitsForSize(page_size));
            જો (i < (1 << pageBitsForSize(page_size))) {
                તોડો; /* overflowed */
            }
        }
        printf("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        જ્યારે (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            જો (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu {\n", pde, PD_INDEX(i));
                arm32_cap_pt_print_slots(pt);
                printf("}\n"); /* pt */
            }
            i += (1 << pageBitsForSize(ARMSection));
            જો (i < (1 << pageBitsForSize(ARMSection))) {
                તોડો; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
સ્થિર શૂન્ય cap_frame_print_attrs_vptr(word_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    પસંદગી (pde_ptr_get_pdeType(pde)) {
    સ્થિતિ pde_pde_section: {
        printf("frame_%p_%04lu ", pde, PD_INDEX(vptr));
        cap_frame_print_attrs_pd(pde);
        તોડો;
    }
    સ્થિતિ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        પસંદગી (pte_ptr_get_pteType(pte)) {
        સ્થિતિ pte_pte_small: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            તોડો;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        સ્થિતિ pte_pte_large: {
            printf("frame_%p_%04lu ", pte, PT_INDEX(vptr));
            cap_frame_print_attrs_pt(pte);
            તોડો;
        }
#endif
        મૂળભૂત:
            પ્રતિજ્ઞા(0);
        }
        તોડો;
    }
    મૂળભૂત:
        પ્રતિજ્ઞા(0);
    }
}

શૂન્ય print_ipc_buffer_slot(tcb_t *tcb)
{
    word_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_page_directory_cap_get_capPDMappedASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    findPDForASID_ret_t find_ret = findPDForASID(asid);
    printf("ipc_buffer_slot: ");
    cap_frame_print_attrs_vptr(vptr, find_ret.pd);
}

શૂન્ય print_cap_arch(cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_page_table_cap: {
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(cap);
        જો (asid) {
            printf("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr), (long unsigned પૂર્ણાંક)asid);
        } નહીંતર {
            printf("pt_%p_%04lu\n", lookupPDSlot(find_ret.pd, vptr), PD_INDEX(vptr));
        }
        તોડો;
    }
    સ્થિતિ cap_page_directory_cap: {
        asid_t asid = cap_page_directory_cap_get_capPDMappedASID(cap);
        findPDForASID_ret_t find_ret = findPDForASID(asid);
        જો (asid) {
            printf("%p_pd (asid: %lu)\n",
                   find_ret.pd, (long unsigned પૂર્ણાંક)asid);
        } નહીંતર {
            printf("%p_pd\n", find_ret.pd);
        }
        તોડો;
    }
    સ્થિતિ cap_asid_control_cap: {
        /* only one in the system */
        printf("asid_control\n");
        તોડો;
    }
    સ્થિતિ cap_small_frame_cap: {
        vptr_t vptr = cap_small_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_small_frame_cap_get_capFMappedASID(cap));
        પ્રતિજ્ઞા(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        તોડો;
    }
    સ્થિતિ cap_frame_cap: {
        vptr_t vptr = cap_frame_cap_get_capFMappedAddress(cap);
        findPDForASID_ret_t find_ret = findPDForASID(cap_frame_cap_get_capFMappedASID(cap));
        પ્રતિજ્ઞા(find_ret.status == EXCEPTION_NONE);
        cap_frame_print_attrs_vptr(vptr, find_ret.pd);
        તોડો;
    }
    સ્થિતિ cap_asid_pool_cap: {
        printf("%p_asid_pool\n", (શૂન્ય *)cap_asid_pool_cap_get_capASIDPool(cap));
        તોડો;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ cap_vcpu_cap: {
        printf("%p_vcpu\n", (શૂન્ય *)cap_vcpu_cap_get_capVCPUPtr(cap));
        તોડો;
    }
#endif

        /* ARM specific caps */
#ifdef CONFIG_TK1_SMMU
    સ્થિતિ cap_io_space_cap: {
        printf("%p_io_space\n", (શૂન્ય *)cap_io_space_cap_get_capModuleID(cap));
        તોડો;
    }
#endif
    મૂળભૂત: {
        printf("[unknown cap %u]\n", cap_get_capType(cap));
        તોડો;
    }
    }
}

શૂન્ય print_object_arch(cap_t cap)
{

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_frame_cap:
    સ્થિતિ cap_small_frame_cap:
    સ્થિતિ cap_page_table_cap:
    સ્થિતિ cap_page_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        તોડો;

    સ્થિતિ cap_asid_pool_cap: {
        printf("%p_asid_pool = asid_pool ",
               (શૂન્ય *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        તોડો;
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ cap_vcpu_cap: {
        printf("%p_vcpu = vcpu\n", (શૂન્ય *)cap_vcpu_cap_get_capVCPUPtr(cap));
        તોડો;
    }
#endif
        /* ARM specific objects */
#ifdef CONFIG_TK1_SMMU
    સ્થિતિ cap_io_space_cap: {
        printf("%p_io_space = io_space ", (શૂન્ય *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        તોડો;
    }
#endif
    મૂળભૂત: {
        printf("[unknown object %u]\n", cap_get_capType(cap));
        તોડો;
    }
    }
}

સ્થિર શૂન્ય obj_frame_print_attrs(resolve_ret_t ret)
{
    printf("(");

    /* VM size */
    પસંદગી (ret.frameSize) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ ARMSection:
        printf("2M");
        તોડો;
    સ્થિતિ ARMSuperSection:
        printf("32M");
        તોડો;
#નહીંતર
    સ્થિતિ ARMSection:
        printf("1M");
        તોડો;
    સ્થિતિ ARMSuperSection:
        printf("16M");
        તોડો;
#endif
    સ્થિતિ ARMLargePage:
        printf("64k");
        તોડો;
    સ્થિતિ ARMSmallPage:
        printf("4k");
        તોડો;
    }

    printf(", paddr: %p)\n", (શૂન્ય *)ret.frameBase);
}

સ્થિર શૂન્ય arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    word_t i = 0;
    જ્યારે (i < BIT(PT_INDEX_BITS + PAGE_BITS)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        પસંદગી (pte_ptr_get_pteType(pte)) {
        સ્થિતિ pte_pte_small: {
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            જો (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } નહીંતર {
                ret.frameSize = ARMSmallPage;
            }
#નહીંતર
            ret.frameSize = ARMSmallPage;
#endif
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            તોડો;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        સ્થિતિ pte_pte_large: {
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            printf("frame_%p_%04lu = frame ", pte, PT_INDEX(i));
            obj_frame_print_attrs(ret);
            તોડો;
        }
#endif
        મૂળભૂત:
            ret.frameSize = ARMSmallPage;
        }
        i += (1 << pageBitsForSize(ret.frameSize));
    }
}

શૂન્ય obj_tcb_print_vtable(tcb_t *tcb)
{
    જો (is_Valid_VTable_મૂળ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        add_to_seen(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_of_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        printf("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        word_t i = 0;
        જ્યારે (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            પસંદગી (pde_ptr_get_pdeType(pde)) {
            સ્થિતિ pde_pde_section: {
                ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                જો (pde_pde_section_ptr_get_size(pde)) {
                    ret.frameSize = ARMSuperSection;
                } નહીંતર {
                    ret.frameSize = ARMSection;
                }
#નહીંતર
                જો (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
                    ret.frameSize = ARMSuperSection;
                } નહીંતર {
                    ret.frameSize = ARMSection;
                }
#endif
                printf("frame_%p_%04lu = frame ", pde, PD_INDEX(i));
                obj_frame_print_attrs(ret);
                તોડો;
            }

            સ્થિતિ pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
                printf("pt_%p_%04lu = pt\n", pde, PD_INDEX(i));
                arm32_obj_pt_print_slots(pt);
                ret.frameSize = ARMSection;
                તોડો;
            }
            મૂળભૂત:
                ret.frameSize = ARMSection;
                તોડો;
            }
            i += (1 << pageBitsForSize(ret.frameSize));
            જો (i < (1 << pageBitsForSize(ret.frameSize))) {
                તોડો; /* overflowed */
            }
        }
    }
}

#endif /* CONFIG_PRINTING */

શૂન્ય debug_capDL(શૂન્ય)
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
