/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

#تعریف_کن MAX_UL          0xffffffff
#تعریف_کن PT_شاخص(vptr)  ((vptr >> صفحه_بیتها) & ماسک(PT_شاخص_بیتها))
#تعریف_کن PD_شاخص(vptr)  (vptr >> (صفحه_بیتها + PT_شاخص_بیتها))

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#اگر_تعریف_شده CONFIG_PRINTING

ساكن فراغ obj_قاب_print_attrs(resolve_ret_t ret);
ساكن فراغ cap_قاب_print_attrs_pt(pte_t *pte);
ساكن فراغ cap_قاب_print_attrs_pd(pde_t *pde);
ساكن فراغ cap_قاب_print_attrs_impl(کلمه_t AP, کلمه_t XN, کلمه_t TEX);
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
ساكن فراغ cap_قاب_print_attrs_pt(pte_t *pte)
{
    کلمه_t AP, XN, TEX;
    بدل (pte_ptr_get_pteType(pte)) {
    حالة pte_pte_small:
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#وإلا
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#پایان_اگر
        XN = pte_pte_small_ptr_get_XN(pte);
        اكسر;

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        اكسر;
#پایان_اگر
    افتراضي:
        أكد(!"should not happend");
    }
    cap_قاب_print_attrs_impl(AP, XN, TEX);
}

ساكن فراغ cap_قاب_print_attrs_pd(pde_t *pde)
{
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_قاب_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#وإلا
    cap_قاب_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#پایان_اگر
}

ساكن فراغ cap_قاب_print_attrs_impl(کلمه_t AP, کلمه_t XN, کلمه_t TEX)
{
    چاپ_کن("(");

    /* rights */
    بدل (AP) {
    حالة 0b00:
    حالة 0b01:
        اكسر;
    حالة 0b10:
        چاپ_کن("R");
        اكسر;
    حالة 0b11:
        چاپ_کن("RW");
    افتراضي:
        اكسر;
    }

    إذا (!XN) {
        چاپ_کن("X");
    }

    إذا (!TEX) {
        چاپ_کن(", uncached");
    }

    چاپ_کن(")\n");
}

ساكن فراغ arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_صفحه_اندازه_t صفحه_اندازه;
    کلمه_t i = 0;
    طالما (i < بیت(PT_شاخص_بیتها + صفحه_بیتها)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                صفحه_اندازه = ARMLarge_صفحه;
            } وإلا {
                صفحه_اندازه = ARMSmall_صفحه;
            }
#وإلا
            صفحه_اندازه = ARMSmall_صفحه;
#پایان_اگر
            چاپ_کن("0x%lx: frame_%p_%04lu ", PT_شاخص(i), pte, PT_شاخص(i));
            cap_قاب_print_attrs_pt(pte);
            اكسر;
        }
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            صفحه_اندازه = ARMLarge_صفحه;
            چاپ_کن("0x%lx: frame_%p_%04lu ", PT_شاخص(i), pte, PT_شاخص(i));
            cap_قاب_print_attrs_pt(pte);
            اكسر;
        }
#پایان_اگر
        افتراضي:
            صفحه_اندازه = ARMSmall_صفحه;
        }
        i += (1 << صفحه_بیتها_For_اندازه(صفحه_اندازه));
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کن_مقصد_دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_از_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_صفحه_اندازه_t صفحه_اندازه;
        چاپ_کن("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        کلمه_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_اندازه(pde)) {
                    صفحه_اندازه = ARMSuperSection;
                } وإلا {
                    صفحه_اندازه = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    صفحه_اندازه = ARMSuperSection;
                } وإلا {
                    صفحه_اندازه = ARMSection;
                }
#پایان_اگر
                چاپ_کن("0x%lx: frame_%p_%04lu ", PD_شاخص(i), pde, PD_شاخص(i));
                cap_قاب_print_attrs_pd(pde);
                اكسر;
            }

            حالة pde_pde_coarse: {
                چاپ_کن("0x%lx: pt_%p_%04lu\n", PD_شاخص(i), pde, PD_شاخص(i));
                صفحه_اندازه = ARMSection;
                اكسر;
            }
            افتراضي:
                صفحه_اندازه = ARMSection;
                اكسر;
            }
            i += (1 << صفحه_بیتها_For_اندازه(صفحه_اندازه));
            إذا (i < (1 << صفحه_بیتها_For_اندازه(صفحه_اندازه))) {
                اكسر; /* overflowed */
            }
        }
        چاپ_کن("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            إذا (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_آدرس(pde));
                چاپ_کن("pt_%p_%04lu {\n", pde, PD_شاخص(i));
                arm32_cap_pt_print_slots(pt);
                چاپ_کن("}\n"); /* pt */
            }
            i += (1 << صفحه_بیتها_For_اندازه(ARMSection));
            إذا (i < (1 << صفحه_بیتها_For_اندازه(ARMSection))) {
                اكسر; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_قاب_print_attrs_vptr(کلمه_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section: {
        چاپ_کن("frame_%p_%04lu ", pde, PD_شاخص(vptr));
        cap_قاب_print_attrs_pd(pde);
        اكسر;
    }
    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_آدرس(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            چاپ_کن("frame_%p_%04lu ", pte, PT_شاخص(vptr));
            cap_قاب_print_attrs_pt(pte);
            اكسر;
        }

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            چاپ_کن("frame_%p_%04lu ", pte, PT_شاخص(vptr));
            cap_قاب_print_attrs_pt(pte);
            اكسر;
        }
#پایان_اگر
        افتراضي:
            أكد(0);
        }
        اكسر;
    }
    افتراضي:
        أكد(0);
    }
}

فراغ print_ipc_میانگیر_جایگاه(tcb_t *tcb)
{
    کلمه_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_صفحه_directory_cap_get_cap_PDMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
    چاپ_کن("ipc_buffer_slot: ");
    cap_قاب_print_attrs_vptr(vptr, پیدا_کن_ret.pd);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحه_جدول_cap: {
        asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap);
        پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
        vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap);
        إذا (asid) {
            چاپ_کن("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(پیدا_کن_ret.pd, vptr), PD_شاخص(vptr), (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("pt_%p_%04lu\n", lookupPDSlot(پیدا_کن_ret.pd, vptr), PD_شاخص(vptr));
        }
        اكسر;
    }
    حالة cap_صفحه_directory_cap: {
        asid_t asid = cap_صفحه_directory_cap_get_cap_PDMapped_ASID(cap);
        پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
        إذا (asid) {
            چاپ_کن("%p_pd (asid: %lu)\n",
                   پیدا_کن_ret.pd, (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("%p_pd\n", پیدا_کن_ret.pd);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        چاپ_کن("asid_control\n");
        اكسر;
    }
    حالة cap_small_قاب_cap: {
        vptr_t vptr = cap_small_قاب_cap_get_cap_FMapped_آدرس(cap);
        پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret = پیدا_کن_PDFor_ASID(cap_small_قاب_cap_get_cap_FMapped_ASID(cap));
        أكد(پیدا_کن_ret.وضعیت == EXCEPTION_NONE);
        cap_قاب_print_attrs_vptr(vptr, پیدا_کن_ret.pd);
        اكسر;
    }
    حالة cap_قاب_cap: {
        vptr_t vptr = cap_قاب_cap_get_cap_FMapped_آدرس(cap);
        پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret = پیدا_کن_PDFor_ASID(cap_قاب_cap_get_cap_FMapped_ASID(cap));
        أكد(پیدا_کن_ret.وضعیت == EXCEPTION_NONE);
        cap_قاب_print_attrs_vptr(vptr, پیدا_کن_ret.pd);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        چاپ_کن("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        چاپ_کن("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#پایان_اگر

        /* ARM specific caps */
#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        چاپ_کن("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#پایان_اگر
    افتراضي: {
        چاپ_کن("[unknown cap %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_قاب_cap:
    حالة cap_small_قاب_cap:
    حالة cap_صفحه_جدول_cap:
    حالة cap_صفحه_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        چاپ_کن("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        چاپ_کن("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#پایان_اگر
        /* ARM specific objects */
#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        چاپ_کن("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#پایان_اگر
    افتراضي: {
        چاپ_کن("[unknown object %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_قاب_print_attrs(resolve_ret_t ret)
{
    چاپ_کن("(");

    /* VM size */
    بدل (ret.قاب_اندازه) {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة ARMSection:
        چاپ_کن("2M");
        اكسر;
    حالة ARMSuperSection:
        چاپ_کن("32M");
        اكسر;
#وإلا
    حالة ARMSection:
        چاپ_کن("1M");
        اكسر;
    حالة ARMSuperSection:
        چاپ_کن("16M");
        اكسر;
#پایان_اگر
    حالة ARMLarge_صفحه:
        چاپ_کن("64k");
        اكسر;
    حالة ARMSmall_صفحه:
        چاپ_کن("4k");
        اكسر;
    }

    چاپ_کن(", paddr: %p)\n", (فراغ *)ret.قاب_پایه);
}

ساكن فراغ arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    کلمه_t i = 0;
    طالما (i < بیت(PT_شاخص_بیتها + صفحه_بیتها)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            ret.قاب_پایه = pte_pte_small_ptr_get_آدرس(pte);
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.قاب_پایه &= ~ماسک(صفحه_بیتها_For_اندازه(ARMLarge_صفحه));
                ret.قاب_اندازه = ARMLarge_صفحه;
            } وإلا {
                ret.قاب_اندازه = ARMSmall_صفحه;
            }
#وإلا
            ret.قاب_اندازه = ARMSmall_صفحه;
#پایان_اگر
            چاپ_کن("frame_%p_%04lu = frame ", pte, PT_شاخص(i));
            obj_قاب_print_attrs(ret);
            اكسر;
        }
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            ret.قاب_پایه = pte_pte_large_ptr_get_آدرس(pte);
            ret.قاب_اندازه = ARMLarge_صفحه;
            چاپ_کن("frame_%p_%04lu = frame ", pte, PT_شاخص(i));
            obj_قاب_print_attrs(ret);
            اكسر;
        }
#پایان_اگر
        افتراضي:
            ret.قاب_اندازه = ARMSmall_صفحه;
        }
        i += (1 << صفحه_بیتها_For_اندازه(ret.قاب_اندازه));
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کن_مقصد_دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_از_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        چاپ_کن("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        کلمه_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
                ret.قاب_پایه = pde_pde_section_ptr_get_آدرس(pde);
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_اندازه(pde)) {
                    ret.قاب_اندازه = ARMSuperSection;
                } وإلا {
                    ret.قاب_اندازه = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.قاب_پایه &= ~ماسک(صفحه_بیتها_For_اندازه(ARMSuperSection));
                    ret.قاب_اندازه = ARMSuperSection;
                } وإلا {
                    ret.قاب_اندازه = ARMSection;
                }
#پایان_اگر
                چاپ_کن("frame_%p_%04lu = frame ", pde, PD_شاخص(i));
                obj_قاب_print_attrs(ret);
                اكسر;
            }

            حالة pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_آدرس(pde));
                چاپ_کن("pt_%p_%04lu = pt\n", pde, PD_شاخص(i));
                arm32_obj_pt_print_slots(pt);
                ret.قاب_اندازه = ARMSection;
                اكسر;
            }
            افتراضي:
                ret.قاب_اندازه = ARMSection;
                اكسر;
            }
            i += (1 << صفحه_بیتها_For_اندازه(ret.قاب_اندازه));
            إذا (i < (1 << صفحه_بیتها_For_اندازه(ret.قاب_اندازه))) {
                اكسر; /* overflowed */
            }
        }
    }
}

#پایان_اگر /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    چاپ_کن("arch aarch32\n");
    چاپ_کن("objects {\n");
#اگر_تعریف_شده CONFIG_PRINTING
    print_objects();
#پایان_اگر
    چاپ_کن("}\n");

    چاپ_کن("caps {\n");

    /* reset the seen list */
    reset_دیده_list();

#اگر_تعریف_شده CONFIG_PRINTING
    print_caps();
    چاپ_کن("}\n");

    obj_irq_print_maps();
#پایان_اگر /* CONFIG_PRINTING */
}


#پایان_اگر /* CONFIG_DEBUG_BUILD */
