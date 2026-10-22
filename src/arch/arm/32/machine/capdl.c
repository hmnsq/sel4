/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

#تعریف_کړه MAX_UL          0xffffffff
#تعریف_کړه PT_شاخص(vptr)  ((vptr >> پاڼه_بټونه) & ماسک(PT_شاخص_بټونه))
#تعریف_کړه PD_شاخص(vptr)  (vptr >> (پاڼه_بټونه + PT_شاخص_بټونه))

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#که_تعریف_شوی CONFIG_PRINTING

ساكن فراغ obj_چوکاټ_print_attrs(resolve_ret_t ret);
ساكن فراغ cap_چوکاټ_print_attrs_pt(pte_t *pte);
ساكن فراغ cap_چوکاټ_print_attrs_pd(pde_t *pde);
ساكن فراغ cap_چوکاټ_print_attrs_impl(کلمه_t AP, کلمه_t XN, کلمه_t TEX);
ساكن فراغ arm32_obj_pt_print_slots(pte_t *pt);
ساكن فراغ arm32_cap_pt_print_slots(pte_t *pt);

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
ساكن فراغ cap_چوکاټ_print_attrs_pt(pte_t *pte)
{
    کلمه_t AP, XN, TEX;
    بدل (pte_ptr_get_pteType(pte)) {
    حالة pte_pte_small:
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#وإلا
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#د_که_پای
        XN = pte_pte_small_ptr_get_XN(pte);
        اكسر;

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        اكسر;
#د_که_پای
    افتراضي:
        أكد(!"should not happend");
    }
    cap_چوکاټ_print_attrs_impl(AP, XN, TEX);
}

ساكن فراغ cap_چوکاټ_print_attrs_pd(pde_t *pde)
{
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_چوکاټ_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#وإلا
    cap_چوکاټ_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#د_که_پای
}

ساكن فراغ cap_چوکاټ_print_attrs_impl(کلمه_t AP, کلمه_t XN, کلمه_t TEX)
{
    چاپ_کړه("(");

    /* rights */
    بدل (AP) {
    حالة 0b00:
    حالة 0b01:
        اكسر;
    حالة 0b10:
        چاپ_کړه("R");
        اكسر;
    حالة 0b11:
        چاپ_کړه("RW");
    افتراضي:
        اكسر;
    }

    إذا (!XN) {
        چاپ_کړه("X");
    }

    إذا (!TEX) {
        چاپ_کړه(", uncached");
    }

    چاپ_کړه(")\n");
}

ساكن فراغ arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_پاڼه_اندازه_t پاڼه_اندازه;
    کلمه_t i = 0;
    طالما (i < بټ(PT_شاخص_بټونه + پاڼه_بټونه)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                پاڼه_اندازه = ARMLarge_پاڼه;
            } وإلا {
                پاڼه_اندازه = ARMSmall_پاڼه;
            }
#وإلا
            پاڼه_اندازه = ARMSmall_پاڼه;
#د_که_پای
            چاپ_کړه("0x%lx: frame_%p_%04lu ", PT_شاخص(i), pte, PT_شاخص(i));
            cap_چوکاټ_print_attrs_pt(pte);
            اكسر;
        }
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            پاڼه_اندازه = ARMLarge_پاڼه;
            چاپ_کړه("0x%lx: frame_%p_%04lu ", PT_شاخص(i), pte, PT_شاخص(i));
            cap_چوکاټ_print_attrs_pt(pte);
            اكسر;
        }
#د_که_پای
        افتراضي:
            پاڼه_اندازه = ARMSmall_پاڼه;
        }
        i += (1 << پاڼه_بټونه_For_اندازه(پاڼه_اندازه));
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_ریښه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کړه_منزل_کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_د_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_پاڼه_اندازه_t پاڼه_اندازه;
        چاپ_کړه("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        کلمه_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_اندازه(pde)) {
                    پاڼه_اندازه = ARMSuperSection;
                } وإلا {
                    پاڼه_اندازه = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    پاڼه_اندازه = ARMSuperSection;
                } وإلا {
                    پاڼه_اندازه = ARMSection;
                }
#د_که_پای
                چاپ_کړه("0x%lx: frame_%p_%04lu ", PD_شاخص(i), pde, PD_شاخص(i));
                cap_چوکاټ_print_attrs_pd(pde);
                اكسر;
            }

            حالة pde_pde_coarse: {
                چاپ_کړه("0x%lx: pt_%p_%04lu\n", PD_شاخص(i), pde, PD_شاخص(i));
                پاڼه_اندازه = ARMSection;
                اكسر;
            }
            افتراضي:
                پاڼه_اندازه = ARMSection;
                اكسر;
            }
            i += (1 << پاڼه_بټونه_For_اندازه(پاڼه_اندازه));
            إذا (i < (1 << پاڼه_بټونه_For_اندازه(پاڼه_اندازه))) {
                اكسر; /* overflowed */
            }
        }
        چاپ_کړه("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            إذا (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پته(pde));
                چاپ_کړه("pt_%p_%04lu {\n", pde, PD_شاخص(i));
                arm32_cap_pt_print_slots(pt);
                چاپ_کړه("}\n"); /* pt */
            }
            i += (1 << پاڼه_بټونه_For_اندازه(ARMSection));
            إذا (i < (1 << پاڼه_بټونه_For_اندازه(ARMSection))) {
                اكسر; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکاټ_print_attrs_vptr(کلمه_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section: {
        چاپ_کړه("frame_%p_%04lu ", pde, PD_شاخص(vptr));
        cap_چوکاټ_print_attrs_pd(pde);
        اكسر;
    }
    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پته(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            چاپ_کړه("frame_%p_%04lu ", pte, PT_شاخص(vptr));
            cap_چوکاټ_print_attrs_pt(pte);
            اكسر;
        }

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            چاپ_کړه("frame_%p_%04lu ", pte, PT_شاخص(vptr));
            cap_چوکاټ_print_attrs_pt(pte);
            اكسر;
        }
#د_که_پای
        افتراضي:
            أكد(0);
        }
        اكسر;
    }
    افتراضي:
        أكد(0);
    }
}

فراغ print_ipc_بفر_خونه(tcb_t *tcb)
{
    کلمه_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
    چاپ_کړه("ipc_buffer_slot: ");
    cap_چوکاټ_print_attrs_vptr(vptr, پیدا_کړه_ret.pd);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_پاڼه_جدول_cap: {
        asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap);
        پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
        vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap);
        إذا (asid) {
            چاپ_کړه("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(پیدا_کړه_ret.pd, vptr), PD_شاخص(vptr), (اوږد بینښه صحيح)asid);
        } وإلا {
            چاپ_کړه("pt_%p_%04lu\n", lookupPDSlot(پیدا_کړه_ret.pd, vptr), PD_شاخص(vptr));
        }
        اكسر;
    }
    حالة cap_پاڼه_directory_cap: {
        asid_t asid = cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(cap);
        پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
        إذا (asid) {
            چاپ_کړه("%p_pd (asid: %lu)\n",
                   پیدا_کړه_ret.pd, (اوږد بینښه صحيح)asid);
        } وإلا {
            چاپ_کړه("%p_pd\n", پیدا_کړه_ret.pd);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        چاپ_کړه("asid_control\n");
        اكسر;
    }
    حالة cap_small_چوکاټ_cap: {
        vptr_t vptr = cap_small_چوکاټ_cap_get_cap_FMapped_پته(cap);
        پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(cap_small_چوکاټ_cap_get_cap_FMapped_ASID(cap));
        أكد(پیدا_کړه_ret.حالت == EXCEPTION_NONE);
        cap_چوکاټ_print_attrs_vptr(vptr, پیدا_کړه_ret.pd);
        اكسر;
    }
    حالة cap_چوکاټ_cap: {
        vptr_t vptr = cap_چوکاټ_cap_get_cap_FMapped_پته(cap);
        پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(cap_چوکاټ_cap_get_cap_FMapped_ASID(cap));
        أكد(پیدا_کړه_ret.حالت == EXCEPTION_NONE);
        cap_چوکاټ_print_attrs_vptr(vptr, پیدا_کړه_ret.pd);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        چاپ_کړه("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        چاپ_کړه("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#د_که_پای

        /* ARM specific caps */
#که_تعریف_شوی CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        چاپ_کړه("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#د_که_پای
    افتراضي: {
        چاپ_کړه("[unknown cap %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_چوکاټ_cap:
    حالة cap_small_چوکاټ_cap:
    حالة cap_پاڼه_جدول_cap:
    حالة cap_پاڼه_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        چاپ_کړه("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        چاپ_کړه("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#د_که_پای
        /* ARM specific objects */
#که_تعریف_شوی CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        چاپ_کړه("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#د_که_پای
    افتراضي: {
        چاپ_کړه("[unknown object %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_چوکاټ_print_attrs(resolve_ret_t ret)
{
    چاپ_کړه("(");

    /* VM size */
    بدل (ret.چوکاټ_اندازه) {
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة ARMSection:
        چاپ_کړه("2M");
        اكسر;
    حالة ARMSuperSection:
        چاپ_کړه("32M");
        اكسر;
#وإلا
    حالة ARMSection:
        چاپ_کړه("1M");
        اكسر;
    حالة ARMSuperSection:
        چاپ_کړه("16M");
        اكسر;
#د_که_پای
    حالة ARMLarge_پاڼه:
        چاپ_کړه("64k");
        اكسر;
    حالة ARMSmall_پاڼه:
        چاپ_کړه("4k");
        اكسر;
    }

    چاپ_کړه(", paddr: %p)\n", (فراغ *)ret.چوکاټ_بنسټ);
}

ساكن فراغ arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    کلمه_t i = 0;
    طالما (i < بټ(PT_شاخص_بټونه + پاڼه_بټونه)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            ret.چوکاټ_بنسټ = pte_pte_small_ptr_get_پته(pte);
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.چوکاټ_بنسټ &= ~ماسک(پاڼه_بټونه_For_اندازه(ARMLarge_پاڼه));
                ret.چوکاټ_اندازه = ARMLarge_پاڼه;
            } وإلا {
                ret.چوکاټ_اندازه = ARMSmall_پاڼه;
            }
#وإلا
            ret.چوکاټ_اندازه = ARMSmall_پاڼه;
#د_که_پای
            چاپ_کړه("frame_%p_%04lu = frame ", pte, PT_شاخص(i));
            obj_چوکاټ_print_attrs(ret);
            اكسر;
        }
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            ret.چوکاټ_بنسټ = pte_pte_large_ptr_get_پته(pte);
            ret.چوکاټ_اندازه = ARMLarge_پاڼه;
            چاپ_کړه("frame_%p_%04lu = frame ", pte, PT_شاخص(i));
            obj_چوکاټ_print_attrs(ret);
            اكسر;
        }
#د_که_پای
        افتراضي:
            ret.چوکاټ_اندازه = ARMSmall_پاڼه;
        }
        i += (1 << پاڼه_بټونه_For_اندازه(ret.چوکاټ_اندازه));
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_ریښه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کړه_منزل_کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_د_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        چاپ_کړه("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        کلمه_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
                ret.چوکاټ_بنسټ = pde_pde_section_ptr_get_پته(pde);
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_اندازه(pde)) {
                    ret.چوکاټ_اندازه = ARMSuperSection;
                } وإلا {
                    ret.چوکاټ_اندازه = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.چوکاټ_بنسټ &= ~ماسک(پاڼه_بټونه_For_اندازه(ARMSuperSection));
                    ret.چوکاټ_اندازه = ARMSuperSection;
                } وإلا {
                    ret.چوکاټ_اندازه = ARMSection;
                }
#د_که_پای
                چاپ_کړه("frame_%p_%04lu = frame ", pde, PD_شاخص(i));
                obj_چوکاټ_print_attrs(ret);
                اكسر;
            }

            حالة pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پته(pde));
                چاپ_کړه("pt_%p_%04lu = pt\n", pde, PD_شاخص(i));
                arm32_obj_pt_print_slots(pt);
                ret.چوکاټ_اندازه = ARMSection;
                اكسر;
            }
            افتراضي:
                ret.چوکاټ_اندازه = ARMSection;
                اكسر;
            }
            i += (1 << پاڼه_بټونه_For_اندازه(ret.چوکاټ_اندازه));
            إذا (i < (1 << پاڼه_بټونه_For_اندازه(ret.چوکاټ_اندازه))) {
                اكسر; /* overflowed */
            }
        }
    }
}

#د_که_پای /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    چاپ_کړه("arch aarch32\n");
    چاپ_کړه("objects {\n");
#که_تعریف_شوی CONFIG_PRINTING
    print_objects();
#د_که_پای
    چاپ_کړه("}\n");

    چاپ_کړه("caps {\n");

    /* reset the seen list */
    reset_کتل_شوی_list();

#که_تعریف_شوی CONFIG_PRINTING
    print_caps();
    چاپ_کړه("}\n");

    obj_irq_print_maps();
#د_که_پای /* CONFIG_PRINTING */
}


#د_که_پای /* CONFIG_DEBUG_BUILD */
