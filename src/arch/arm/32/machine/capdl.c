/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

#وصف_ڪريو MAX_UL          0xffffffff
#وصف_ڪريو PT_اشاريو(vptr)  ((vptr >> صفحو_بٽ) & نقاب(PT_اشاريو_بٽ))
#وصف_ڪريو PD_اشاريو(vptr)  (vptr >> (صفحو_بٽ + PT_اشاريو_بٽ))

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING

ساكن فراغ obj_چوکٺو_print_attrs(resolve_ret_t ret);
ساكن فراغ cap_چوکٺو_print_attrs_pt(pte_t *pte);
ساكن فراغ cap_چوکٺو_print_attrs_pd(pde_t *pde);
ساكن فراغ cap_چوکٺو_print_attrs_impl(لفظ_t AP, لفظ_t XN, لفظ_t TEX);
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
ساكن فراغ cap_چوکٺو_print_attrs_pt(pte_t *pte)
{
    لفظ_t AP, XN, TEX;
    بدل (pte_ptr_get_pteType(pte)) {
    حالة pte_pte_small:
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#وإلا
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#جيڪڏهن_پورو
        XN = pte_pte_small_ptr_get_XN(pte);
        اكسر;

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        اكسر;
#جيڪڏهن_پورو
    افتراضي:
        أكد(!"should not happend");
    }
    cap_چوکٺو_print_attrs_impl(AP, XN, TEX);
}

ساكن فراغ cap_چوکٺو_print_attrs_pd(pde_t *pde)
{
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_چوکٺو_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#وإلا
    cap_چوکٺو_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#جيڪڏهن_پورو
}

ساكن فراغ cap_چوکٺو_print_attrs_impl(لفظ_t AP, لفظ_t XN, لفظ_t TEX)
{
    لکو("(");

    /* rights */
    بدل (AP) {
    حالة 0b00:
    حالة 0b01:
        اكسر;
    حالة 0b10:
        لکو("R");
        اكسر;
    حالة 0b11:
        لکو("RW");
    افتراضي:
        اكسر;
    }

    إذا (!XN) {
        لکو("X");
    }

    إذا (!TEX) {
        لکو(", uncached");
    }

    لکو(")\n");
}

ساكن فراغ arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_صفحو_سائيز_t صفحو_سائيز;
    لفظ_t i = 0;
    طالما (i < ٻٽو_انگ(PT_اشاريو_بٽ + صفحو_بٽ)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                صفحو_سائيز = ARMLarge_صفحو;
            } وإلا {
                صفحو_سائيز = ARMSmall_صفحو;
            }
#وإلا
            صفحو_سائيز = ARMSmall_صفحو;
#جيڪڏهن_پورو
            لکو("0x%lx: frame_%p_%04lu ", PT_اشاريو(i), pte, PT_اشاريو(i));
            cap_چوکٺو_print_attrs_pt(pte);
            اكسر;
        }
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            صفحو_سائيز = ARMLarge_صفحو;
            لکو("0x%lx: frame_%p_%04lu ", PT_اشاريو(i), pte, PT_اشاريو(i));
            cap_چوکٺو_print_attrs_pt(pte);
            اكسر;
        }
#جيڪڏهن_پورو
        افتراضي:
            صفحو_سائيز = ARMSmall_صفحو;
        }
        i += (1 << صفحو_بٽ_For_سائيز(صفحو_سائيز));
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_پاڙ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڙيو_منزل_ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_جو_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_صفحو_سائيز_t صفحو_سائيز;
        لکو("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        لفظ_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_سائيز(pde)) {
                    صفحو_سائيز = ARMSuperSection;
                } وإلا {
                    صفحو_سائيز = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    صفحو_سائيز = ARMSuperSection;
                } وإلا {
                    صفحو_سائيز = ARMSection;
                }
#جيڪڏهن_پورو
                لکو("0x%lx: frame_%p_%04lu ", PD_اشاريو(i), pde, PD_اشاريو(i));
                cap_چوکٺو_print_attrs_pd(pde);
                اكسر;
            }

            حالة pde_pde_coarse: {
                لکو("0x%lx: pt_%p_%04lu\n", PD_اشاريو(i), pde, PD_اشاريو(i));
                صفحو_سائيز = ARMSection;
                اكسر;
            }
            افتراضي:
                صفحو_سائيز = ARMSection;
                اكسر;
            }
            i += (1 << صفحو_بٽ_For_سائيز(صفحو_سائيز));
            إذا (i < (1 << صفحو_بٽ_For_سائيز(صفحو_سائيز))) {
                اكسر; /* overflowed */
            }
        }
        لکو("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            إذا (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتو(pde));
                لکو("pt_%p_%04lu {\n", pde, PD_اشاريو(i));
                arm32_cap_pt_print_slots(pt);
                لکو("}\n"); /* pt */
            }
            i += (1 << صفحو_بٽ_For_سائيز(ARMSection));
            إذا (i < (1 << صفحو_بٽ_For_سائيز(ARMSection))) {
                اكسر; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکٺو_print_attrs_vptr(لفظ_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section: {
        لکو("frame_%p_%04lu ", pde, PD_اشاريو(vptr));
        cap_چوکٺو_print_attrs_pd(pde);
        اكسر;
    }
    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتو(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            لکو("frame_%p_%04lu ", pte, PT_اشاريو(vptr));
            cap_چوکٺو_print_attrs_pt(pte);
            اكسر;
        }

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            لکو("frame_%p_%04lu ", pte, PT_اشاريو(vptr));
            cap_چوکٺو_print_attrs_pt(pte);
            اكسر;
        }
#جيڪڏهن_پورو
        افتراضي:
            أكد(0);
        }
        اكسر;
    }
    افتراضي:
        أكد(0);
    }
}

فراغ print_ipc_بفر_جڳهه(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_صفحو_directory_cap_get_cap_PDMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    ڳوليو_PDFor_ASID_ret_t ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
    لکو("ipc_buffer_slot: ");
    cap_چوکٺو_print_attrs_vptr(vptr, ڳوليو_ret.pd);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحو_جدول_cap: {
        asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap);
        ڳوليو_PDFor_ASID_ret_t ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
        vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap);
        إذا (asid) {
            لکو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(ڳوليو_ret.pd, vptr), PD_اشاريو(vptr), (ڊگهو بنانشان صحيح)asid);
        } وإلا {
            لکو("pt_%p_%04lu\n", lookupPDSlot(ڳوليو_ret.pd, vptr), PD_اشاريو(vptr));
        }
        اكسر;
    }
    حالة cap_صفحو_directory_cap: {
        asid_t asid = cap_صفحو_directory_cap_get_cap_PDMapped_ASID(cap);
        ڳوليو_PDFor_ASID_ret_t ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
        إذا (asid) {
            لکو("%p_pd (asid: %lu)\n",
                   ڳوليو_ret.pd, (ڊگهو بنانشان صحيح)asid);
        } وإلا {
            لکو("%p_pd\n", ڳوليو_ret.pd);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        لکو("asid_control\n");
        اكسر;
    }
    حالة cap_small_چوکٺو_cap: {
        vptr_t vptr = cap_small_چوکٺو_cap_get_cap_FMapped_پتو(cap);
        ڳوليو_PDFor_ASID_ret_t ڳوليو_ret = ڳوليو_PDFor_ASID(cap_small_چوکٺو_cap_get_cap_FMapped_ASID(cap));
        أكد(ڳوليو_ret.حالت == EXCEPTION_NONE);
        cap_چوکٺو_print_attrs_vptr(vptr, ڳوليو_ret.pd);
        اكسر;
    }
    حالة cap_چوکٺو_cap: {
        vptr_t vptr = cap_چوکٺو_cap_get_cap_FMapped_پتو(cap);
        ڳوليو_PDFor_ASID_ret_t ڳوليو_ret = ڳوليو_PDFor_ASID(cap_چوکٺو_cap_get_cap_FMapped_ASID(cap));
        أكد(ڳوليو_ret.حالت == EXCEPTION_NONE);
        cap_چوکٺو_print_attrs_vptr(vptr, ڳوليو_ret.pd);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        لکو("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        لکو("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جيڪڏهن_پورو

        /* ARM specific caps */
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        لکو("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#جيڪڏهن_پورو
    افتراضي: {
        لکو("[unknown cap %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_چوکٺو_cap:
    حالة cap_small_چوکٺو_cap:
    حالة cap_صفحو_جدول_cap:
    حالة cap_صفحو_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        لکو("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        لکو("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جيڪڏهن_پورو
        /* ARM specific objects */
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        لکو("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_پورو
    افتراضي: {
        لکو("[unknown object %u]\n", cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_چوکٺو_print_attrs(resolve_ret_t ret)
{
    لکو("(");

    /* VM size */
    بدل (ret.چوکٺو_سائيز) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة ARMSection:
        لکو("2M");
        اكسر;
    حالة ARMSuperSection:
        لکو("32M");
        اكسر;
#وإلا
    حالة ARMSection:
        لکو("1M");
        اكسر;
    حالة ARMSuperSection:
        لکو("16M");
        اكسر;
#جيڪڏهن_پورو
    حالة ARMLarge_صفحو:
        لکو("64k");
        اكسر;
    حالة ARMSmall_صفحو:
        لکو("4k");
        اكسر;
    }

    لکو(", paddr: %p)\n", (فراغ *)ret.چوکٺو_بنياد);
}

ساكن فراغ arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    لفظ_t i = 0;
    طالما (i < ٻٽو_انگ(PT_اشاريو_بٽ + صفحو_بٽ)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small: {
            ret.چوکٺو_بنياد = pte_pte_small_ptr_get_پتو(pte);
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.چوکٺو_بنياد &= ~نقاب(صفحو_بٽ_For_سائيز(ARMLarge_صفحو));
                ret.چوکٺو_سائيز = ARMLarge_صفحو;
            } وإلا {
                ret.چوکٺو_سائيز = ARMSmall_صفحو;
            }
#وإلا
            ret.چوکٺو_سائيز = ARMSmall_صفحو;
#جيڪڏهن_پورو
            لکو("frame_%p_%04lu = frame ", pte, PT_اشاريو(i));
            obj_چوکٺو_print_attrs(ret);
            اكسر;
        }
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large: {
            ret.چوکٺو_بنياد = pte_pte_large_ptr_get_پتو(pte);
            ret.چوکٺو_سائيز = ARMLarge_صفحو;
            لکو("frame_%p_%04lu = frame ", pte, PT_اشاريو(i));
            obj_چوکٺو_print_attrs(ret);
            اكسر;
        }
#جيڪڏهن_پورو
        افتراضي:
            ret.چوکٺو_سائيز = ARMSmall_صفحو;
        }
        i += (1 << صفحو_بٽ_For_سائيز(ret.چوکٺو_سائيز));
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_پاڙ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڙيو_منزل_ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_جو_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        لکو("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        لفظ_t i = 0;
        طالما (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            بدل (pde_ptr_get_pdeType(pde)) {
            حالة pde_pde_section: {
                ret.چوکٺو_بنياد = pde_pde_section_ptr_get_پتو(pde);
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                إذا (pde_pde_section_ptr_get_سائيز(pde)) {
                    ret.چوکٺو_سائيز = ARMSuperSection;
                } وإلا {
                    ret.چوکٺو_سائيز = ARMSection;
                }
#وإلا
                إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.چوکٺو_بنياد &= ~نقاب(صفحو_بٽ_For_سائيز(ARMSuperSection));
                    ret.چوکٺو_سائيز = ARMSuperSection;
                } وإلا {
                    ret.چوکٺو_سائيز = ARMSection;
                }
#جيڪڏهن_پورو
                لکو("frame_%p_%04lu = frame ", pde, PD_اشاريو(i));
                obj_چوکٺو_print_attrs(ret);
                اكسر;
            }

            حالة pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتو(pde));
                لکو("pt_%p_%04lu = pt\n", pde, PD_اشاريو(i));
                arm32_obj_pt_print_slots(pt);
                ret.چوکٺو_سائيز = ARMSection;
                اكسر;
            }
            افتراضي:
                ret.چوکٺو_سائيز = ARMSection;
                اكسر;
            }
            i += (1 << صفحو_بٽ_For_سائيز(ret.چوکٺو_سائيز));
            إذا (i < (1 << صفحو_بٽ_For_سائيز(ret.چوکٺو_سائيز))) {
                اكسر; /* overflowed */
            }
        }
    }
}

#جيڪڏهن_پورو /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    لکو("arch aarch32\n");
    لکو("objects {\n");
#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
    print_objects();
#جيڪڏهن_پورو
    لکو("}\n");

    لکو("caps {\n");

    /* reset the seen list */
    reset_ڏٺل_list();

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
    print_caps();
    لکو("}\n");

    obj_irq_print_maps();
#جيڪڏهن_پورو /* CONFIG_PRINTING */
}


#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
