/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

#تعریف_کرو MAX_UL          0xffffffff
#تعریف_کرو PT_اشاریہ(vptr)  ((vptr >> ورقہ_بٹ) & نقاب(PT_اشاریہ_بٹ))
#تعریف_کرو PD_اشاریہ(vptr)  (vptr >> (ورقہ_بٹ + PT_اشاریہ_بٹ))

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#جے_تعریف_شدہ CONFIG_PRINTING

ساكن فراغ obj_چوکھٹا_print_attrs(resolve_ret_t ret);
ساكن فراغ cap_چوکھٹا_print_attrs_pt(pte_t *pte);
ساكن فراغ cap_چوکھٹا_print_attrs_pd(pde_t *pde);
ساكن فراغ cap_چوکھٹا_print_attrs_impl(لفظ_t AP, لفظ_t XN, لفظ_t TEX);
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
ساكن فراغ cap_چوکھٹا_print_attrs_pt(pte_t *pte)
{
    لفظ_t AP, XN, TEX;
    بدل (pte_ptr_get_pteType(pte)) {
    حالة pte_pte_small:
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#وإلا
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#جے_انت
        XN = pte_pte_small_ptr_get_XN(pte);
        اكسر;

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        اكسر;
#جے_انت
    افتراضي:
        أكد(!"should not happend");
    }
    cap_چوکھٹا_print_attrs_impl(AP, XN, TEX);
}

ساكن فراغ cap_چوکھٹا_print_attrs_pd(pde_t *pde)
{
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_چوکھٹا_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#وإلا
    cap_چوکھٹا_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#جے_انت
}

ساكن فراغ cap_چوکھٹا_print_attrs_impl(لفظ_t AP, لفظ_t XN, لفظ_t TEX)
{
    لکھو("(");

    /* rights */
    بدل (AP) {
    حالة 0b00:
    حالة 0b01:
        اكسر;
    حالة 0b10:
        لکھو("R");
        اكسر;
    حالة 0b11:
        لکھو("RW");
    افتراضي:
        اكسر;
    }

    إذا (!XN) {
        لکھو("X");
    }

    إذا (!TEX) {
        لکھو(", uncached");
    }

    لکھو(")\n");
}

ساكن فراغ arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_ورقہ_ناپ_t ورقہ_ناپ;
    لفظ_t i = 0;
    طالما (i < ثنائی_ہندسہ(PT_اشاریہ_بٹ + ورقہ_بٹ)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                ورقہ_ناپ = ARMLarge_ورقہ;
            } وإلا {
                ورقہ_ناپ = ARMSmall_ورقہ;
            }
#وإلا
            ورقہ_ناپ = ARMSmall_ورقہ;
#جے_انت
            لکھو("0x%lx: frame_%p_%04lu ", PT_اشاریہ(i), pte, PT_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pt(pte);
            اكسر;
        }
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            ورقہ_ناپ = ARMLarge_ورقہ;
            لکھو("0x%lx: frame_%p_%04lu ", PT_اشاریہ(i), pte, PT_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pt(pte);
            اكسر;
        }
#جے_انت
        افتراضي:
            ورقہ_ناپ = ARMSmall_ورقہ;
        }
        i += (1 << ورقہ_بٹ_For_ناپ(ورقہ_ناپ));
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_منزل_تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_دا_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_ورقہ_ناپ_t ورقہ_ناپ;
        لکھو("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        لفظ_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_ناپ(pde)) {
                    ورقہ_ناپ = ARMSuperSection;
                } وإلا {
                    ورقہ_ناپ = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    ورقہ_ناپ = ARMSuperSection;
                } وإلا {
                    ورقہ_ناپ = ARMSection;
                }
#جے_انت
                لکھو("0x%lx: frame_%p_%04lu ", PD_اشاریہ(i), pde, PD_اشاریہ(i));
                cap_چوکھٹا_print_attrs_pd(pde);
                اكسر;
            }

            حالة pde_pde_coarse: {
                لکھو("0x%lx: pt_%p_%04lu\n", PD_اشاریہ(i), pde, PD_اشاریہ(i));
                ورقہ_ناپ = ARMSection;
                اكسر;
            }
            افتراضي:
                ورقہ_ناپ = ARMSection;
                اكسر;
            }
            i += (1 << ورقہ_بٹ_For_ناپ(ورقہ_ناپ));
            إذا (i < (1 << ورقہ_بٹ_For_ناپ(ورقہ_ناپ))) {
                اكسر; /* overflowed */
            }
        }
        لکھو("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            إذا (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pde));
                لکھو("pt_%p_%04lu {\n", pde, PD_اشاریہ(i));
                arm32_cap_pt_print_slots(pt);
                لکھو("}\n"); /* pt */
            }
            i += (1 << ورقہ_بٹ_For_ناپ(ARMSection));
            إذا (i < (1 << ورقہ_بٹ_For_ناپ(ARMSection))) {
                اكسر; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section: {
        لکھو("frame_%p_%04lu ", pde, PD_اشاریہ(vptr));
        cap_چوکھٹا_print_attrs_pd(pde);
        اكسر;
    }
    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            لکھو("frame_%p_%04lu ", pte, PT_اشاریہ(vptr));
            cap_چوکھٹا_print_attrs_pt(pte);
            اكسر;
        }

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            لکھو("frame_%p_%04lu ", pte, PT_اشاریہ(vptr));
            cap_چوکھٹا_print_attrs_pt(pte);
            اكسر;
        }
#جے_انت
        افتراضي:
            أكد(0);
        }
        اكسر;
    }
    افتراضي:
        أكد(0);
    }
}

فراغ print_ipc_بفر_تھاں(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    لبھو_PDFor_ASID_ret_t لبھو_ret = لبھو_PDFor_ASID(asid);
    لکھو("ipc_buffer_slot: ");
    cap_چوکھٹا_print_attrs_vptr(vptr, لبھو_ret.pd);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_ورقہ_جدول_cap: {
        asid_t asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap);
        لبھو_PDFor_ASID_ret_t لبھو_ret = لبھو_PDFor_ASID(asid);
        vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap);
        إذا (asid) {
            لکھو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(لبھو_ret.pd, vptr), PD_اشاریہ(vptr), (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("pt_%p_%04lu\n", lookupPDSlot(لبھو_ret.pd, vptr), PD_اشاریہ(vptr));
        }
        اكسر;
    }
    حالة cap_ورقہ_directory_cap: {
        asid_t asid = cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(cap);
        لبھو_PDFor_ASID_ret_t لبھو_ret = لبھو_PDFor_ASID(asid);
        إذا (asid) {
            لکھو("%p_pd (asid: %lu)\n",
                   لبھو_ret.pd, (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("%p_pd\n", لبھو_ret.pd);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        لکھو("asid_control\n");
        اكسر;
    }
    حالة cap_small_چوکھٹا_cap: {
        vptr_t vptr = cap_small_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        لبھو_PDFor_ASID_ret_t لبھو_ret = لبھو_PDFor_ASID(cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap));
        أكد(لبھو_ret.حالت == EXCEPTION_NONE);
        cap_چوکھٹا_print_attrs_vptr(vptr, لبھو_ret.pd);
        اكسر;
    }
    حالة cap_چوکھٹا_cap: {
        vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        لبھو_PDFor_ASID_ret_t لبھو_ret = لبھو_PDFor_ASID(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap));
        أكد(لبھو_ret.حالت == EXCEPTION_NONE);
        cap_چوکھٹا_print_attrs_vptr(vptr, لبھو_ret.pd);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        لکھو("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        لکھو("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جے_انت

        /* ARM specific caps */
#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        لکھو("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#جے_انت
    افتراضي: {
        لکھو("[unknown cap %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_چوکھٹا_cap:
    حالة cap_small_چوکھٹا_cap:
    حالة cap_ورقہ_جدول_cap:
    حالة cap_ورقہ_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        لکھو("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        لکھو("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جے_انت
        /* ARM specific objects */
#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        لکھو("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#جے_انت
    افتراضي: {
        لکھو("[unknown object %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_چوکھٹا_print_attrs(resolve_ret_t ret)
{
    لکھو("(");

    /* VM size */
    بدل (ret.چوکھٹا_ناپ) {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة ARMSection:
        لکھو("2M");
        اكسر;
    حالة ARMSuperSection:
        لکھو("32M");
        اكسر;
#وإلا
    حالة ARMSection:
        لکھو("1M");
        اكسر;
    حالة ARMSuperSection:
        لکھو("16M");
        اكسر;
#جے_انت
    حالة ARMLarge_ورقہ:
        لکھو("64k");
        اكسر;
    حالة ARMSmall_ورقہ:
        لکھو("4k");
        اكسر;
    }

    لکھو(", paddr: %p)\n", (فراغ *)ret.چوکھٹا_ادھار);
}

ساكن فراغ arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    لفظ_t i = 0;
    طالما (i < ثنائی_ہندسہ(PT_اشاریہ_بٹ + ورقہ_بٹ)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            ret.چوکھٹا_ادھار = pte_pte_small_ptr_get_پتہ(pte);
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.چوکھٹا_ادھار &= ~نقاب(ورقہ_بٹ_For_ناپ(ARMLarge_ورقہ));
                ret.چوکھٹا_ناپ = ARMLarge_ورقہ;
            } وإلا {
                ret.چوکھٹا_ناپ = ARMSmall_ورقہ;
            }
#وإلا
            ret.چوکھٹا_ناپ = ARMSmall_ورقہ;
#جے_انت
            لکھو("frame_%p_%04lu = frame ", pte, PT_اشاریہ(i));
            obj_چوکھٹا_print_attrs(ret);
            اكسر;
        }
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            ret.چوکھٹا_ادھار = pte_pte_large_ptr_get_پتہ(pte);
            ret.چوکھٹا_ناپ = ARMLarge_ورقہ;
            لکھو("frame_%p_%04lu = frame ", pte, PT_اشاریہ(i));
            obj_چوکھٹا_print_attrs(ret);
            اكسر;
        }
#جے_انت
        افتراضي:
            ret.چوکھٹا_ناپ = ARMSmall_ورقہ;
        }
        i += (1 << ورقہ_بٹ_For_ناپ(ret.چوکھٹا_ناپ));
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_منزل_تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_دا_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        لکھو("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        لفظ_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
                ret.چوکھٹا_ادھار = pde_pde_section_ptr_get_پتہ(pde);
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_ناپ(pde)) {
                    ret.چوکھٹا_ناپ = ARMSuperSection;
                } وإلا {
                    ret.چوکھٹا_ناپ = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.چوکھٹا_ادھار &= ~نقاب(ورقہ_بٹ_For_ناپ(ARMSuperSection));
                    ret.چوکھٹا_ناپ = ARMSuperSection;
                } وإلا {
                    ret.چوکھٹا_ناپ = ARMSection;
                }
#جے_انت
                لکھو("frame_%p_%04lu = frame ", pde, PD_اشاریہ(i));
                obj_چوکھٹا_print_attrs(ret);
                اكسر;
            }

            حالة pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pde));
                لکھو("pt_%p_%04lu = pt\n", pde, PD_اشاریہ(i));
                arm32_obj_pt_print_slots(pt);
                ret.چوکھٹا_ناپ = ARMSection;
                اكسر;
            }
            افتراضي:
                ret.چوکھٹا_ناپ = ARMSection;
                اكسر;
            }
            i += (1 << ورقہ_بٹ_For_ناپ(ret.چوکھٹا_ناپ));
            إذا (i < (1 << ورقہ_بٹ_For_ناپ(ret.چوکھٹا_ناپ))) {
                اكسر; /* overflowed */
            }
        }
    }
}

#جے_انت /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    لکھو("arch aarch32\n");
    لکھو("objects {\n");
#جے_تعریف_شدہ CONFIG_PRINTING
    print_objects();
#جے_انت
    لکھو("}\n");

    لکھو("caps {\n");

    /* reset the seen list */
    reset_تکیا_list();

#جے_تعریف_شدہ CONFIG_PRINTING
    print_caps();
    لکھو("}\n");

    obj_irq_print_maps();
#جے_انت /* CONFIG_PRINTING */
}


#جے_انت /* CONFIG_DEBUG_BUILD */
