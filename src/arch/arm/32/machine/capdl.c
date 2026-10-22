/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/capdl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_UL          0xffffffff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PT_ਸੂਚਕਾਂਕ(vptr)  ((vptr >> ਪੰਨਾ_ਬਿੱਟ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PD_ਸੂਚਕਾਂਕ(vptr)  (vptr >> (ਪੰਨਾ_ਬਿੱਟ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ))

ਸ਼ਬਦ_t get_tcb_sp(tcb_t *tcb)
{
    ਵਾਪਸ tcb->tcbArch.tcbContext.registers[SP];
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING

ਸਥਿਰ ਖਾਲੀ obj_ਚੌਖਟਾ_print_attrs(resolve_ret_t ret);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pt(pte_t *pte);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pd(pde_t *pde);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_impl(ਸ਼ਬਦ_t AP, ਸ਼ਬਦ_t XN, ਸ਼ਬਦ_t TEX);
ਸਥਿਰ ਖਾਲੀ arm32_obj_pt_print_slots(pte_t *pt);
ਸਥਿਰ ਖਾਲੀ arm32_cap_pt_print_slots(pte_t *pt);

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
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pt(pte_t *pte)
{
    ਸ਼ਬਦ_t AP, XN, TEX;
    ਚੋਣ (pte_ptr_get_pteType(pte)) {
    ਹਾਲਤ pte_pte_small:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#ਨਹੀਂਤਾਂ
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#ਜੇ_ਅੰਤ
        XN = pte_pte_small_ptr_get_XN(pte);
        ਰੋਕੋ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        ਪ੍ਰਤਿਜ੍ਞਾ(!"should not happend");
    }
    cap_ਚੌਖਟਾ_print_attrs_impl(AP, XN, TEX);
}

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pd(pde_t *pde)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_ਚੌਖਟਾ_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#ਨਹੀਂਤਾਂ
    cap_ਚੌਖਟਾ_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_impl(ਸ਼ਬਦ_t AP, ਸ਼ਬਦ_t XN, ਸ਼ਬਦ_t TEX)
{
    ਲਿਖੋ("(");

    /* rights */
    ਚੋਣ (AP) {
    ਹਾਲਤ 0b00:
    ਹਾਲਤ 0b01:
        ਰੋਕੋ;
    ਹਾਲਤ 0b10:
        ਲਿਖੋ("R");
        ਰੋਕੋ;
    ਹਾਲਤ 0b11:
        ਲਿਖੋ("RW");
    ਮੂਲ:
        ਰੋਕੋ;
    }

    ਜੇ (!XN) {
        ਲਿਖੋ("X");
    }

    ਜੇ (!TEX) {
        ਲਿਖੋ(", uncached");
    }

    ਲਿਖੋ(")\n");
}

ਸਥਿਰ ਖਾਲੀ arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ;
    ਸ਼ਬਦ_t i = 0;
    ਜਦੋਂ (i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ਚੋਣ (pte_ptr_get_pteType(pte)) {
        ਹਾਲਤ pte_pte_small: {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
            ਜੇ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                ਪੰਨਾ_ਮਾਪ = ARMLarge_ਪੰਨਾ;
            } ਨਹੀਂਤਾਂ {
                ਪੰਨਾ_ਮਾਪ = ARMSmall_ਪੰਨਾ;
            }
#ਨਹੀਂਤਾਂ
            ਪੰਨਾ_ਮਾਪ = ARMSmall_ਪੰਨਾ;
#ਜੇ_ਅੰਤ
            ਲਿਖੋ("0x%lx: frame_%p_%04lu ", PT_ਸੂਚਕਾਂਕ(i), pte, PT_ਸੂਚਕਾਂਕ(i));
            cap_ਚੌਖਟਾ_print_attrs_pt(pte);
            ਰੋਕੋ;
        }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਹਾਲਤ pte_pte_large: {
            ਪੰਨਾ_ਮਾਪ = ARMLarge_ਪੰਨਾ;
            ਲਿਖੋ("0x%lx: frame_%p_%04lu ", PT_ਸੂਚਕਾਂਕ(i), pte, PT_ਸੂਚਕਾਂਕ(i));
            cap_ਚੌਖਟਾ_print_attrs_pt(pte);
            ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ
        ਮੂਲ:
            ਪੰਨਾ_ਮਾਪ = ARMSmall_ਪੰਨਾ;
        }
        i += (1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਪੰਨਾ_ਮਾਪ));
    }
}

ਖਾਲੀ obj_vtable_print_slots(tcb_t *tcb)
{
    ਜੇ (is_Valid_VTable_ਜੜ੍ਹ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_ਦਾ_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ;
        ਲਿਖੋ("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ਸ਼ਬਦ_t i = 0;
        ਜਦੋਂ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ਚੋਣ (pde_ptr_get_pdeType(pde)) {
            ਹਾਲਤ pde_pde_section: {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                ਜੇ (pde_pde_section_ptr_get_ਮਾਪ(pde)) {
                    ਪੰਨਾ_ਮਾਪ = ARMSuperSection;
                } ਨਹੀਂਤਾਂ {
                    ਪੰਨਾ_ਮਾਪ = ARMSection;
                }
#ਨਹੀਂਤਾਂ
                ਜੇ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    ਪੰਨਾ_ਮਾਪ = ARMSuperSection;
                } ਨਹੀਂਤਾਂ {
                    ਪੰਨਾ_ਮਾਪ = ARMSection;
                }
#ਜੇ_ਅੰਤ
                ਲਿਖੋ("0x%lx: frame_%p_%04lu ", PD_ਸੂਚਕਾਂਕ(i), pde, PD_ਸੂਚਕਾਂਕ(i));
                cap_ਚੌਖਟਾ_print_attrs_pd(pde);
                ਰੋਕੋ;
            }

            ਹਾਲਤ pde_pde_coarse: {
                ਲਿਖੋ("0x%lx: pt_%p_%04lu\n", PD_ਸੂਚਕਾਂਕ(i), pde, PD_ਸੂਚਕਾਂਕ(i));
                ਪੰਨਾ_ਮਾਪ = ARMSection;
                ਰੋਕੋ;
            }
            ਮੂਲ:
                ਪੰਨਾ_ਮਾਪ = ARMSection;
                ਰੋਕੋ;
            }
            i += (1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਪੰਨਾ_ਮਾਪ));
            ਜੇ (i < (1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਪੰਨਾ_ਮਾਪ))) {
                ਰੋਕੋ; /* overflowed */
            }
        }
        ਲਿਖੋ("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ਜਦੋਂ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ਜੇ (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ਪਤਾ(pde));
                ਲਿਖੋ("pt_%p_%04lu {\n", pde, PD_ਸੂਚਕਾਂਕ(i));
                arm32_cap_pt_print_slots(pt);
                ਲਿਖੋ("}\n"); /* pt */
            }
            i += (1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection));
            ਜੇ (i < (1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection))) {
                ਰੋਕੋ; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr(ਸ਼ਬਦ_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    ਚੋਣ (pde_ptr_get_pdeType(pde)) {
    ਹਾਲਤ pde_pde_section: {
        ਲਿਖੋ("frame_%p_%04lu ", pde, PD_ਸੂਚਕਾਂਕ(vptr));
        cap_ਚੌਖਟਾ_print_attrs_pd(pde);
        ਰੋਕੋ;
    }
    ਹਾਲਤ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ਪਤਾ(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        ਚੋਣ (pte_ptr_get_pteType(pte)) {
        ਹਾਲਤ pte_pte_small: {
            ਲਿਖੋ("frame_%p_%04lu ", pte, PT_ਸੂਚਕਾਂਕ(vptr));
            cap_ਚੌਖਟਾ_print_attrs_pt(pte);
            ਰੋਕੋ;
        }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਹਾਲਤ pte_pte_large: {
            ਲਿਖੋ("frame_%p_%04lu ", pte, PT_ਸੂਚਕਾਂਕ(vptr));
            cap_ਚੌਖਟਾ_print_attrs_pt(pte);
            ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ
        ਮੂਲ:
            ਪ੍ਰਤਿਜ੍ਞਾ(0);
        }
        ਰੋਕੋ;
    }
    ਮੂਲ:
        ਪ੍ਰਤਿਜ੍ਞਾ(0);
    }
}

ਖਾਲੀ print_ipc_ਬਫਰ_ਥਾਂ(tcb_t *tcb)
{
    ਸ਼ਬਦ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
    ਲਿਖੋ("ipc_buffer_slot: ");
    cap_ਚੌਖਟਾ_print_attrs_vptr(vptr, ਲੱਭੋ_ret.pd);
}

ਖਾਲੀ print_cap_arch(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap: {
        asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap);
        ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
        vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap);
        ਜੇ (asid) {
            ਲਿਖੋ("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(ਲੱਭੋ_ret.pd, vptr), PD_ਸੂਚਕਾਂਕ(vptr), (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("pt_%p_%04lu\n", lookupPDSlot(ਲੱਭੋ_ret.pd, vptr), PD_ਸੂਚਕਾਂਕ(vptr));
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap: {
        asid_t asid = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(cap);
        ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
        ਜੇ (asid) {
            ਲਿਖੋ("%p_pd (asid: %lu)\n",
                   ਲੱਭੋ_ret.pd, (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("%p_pd\n", ਲੱਭੋ_ret.pd);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_asid_control_cap: {
        /* only one in the system */
        ਲਿਖੋ("asid_control\n");
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_small_ਚੌਖਟਾ_cap: {
        vptr_t vptr = cap_small_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
        ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(cap_small_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਲੱਭੋ_ret.ਸਥਿਤੀ == EXCEPTION_NONE);
        cap_ਚੌਖਟਾ_print_attrs_vptr(vptr, ਲੱਭੋ_ret.pd);
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap: {
        vptr_t vptr = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
        ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਲੱਭੋ_ret.ਸਥਿਤੀ == EXCEPTION_NONE);
        cap_ਚੌਖਟਾ_print_attrs_vptr(vptr, ਲੱਭੋ_ret.pd);
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_asid_pool_cap: {
        ਲਿਖੋ("%p_asid_pool\n", (ਖਾਲੀ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap: {
        ਲਿਖੋ("%p_vcpu\n", (ਖਾਲੀ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ

        /* ARM specific caps */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ cap_io_space_cap: {
        ਲਿਖੋ("%p_io_space\n", (ਖਾਲੀ *)cap_io_space_cap_get_capModuleID(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਮੂਲ: {
        ਲਿਖੋ("[unknown cap %u]\n", cap_get_capType(cap));
        ਰੋਕੋ;
    }
    }
}

ਖਾਲੀ print_object_arch(cap_t cap)
{

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
    ਹਾਲਤ cap_small_ਚੌਖਟਾ_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ਰੋਕੋ;

    ਹਾਲਤ cap_asid_pool_cap: {
        ਲਿਖੋ("%p_asid_pool = asid_pool ",
               (ਖਾਲੀ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap: {
        ਲਿਖੋ("%p_vcpu = vcpu\n", (ਖਾਲੀ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
        /* ARM specific objects */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ cap_io_space_cap: {
        ਲਿਖੋ("%p_io_space = io_space ", (ਖਾਲੀ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਮੂਲ: {
        ਲਿਖੋ("[unknown object %u]\n", cap_get_capType(cap));
        ਰੋਕੋ;
    }
    }
}

ਸਥਿਰ ਖਾਲੀ obj_ਚੌਖਟਾ_print_attrs(resolve_ret_t ret)
{
    ਲਿਖੋ("(");

    /* VM size */
    ਚੋਣ (ret.ਚੌਖਟਾ_ਮਾਪ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ ARMSection:
        ਲਿਖੋ("2M");
        ਰੋਕੋ;
    ਹਾਲਤ ARMSuperSection:
        ਲਿਖੋ("32M");
        ਰੋਕੋ;
#ਨਹੀਂਤਾਂ
    ਹਾਲਤ ARMSection:
        ਲਿਖੋ("1M");
        ਰੋਕੋ;
    ਹਾਲਤ ARMSuperSection:
        ਲਿਖੋ("16M");
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਹਾਲਤ ARMLarge_ਪੰਨਾ:
        ਲਿਖੋ("64k");
        ਰੋਕੋ;
    ਹਾਲਤ ARMSmall_ਪੰਨਾ:
        ਲਿਖੋ("4k");
        ਰੋਕੋ;
    }

    ਲਿਖੋ(", paddr: %p)\n", (ਖਾਲੀ *)ret.ਚੌਖਟਾ_ਅਧਾਰ);
}

ਸਥਿਰ ਖਾਲੀ arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    ਸ਼ਬਦ_t i = 0;
    ਜਦੋਂ (i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ਚੋਣ (pte_ptr_get_pteType(pte)) {
        ਹਾਲਤ pte_pte_small: {
            ret.ਚੌਖਟਾ_ਅਧਾਰ = pte_pte_small_ptr_get_ਪਤਾ(pte);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
            ਜੇ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.ਚੌਖਟਾ_ਅਧਾਰ &= ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMLarge_ਪੰਨਾ));
                ret.ਚੌਖਟਾ_ਮਾਪ = ARMLarge_ਪੰਨਾ;
            } ਨਹੀਂਤਾਂ {
                ret.ਚੌਖਟਾ_ਮਾਪ = ARMSmall_ਪੰਨਾ;
            }
#ਨਹੀਂਤਾਂ
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMSmall_ਪੰਨਾ;
#ਜੇ_ਅੰਤ
            ਲਿਖੋ("frame_%p_%04lu = frame ", pte, PT_ਸੂਚਕਾਂਕ(i));
            obj_ਚੌਖਟਾ_print_attrs(ret);
            ਰੋਕੋ;
        }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਹਾਲਤ pte_pte_large: {
            ret.ਚੌਖਟਾ_ਅਧਾਰ = pte_pte_large_ptr_get_ਪਤਾ(pte);
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMLarge_ਪੰਨਾ;
            ਲਿਖੋ("frame_%p_%04lu = frame ", pte, PT_ਸੂਚਕਾਂਕ(i));
            obj_ਚੌਖਟਾ_print_attrs(ret);
            ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ
        ਮੂਲ:
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMSmall_ਪੰਨਾ;
        }
        i += (1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ret.ਚੌਖਟਾ_ਮਾਪ));
    }
}

ਖਾਲੀ obj_tcb_print_vtable(tcb_t *tcb)
{
    ਜੇ (is_Valid_VTable_ਜੜ੍ਹ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_ਦਾ_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        ਲਿਖੋ("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        ਸ਼ਬਦ_t i = 0;
        ਜਦੋਂ (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ਚੋਣ (pde_ptr_get_pdeType(pde)) {
            ਹਾਲਤ pde_pde_section: {
                ret.ਚੌਖਟਾ_ਅਧਾਰ = pde_pde_section_ptr_get_ਪਤਾ(pde);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                ਜੇ (pde_pde_section_ptr_get_ਮਾਪ(pde)) {
                    ret.ਚੌਖਟਾ_ਮਾਪ = ARMSuperSection;
                } ਨਹੀਂਤਾਂ {
                    ret.ਚੌਖਟਾ_ਮਾਪ = ARMSection;
                }
#ਨਹੀਂਤਾਂ
                ਜੇ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.ਚੌਖਟਾ_ਅਧਾਰ &= ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSuperSection));
                    ret.ਚੌਖਟਾ_ਮਾਪ = ARMSuperSection;
                } ਨਹੀਂਤਾਂ {
                    ret.ਚੌਖਟਾ_ਮਾਪ = ARMSection;
                }
#ਜੇ_ਅੰਤ
                ਲਿਖੋ("frame_%p_%04lu = frame ", pde, PD_ਸੂਚਕਾਂਕ(i));
                obj_ਚੌਖਟਾ_print_attrs(ret);
                ਰੋਕੋ;
            }

            ਹਾਲਤ pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ਪਤਾ(pde));
                ਲਿਖੋ("pt_%p_%04lu = pt\n", pde, PD_ਸੂਚਕਾਂਕ(i));
                arm32_obj_pt_print_slots(pt);
                ret.ਚੌਖਟਾ_ਮਾਪ = ARMSection;
                ਰੋਕੋ;
            }
            ਮੂਲ:
                ret.ਚੌਖਟਾ_ਮਾਪ = ARMSection;
                ਰੋਕੋ;
            }
            i += (1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ret.ਚੌਖਟਾ_ਮਾਪ));
            ਜੇ (i < (1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ret.ਚੌਖਟਾ_ਮਾਪ))) {
                ਰੋਕੋ; /* overflowed */
            }
        }
    }
}

#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

ਖਾਲੀ debug_capDL(ਖਾਲੀ)
{
    ਲਿਖੋ("arch aarch32\n");
    ਲਿਖੋ("objects {\n");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    print_objects();
#ਜੇ_ਅੰਤ
    ਲਿਖੋ("}\n");

    ਲਿਖੋ("caps {\n");

    /* reset the seen list */
    reset_ਵੇਖਿਆ_list();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    print_caps();
    ਲਿਖੋ("}\n");

    obj_irq_print_maps();
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */
}


#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
