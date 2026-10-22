/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD

#شامل <arch/machine/capdl.h>
#شامل <string.h>
#شامل <kernel/cspace.h>

#تعریف_کرو MAX_UL          0xffffffff
#تعریف_کرو PT_اشاریہ(vptr)  ((vptr >> صفحہ_بٹ) & نقاب(PT_اشاریہ_بٹ))
#تعریف_کرو PD_اشاریہ(vptr)  (vptr >> (صفحہ_بٹ + PT_اشاریہ_بٹ))

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    واپس tcb->tcbArch.tcbContext.registers[SP];
}

#اگر_تعریف_شدہ CONFIG_PRINTING

ساکن خالی obj_چوکھٹا_print_attrs(resolve_ret_t ret);
ساکن خالی cap_چوکھٹا_print_attrs_pt(pte_t *pte);
ساکن خالی cap_چوکھٹا_print_attrs_pd(pde_t *pde);
ساکن خالی cap_چوکھٹا_print_attrs_impl(لفظ_t AP, لفظ_t XN, لفظ_t TEX);
ساکن خالی arm32_obj_pt_print_slots(pte_t *pt);
ساکن خالی arm32_cap_pt_print_slots(pte_t *pt);

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
ساکن خالی cap_چوکھٹا_print_attrs_pt(pte_t *pte)
{
    لفظ_t AP, XN, TEX;
    انتخاب (pte_ptr_get_pteType(pte)) {
    حالت pte_pte_small:
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#ورنہ
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#اگر_ختم
        XN = pte_pte_small_ptr_get_XN(pte);
        توڑو;

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        توڑو;
#اگر_ختم
    طےشدہ:
        أكد(!"should not happend");
    }
    cap_چوکھٹا_print_attrs_impl(AP, XN, TEX);
}

ساکن خالی cap_چوکھٹا_print_attrs_pd(pde_t *pde)
{
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_چوکھٹا_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#ورنہ
    cap_چوکھٹا_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#اگر_ختم
}

ساکن خالی cap_چوکھٹا_print_attrs_impl(لفظ_t AP, لفظ_t XN, لفظ_t TEX)
{
    لکھو("(");

    /* rights */
    انتخاب (AP) {
    حالت 0b00:
    حالت 0b01:
        توڑو;
    حالت 0b10:
        لکھو("R");
        توڑو;
    حالت 0b11:
        لکھو("RW");
    طےشدہ:
        توڑو;
    }

    اگر (!XN) {
        لکھو("X");
    }

    اگر (!TEX) {
        لکھو(", uncached");
    }

    لکھو(")\n");
}

ساکن خالی arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_صفحہ_سائز_t صفحہ_سائز;
    لفظ_t i = 0;
    جب (i < ثنائی_ہندسہ(PT_اشاریہ_بٹ + صفحہ_بٹ)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        انتخاب (pte_ptr_get_pteType(pte)) {
        حالت pte_pte_small: {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            اگر (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                صفحہ_سائز = ARMLarge_صفحہ;
            } ورنہ {
                صفحہ_سائز = ARMSmall_صفحہ;
            }
#ورنہ
            صفحہ_سائز = ARMSmall_صفحہ;
#اگر_ختم
            لکھو("0x%lx: frame_%p_%04lu ", PT_اشاریہ(i), pte, PT_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pt(pte);
            توڑو;
        }
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        حالت pte_pte_large: {
            صفحہ_سائز = ARMLarge_صفحہ;
            لکھو("0x%lx: frame_%p_%04lu ", PT_اشاریہ(i), pte, PT_اشاریہ(i));
            cap_چوکھٹا_print_attrs_pt(pte);
            توڑو;
        }
#اگر_ختم
        طےشدہ:
            صفحہ_سائز = ARMSmall_صفحہ;
        }
        i += (1 << صفحہ_بٹ_For_سائز(صفحہ_سائز));
    }
}

خالی obj_vtable_print_slots(tcb_t *tcb)
{
    اگر (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_مقصد_نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_کا_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_صفحہ_سائز_t صفحہ_سائز;
        لکھو("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        لفظ_t i = 0;
        جب (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            انتخاب (pde_ptr_get_pdeType(pde)) {
            حالت pde_pde_section: {
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                اگر (pde_pde_section_ptr_get_سائز(pde)) {
                    صفحہ_سائز = ARMSuperSection;
                } ورنہ {
                    صفحہ_سائز = ARMSection;
                }
#ورنہ
                اگر (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    صفحہ_سائز = ARMSuperSection;
                } ورنہ {
                    صفحہ_سائز = ARMSection;
                }
#اگر_ختم
                لکھو("0x%lx: frame_%p_%04lu ", PD_اشاریہ(i), pde, PD_اشاریہ(i));
                cap_چوکھٹا_print_attrs_pd(pde);
                توڑو;
            }

            حالت pde_pde_coarse: {
                لکھو("0x%lx: pt_%p_%04lu\n", PD_اشاریہ(i), pde, PD_اشاریہ(i));
                صفحہ_سائز = ARMSection;
                توڑو;
            }
            طےشدہ:
                صفحہ_سائز = ARMSection;
                توڑو;
            }
            i += (1 << صفحہ_بٹ_For_سائز(صفحہ_سائز));
            اگر (i < (1 << صفحہ_بٹ_For_سائز(صفحہ_سائز))) {
                توڑو; /* overflowed */
            }
        }
        لکھو("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        جب (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            اگر (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pde));
                لکھو("pt_%p_%04lu {\n", pde, PD_اشاریہ(i));
                arm32_cap_pt_print_slots(pt);
                لکھو("}\n"); /* pt */
            }
            i += (1 << صفحہ_بٹ_For_سائز(ARMSection));
            اگر (i < (1 << صفحہ_بٹ_For_سائز(ARMSection))) {
                توڑو; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ساکن خالی cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    انتخاب (pde_ptr_get_pdeType(pde)) {
    حالت pde_pde_section: {
        لکھو("frame_%p_%04lu ", pde, PD_اشاریہ(vptr));
        cap_چوکھٹا_print_attrs_pd(pde);
        توڑو;
    }
    حالت pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        انتخاب (pte_ptr_get_pteType(pte)) {
        حالت pte_pte_small: {
            لکھو("frame_%p_%04lu ", pte, PT_اشاریہ(vptr));
            cap_چوکھٹا_print_attrs_pt(pte);
            توڑو;
        }

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        حالت pte_pte_large: {
            لکھو("frame_%p_%04lu ", pte, PT_اشاریہ(vptr));
            cap_چوکھٹا_print_attrs_pt(pte);
            توڑو;
        }
#اگر_ختم
        طےشدہ:
            أكد(0);
        }
        توڑو;
    }
    طےشدہ:
        أكد(0);
    }
}

خالی print_ipc_بفر_خانہ(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
    لکھو("ipc_buffer_slot: ");
    cap_چوکھٹا_print_attrs_vptr(vptr, ڈھونڈو_ret.pd);
}

خالی print_cap_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_صفحہ_جدول_cap: {
        asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap);
        ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
        vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap);
        اگر (asid) {
            لکھو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(ڈھونڈو_ret.pd, vptr), PD_اشاریہ(vptr), (طویل غیرعلامتی صحیح)asid);
        } ورنہ {
            لکھو("pt_%p_%04lu\n", lookupPDSlot(ڈھونڈو_ret.pd, vptr), PD_اشاریہ(vptr));
        }
        توڑو;
    }
    حالت cap_صفحہ_directory_cap: {
        asid_t asid = cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(cap);
        ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
        اگر (asid) {
            لکھو("%p_pd (asid: %lu)\n",
                   ڈھونڈو_ret.pd, (طویل غیرعلامتی صحیح)asid);
        } ورنہ {
            لکھو("%p_pd\n", ڈھونڈو_ret.pd);
        }
        توڑو;
    }
    حالت cap_asid_control_cap: {
        /* only one in the system */
        لکھو("asid_control\n");
        توڑو;
    }
    حالت cap_small_چوکھٹا_cap: {
        vptr_t vptr = cap_small_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap));
        أكد(ڈھونڈو_ret.حالت_2 == EXCEPTION_NONE);
        cap_چوکھٹا_print_attrs_vptr(vptr, ڈھونڈو_ret.pd);
        توڑو;
    }
    حالت cap_چوکھٹا_cap: {
        vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap));
        أكد(ڈھونڈو_ret.حالت_2 == EXCEPTION_NONE);
        cap_چوکھٹا_print_attrs_vptr(vptr, ڈھونڈو_ret.pd);
        توڑو;
    }
    حالت cap_asid_pool_cap: {
        لکھو("%p_asid_pool\n", (خالی *)cap_asid_pool_cap_get_capASIDPool(cap));
        توڑو;
    }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap: {
        لکھو("%p_vcpu\n", (خالی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        توڑو;
    }
#اگر_ختم

        /* ARM specific caps */
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت cap_io_space_cap: {
        لکھو("%p_io_space\n", (خالی *)cap_io_space_cap_get_capModuleID(cap));
        توڑو;
    }
#اگر_ختم
    طےشدہ: {
        لکھو("[unknown cap %u]\n", cap_get_capType(cap));
        توڑو;
    }
    }
}

خالی print_object_arch(cap_t cap)
{

    انتخاب (cap_get_capType(cap)) {
    حالت cap_چوکھٹا_cap:
    حالت cap_small_چوکھٹا_cap:
    حالت cap_صفحہ_جدول_cap:
    حالت cap_صفحہ_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        توڑو;

    حالت cap_asid_pool_cap: {
        لکھو("%p_asid_pool = asid_pool ",
               (خالی *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        توڑو;
    }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap: {
        لکھو("%p_vcpu = vcpu\n", (خالی *)cap_vcpu_cap_get_capVCPUPtr(cap));
        توڑو;
    }
#اگر_ختم
        /* ARM specific objects */
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت cap_io_space_cap: {
        لکھو("%p_io_space = io_space ", (خالی *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        توڑو;
    }
#اگر_ختم
    طےشدہ: {
        لکھو("[unknown object %u]\n", cap_get_capType(cap));
        توڑو;
    }
    }
}

ساکن خالی obj_چوکھٹا_print_attrs(resolve_ret_t ret)
{
    لکھو("(");

    /* VM size */
    انتخاب (ret.چوکھٹا_سائز) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت ARMSection:
        لکھو("2M");
        توڑو;
    حالت ARMSuperSection:
        لکھو("32M");
        توڑو;
#ورنہ
    حالت ARMSection:
        لکھو("1M");
        توڑو;
    حالت ARMSuperSection:
        لکھو("16M");
        توڑو;
#اگر_ختم
    حالت ARMLarge_صفحہ:
        لکھو("64k");
        توڑو;
    حالت ARMSmall_صفحہ:
        لکھو("4k");
        توڑو;
    }

    لکھو(", paddr: %p)\n", (خالی *)ret.چوکھٹا_بنیاد);
}

ساکن خالی arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    لفظ_t i = 0;
    جب (i < ثنائی_ہندسہ(PT_اشاریہ_بٹ + صفحہ_بٹ)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        انتخاب (pte_ptr_get_pteType(pte)) {
        حالت pte_pte_small: {
            ret.چوکھٹا_بنیاد = pte_pte_small_ptr_get_پتہ(pte);
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            اگر (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.چوکھٹا_بنیاد &= ~نقاب(صفحہ_بٹ_For_سائز(ARMLarge_صفحہ));
                ret.چوکھٹا_سائز = ARMLarge_صفحہ;
            } ورنہ {
                ret.چوکھٹا_سائز = ARMSmall_صفحہ;
            }
#ورنہ
            ret.چوکھٹا_سائز = ARMSmall_صفحہ;
#اگر_ختم
            لکھو("frame_%p_%04lu = frame ", pte, PT_اشاریہ(i));
            obj_چوکھٹا_print_attrs(ret);
            توڑو;
        }
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        حالت pte_pte_large: {
            ret.چوکھٹا_بنیاد = pte_pte_large_ptr_get_پتہ(pte);
            ret.چوکھٹا_سائز = ARMLarge_صفحہ;
            لکھو("frame_%p_%04lu = frame ", pte, PT_اشاریہ(i));
            obj_چوکھٹا_print_attrs(ret);
            توڑو;
        }
#اگر_ختم
        طےشدہ:
            ret.چوکھٹا_سائز = ARMSmall_صفحہ;
        }
        i += (1 << صفحہ_بٹ_For_سائز(ret.چوکھٹا_سائز));
    }
}

خالی obj_tcb_print_vtable(tcb_t *tcb)
{
    اگر (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_مقصد_نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_کا_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        لکھو("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        لفظ_t i = 0;
        جب (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            انتخاب (pde_ptr_get_pdeType(pde)) {
            حالت pde_pde_section: {
                ret.چوکھٹا_بنیاد = pde_pde_section_ptr_get_پتہ(pde);
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                اگر (pde_pde_section_ptr_get_سائز(pde)) {
                    ret.چوکھٹا_سائز = ARMSuperSection;
                } ورنہ {
                    ret.چوکھٹا_سائز = ARMSection;
                }
#ورنہ
                اگر (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.چوکھٹا_بنیاد &= ~نقاب(صفحہ_بٹ_For_سائز(ARMSuperSection));
                    ret.چوکھٹا_سائز = ARMSuperSection;
                } ورنہ {
                    ret.چوکھٹا_سائز = ARMSection;
                }
#اگر_ختم
                لکھو("frame_%p_%04lu = frame ", pde, PD_اشاریہ(i));
                obj_چوکھٹا_print_attrs(ret);
                توڑو;
            }

            حالت pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pde));
                لکھو("pt_%p_%04lu = pt\n", pde, PD_اشاریہ(i));
                arm32_obj_pt_print_slots(pt);
                ret.چوکھٹا_سائز = ARMSection;
                توڑو;
            }
            طےشدہ:
                ret.چوکھٹا_سائز = ARMSection;
                توڑو;
            }
            i += (1 << صفحہ_بٹ_For_سائز(ret.چوکھٹا_سائز));
            اگر (i < (1 << صفحہ_بٹ_For_سائز(ret.چوکھٹا_سائز))) {
                توڑو; /* overflowed */
            }
        }
    }
}

#اگر_ختم /* CONFIG_PRINTING */

خالی debug_capDL(خالی)
{
    لکھو("arch aarch32\n");
    لکھو("objects {\n");
#اگر_تعریف_شدہ CONFIG_PRINTING
    print_objects();
#اگر_ختم
    لکھو("}\n");

    لکھو("caps {\n");

    /* reset the seen list */
    reset_نظر_آیا_list();

#اگر_تعریف_شدہ CONFIG_PRINTING
    print_caps();
    لکھو("}\n");

    obj_irq_print_maps();
#اگر_ختم /* CONFIG_PRINTING */
}


#اگر_ختم /* CONFIG_DEBUG_BUILD */
