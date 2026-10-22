/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_DEBUG_BUILD

#అన్తర్భావ <arch/machine/capdl.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <kernel/cspace.h>

#నిర్వచించు MAX_UL          0xffffffff
#నిర్వచించు PT_సూచీ(vptr)  ((vptr >> పుట_బిట్లు) & ముసుగు(PT_సూచీ_బిట్లు))
#నిర్వచించు PD_సూచీ(vptr)  (vptr >> (పుట_బిట్లు + PT_సూచీ_బిట్లు))

మాట_t get_tcb_sp(tcb_t *tcb)
{
    తిరిగి tcb->tcbArch.tcbContext.registers[SP];
}

#నిర్వచితమైతే CONFIG_PRINTING

స్థిర శూన్య obj_చట్రం_print_attrs(resolve_ret_t ret);
స్థిర శూన్య cap_చట్రం_print_attrs_pt(pte_t *pte);
స్థిర శూన్య cap_చట్రం_print_attrs_pd(pde_t *pde);
స్థిర శూన్య cap_చట్రం_print_attrs_impl(మాట_t AP, మాట_t XN, మాట_t TEX);
స్థిర శూన్య arm32_obj_pt_print_slots(pte_t *pt);
స్థిర శూన్య arm32_cap_pt_print_slots(pte_t *pt);

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
స్థిర శూన్య cap_చట్రం_print_attrs_pt(pte_t *pte)
{
    మాట_t AP, XN, TEX;
    ఎంపిక (pte_ptr_get_pteType(pte)) {
    స్థితి pte_pte_small:
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        AP = pte_pte_small_ptr_get_HAP(pte);
        TEX = pte_pte_small_ptr_get_MemAttr(pte);
#లేకపోతే
        AP = pte_pte_small_ptr_get_AP(pte);
        TEX = pte_pte_small_ptr_get_TEX(pte);
#అయితే_ముగింపు
        XN = pte_pte_small_ptr_get_XN(pte);
        ఆపు;

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి pte_pte_large:
        AP = pte_pte_large_ptr_get_AP(pte);
        TEX = pte_pte_large_ptr_get_TEX(pte);
        XN = pte_pte_large_ptr_get_XN(pte);
        ఆపు;
#అయితే_ముగింపు
    అప్రమేయ:
        ప్రతిజ్ఞా(!"should not happend");
    }
    cap_చట్రం_print_attrs_impl(AP, XN, TEX);
}

స్థిర శూన్య cap_చట్రం_print_attrs_pd(pde_t *pde)
{
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    cap_చట్రం_print_attrs_impl(pde_pde_section_ptr_get_HAP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_MemAttr(pde));
#లేకపోతే
    cap_చట్రం_print_attrs_impl(pde_pde_section_ptr_get_AP(pde),
                               pde_pde_section_ptr_get_XN(pde),
                               pde_pde_section_ptr_get_TEX(pde));
#అయితే_ముగింపు
}

స్థిర శూన్య cap_చట్రం_print_attrs_impl(మాట_t AP, మాట_t XN, మాట_t TEX)
{
    రాయి("(");

    /* rights */
    ఎంపిక (AP) {
    స్థితి 0b00:
    స్థితి 0b01:
        ఆపు;
    స్థితి 0b10:
        రాయి("R");
        ఆపు;
    స్థితి 0b11:
        రాయి("RW");
    అప్రమేయ:
        ఆపు;
    }

    ఒకవేళ (!XN) {
        రాయి("X");
    }

    ఒకవేళ (!TEX) {
        రాయి(", uncached");
    }

    రాయి(")\n");
}

స్థిర శూన్య arm32_cap_pt_print_slots(pte_t *pt)
{
    vm_పుట_కొలత_t పుట_కొలత;
    మాట_t i = 0;
    అయితే (i < బిట్టు(PT_సూచీ_బిట్లు + పుట_బిట్లు)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ఎంపిక (pte_ptr_get_pteType(pte)) {
        స్థితి pte_pte_small: {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
            ఒకవేళ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                పుట_కొలత = ARMLarge_పుట;
            } లేకపోతే {
                పుట_కొలత = ARMSmall_పుట;
            }
#లేకపోతే
            పుట_కొలత = ARMSmall_పుట;
#అయితే_ముగింపు
            రాయి("0x%lx: frame_%p_%04lu ", PT_సూచీ(i), pte, PT_సూచీ(i));
            cap_చట్రం_print_attrs_pt(pte);
            ఆపు;
        }
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        స్థితి pte_pte_large: {
            పుట_కొలత = ARMLarge_పుట;
            రాయి("0x%lx: frame_%p_%04lu ", PT_సూచీ(i), pte, PT_సూచీ(i));
            cap_చట్రం_print_attrs_pt(pte);
            ఆపు;
        }
#అయితే_ముగింపు
        అప్రమేయ:
            పుట_కొలత = ARMSmall_పుట;
        }
        i += (1 << పుట_బిట్లు_For_కొలత(పుట_కొలత));
    }
}

శూన్య obj_vtable_print_slots(tcb_t *tcb)
{
    ఒకవేళ (is_Valid_VTable_మూలం(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        కూడు_గమ్యం_చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_యొక్క_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vm_పుట_కొలత_t పుట_కొలత;
        రాయి("%p_pd {\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        మాట_t i = 0;
        అయితే (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ఎంపిక (pde_ptr_get_pdeType(pde)) {
            స్థితి pde_pde_section: {
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
                ఒకవేళ (pde_pde_section_ptr_get_కొలత(pde)) {
                    పుట_కొలత = ARMSuperSection;
                } లేకపోతే {
                    పుట_కొలత = ARMSection;
                }
#లేకపోతే
                ఒకవేళ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    పుట_కొలత = ARMSuperSection;
                } లేకపోతే {
                    పుట_కొలత = ARMSection;
                }
#అయితే_ముగింపు
                రాయి("0x%lx: frame_%p_%04lu ", PD_సూచీ(i), pde, PD_సూచీ(i));
                cap_చట్రం_print_attrs_pd(pde);
                ఆపు;
            }

            స్థితి pde_pde_coarse: {
                రాయి("0x%lx: pt_%p_%04lu\n", PD_సూచీ(i), pde, PD_సూచీ(i));
                పుట_కొలత = ARMSection;
                ఆపు;
            }
            అప్రమేయ:
                పుట_కొలత = ARMSection;
                ఆపు;
            }
            i += (1 << పుట_బిట్లు_For_కొలత(పుట_కొలత));
            ఒకవేళ (i < (1 << పుట_బిట్లు_For_కొలత(పుట_కొలత))) {
                ఆపు; /* overflowed */
            }
        }
        రాయి("}\n"); /* pd */

        i = 0;
        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        అయితే (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ఒకవేళ (pde_ptr_get_pdeType(pde) == pde_pde_coarse) {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_చిరునామా(pde));
                రాయి("pt_%p_%04lu {\n", pde, PD_సూచీ(i));
                arm32_cap_pt_print_slots(pt);
                రాయి("}\n"); /* pt */
            }
            i += (1 << పుట_బిట్లు_For_కొలత(ARMSection));
            ఒకవేళ (i < (1 << పుట_బిట్లు_For_కొలత(ARMSection))) {
                ఆపు; /* overflowed */
            }
        }
    }
}

/* use when only have access to vptr of frames */
స్థిర శూన్య cap_చట్రం_print_attrs_vptr(మాట_t vptr, pde_t *pd)
{
    pde_t *pde = lookupPDSlot(pd, vptr);

    ఎంపిక (pde_ptr_get_pdeType(pde)) {
    స్థితి pde_pde_section: {
        రాయి("frame_%p_%04lu ", pde, PD_సూచీ(vptr));
        cap_చట్రం_print_attrs_pd(pde);
        ఆపు;
    }
    స్థితి pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_చిరునామా(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vptr);
        ఎంపిక (pte_ptr_get_pteType(pte)) {
        స్థితి pte_pte_small: {
            రాయి("frame_%p_%04lu ", pte, PT_సూచీ(vptr));
            cap_చట్రం_print_attrs_pt(pte);
            ఆపు;
        }

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        స్థితి pte_pte_large: {
            రాయి("frame_%p_%04lu ", pte, PT_సూచీ(vptr));
            cap_చట్రం_print_attrs_pt(pte);
            ఆపు;
        }
#అయితే_ముగింపు
        అప్రమేయ:
            ప్రతిజ్ఞా(0);
        }
        ఆపు;
    }
    అప్రమేయ:
        ప్రతిజ్ఞా(0);
    }
}

శూన్య print_ipc_బఫరు_స్థానం(tcb_t *tcb)
{
    మాట_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_పుట_directory_cap_get_cap_PDMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    కనుగొను_PDFor_ASID_ret_t కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
    రాయి("ipc_buffer_slot: ");
    cap_చట్రం_print_attrs_vptr(vptr, కనుగొను_ret.pd);
}

శూన్య print_cap_arch(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_పట్టిక_cap: {
        asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap);
        కనుగొను_PDFor_ASID_ret_t కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
        vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap);
        ఒకవేళ (asid) {
            రాయి("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(కనుగొను_ret.pd, vptr), PD_సూచీ(vptr), (దీర్ఘ గుర్తులేని పూర్ణాంకం)asid);
        } లేకపోతే {
            రాయి("pt_%p_%04lu\n", lookupPDSlot(కనుగొను_ret.pd, vptr), PD_సూచీ(vptr));
        }
        ఆపు;
    }
    స్థితి cap_పుట_directory_cap: {
        asid_t asid = cap_పుట_directory_cap_get_cap_PDMapped_ASID(cap);
        కనుగొను_PDFor_ASID_ret_t కనుగొను_ret = కనుగొను_PDFor_ASID(asid);
        ఒకవేళ (asid) {
            రాయి("%p_pd (asid: %lu)\n",
                   కనుగొను_ret.pd, (దీర్ఘ గుర్తులేని పూర్ణాంకం)asid);
        } లేకపోతే {
            రాయి("%p_pd\n", కనుగొను_ret.pd);
        }
        ఆపు;
    }
    స్థితి cap_asid_control_cap: {
        /* only one in the system */
        రాయి("asid_control\n");
        ఆపు;
    }
    స్థితి cap_small_చట్రం_cap: {
        vptr_t vptr = cap_small_చట్రం_cap_get_cap_FMapped_చిరునామా(cap);
        కనుగొను_PDFor_ASID_ret_t కనుగొను_ret = కనుగొను_PDFor_ASID(cap_small_చట్రం_cap_get_cap_FMapped_ASID(cap));
        ప్రతిజ్ఞా(కనుగొను_ret.స్థితి_2 == EXCEPTION_NONE);
        cap_చట్రం_print_attrs_vptr(vptr, కనుగొను_ret.pd);
        ఆపు;
    }
    స్థితి cap_చట్రం_cap: {
        vptr_t vptr = cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap);
        కనుగొను_PDFor_ASID_ret_t కనుగొను_ret = కనుగొను_PDFor_ASID(cap_చట్రం_cap_get_cap_FMapped_ASID(cap));
        ప్రతిజ్ఞా(కనుగొను_ret.స్థితి_2 == EXCEPTION_NONE);
        cap_చట్రం_print_attrs_vptr(vptr, కనుగొను_ret.pd);
        ఆపు;
    }
    స్థితి cap_asid_pool_cap: {
        రాయి("%p_asid_pool\n", (శూన్య *)cap_asid_pool_cap_get_capASIDPool(cap));
        ఆపు;
    }
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి cap_vcpu_cap: {
        రాయి("%p_vcpu\n", (శూన్య *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ఆపు;
    }
#అయితే_ముగింపు

        /* ARM specific caps */
#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి cap_io_space_cap: {
        రాయి("%p_io_space\n", (శూన్య *)cap_io_space_cap_get_capModuleID(cap));
        ఆపు;
    }
#అయితే_ముగింపు
    అప్రమేయ: {
        రాయి("[unknown cap %u]\n", cap_get_capType(cap));
        ఆపు;
    }
    }
}

శూన్య print_object_arch(cap_t cap)
{

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_చట్రం_cap:
    స్థితి cap_small_చట్రం_cap:
    స్థితి cap_పుట_పట్టిక_cap:
    స్థితి cap_పుట_directory_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ఆపు;

    స్థితి cap_asid_pool_cap: {
        రాయి("%p_asid_pool = asid_pool ",
               (శూన్య *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ఆపు;
    }
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి cap_vcpu_cap: {
        రాయి("%p_vcpu = vcpu\n", (శూన్య *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ఆపు;
    }
#అయితే_ముగింపు
        /* ARM specific objects */
#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి cap_io_space_cap: {
        రాయి("%p_io_space = io_space ", (శూన్య *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        ఆపు;
    }
#అయితే_ముగింపు
    అప్రమేయ: {
        రాయి("[unknown object %u]\n", cap_get_capType(cap));
        ఆపు;
    }
    }
}

స్థిర శూన్య obj_చట్రం_print_attrs(resolve_ret_t ret)
{
    రాయి("(");

    /* VM size */
    ఎంపిక (ret.చట్రం_కొలత) {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి ARMSection:
        రాయి("2M");
        ఆపు;
    స్థితి ARMSuperSection:
        రాయి("32M");
        ఆపు;
#లేకపోతే
    స్థితి ARMSection:
        రాయి("1M");
        ఆపు;
    స్థితి ARMSuperSection:
        రాయి("16M");
        ఆపు;
#అయితే_ముగింపు
    స్థితి ARMLarge_పుట:
        రాయి("64k");
        ఆపు;
    స్థితి ARMSmall_పుట:
        రాయి("4k");
        ఆపు;
    }

    రాయి(", paddr: %p)\n", (శూన్య *)ret.చట్రం_పునాది);
}

స్థిర శూన్య arm32_obj_pt_print_slots(pte_t *pt)
{
    resolve_ret_t ret;
    మాట_t i = 0;
    అయితే (i < బిట్టు(PT_సూచీ_బిట్లు + పుట_బిట్లు)) {
        pte_t *pte = lookupPTSlot_nofail(pt, i);
        ఎంపిక (pte_ptr_get_pteType(pte)) {
        స్థితి pte_pte_small: {
            ret.చట్రం_పునాది = pte_pte_small_ptr_get_చిరునామా(pte);
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
            ఒకవేళ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.చట్రం_పునాది &= ~ముసుగు(పుట_బిట్లు_For_కొలత(ARMLarge_పుట));
                ret.చట్రం_కొలత = ARMLarge_పుట;
            } లేకపోతే {
                ret.చట్రం_కొలత = ARMSmall_పుట;
            }
#లేకపోతే
            ret.చట్రం_కొలత = ARMSmall_పుట;
#అయితే_ముగింపు
            రాయి("frame_%p_%04lu = frame ", pte, PT_సూచీ(i));
            obj_చట్రం_print_attrs(ret);
            ఆపు;
        }
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        స్థితి pte_pte_large: {
            ret.చట్రం_పునాది = pte_pte_large_ptr_get_చిరునామా(pte);
            ret.చట్రం_కొలత = ARMLarge_పుట;
            రాయి("frame_%p_%04lu = frame ", pte, PT_సూచీ(i));
            obj_చట్రం_print_attrs(ret);
            ఆపు;
        }
#అయితే_ముగింపు
        అప్రమేయ:
            ret.చట్రం_కొలత = ARMSmall_పుట;
        }
        i += (1 << పుట_బిట్లు_For_కొలత(ret.చట్రం_కొలత));
    }
}

శూన్య obj_tcb_print_vtable(tcb_t *tcb)
{
    ఒకవేళ (is_Valid_VTable_మూలం(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        కూడు_గమ్యం_చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pde_t *pd = (pde_t *)pptr_యొక్క_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        resolve_ret_t ret = {};
        రాయి("%p_pd = pd\n", pd);

        /* PD_INDEX_BITS + ARMSectionBits = 32, can't use left shift here */
        మాట_t i = 0;
        అయితే (i < MAX_UL) {
            pde_t *pde = lookupPDSlot(pd, i);
            ఎంపిక (pde_ptr_get_pdeType(pde)) {
            స్థితి pde_pde_section: {
                ret.చట్రం_పునాది = pde_pde_section_ptr_get_చిరునామా(pde);
#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
                ఒకవేళ (pde_pde_section_ptr_get_కొలత(pde)) {
                    ret.చట్రం_కొలత = ARMSuperSection;
                } లేకపోతే {
                    ret.చట్రం_కొలత = ARMSection;
                }
#లేకపోతే
                ఒకవేళ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
                    /* Entries are represented as 16 contiguous sections. We need to mask
                    to get the super section frame base */
                    ret.చట్రం_పునాది &= ~ముసుగు(పుట_బిట్లు_For_కొలత(ARMSuperSection));
                    ret.చట్రం_కొలత = ARMSuperSection;
                } లేకపోతే {
                    ret.చట్రం_కొలత = ARMSection;
                }
#అయితే_ముగింపు
                రాయి("frame_%p_%04lu = frame ", pde, PD_సూచీ(i));
                obj_చట్రం_print_attrs(ret);
                ఆపు;
            }

            స్థితి pde_pde_coarse: {
                pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_చిరునామా(pde));
                రాయి("pt_%p_%04lu = pt\n", pde, PD_సూచీ(i));
                arm32_obj_pt_print_slots(pt);
                ret.చట్రం_కొలత = ARMSection;
                ఆపు;
            }
            అప్రమేయ:
                ret.చట్రం_కొలత = ARMSection;
                ఆపు;
            }
            i += (1 << పుట_బిట్లు_For_కొలత(ret.చట్రం_కొలత));
            ఒకవేళ (i < (1 << పుట_బిట్లు_For_కొలత(ret.చట్రం_కొలత))) {
                ఆపు; /* overflowed */
            }
        }
    }
}

#అయితే_ముగింపు /* CONFIG_PRINTING */

శూన్య debug_capDL(శూన్య)
{
    రాయి("arch aarch32\n");
    రాయి("objects {\n");
#నిర్వచితమైతే CONFIG_PRINTING
    print_objects();
#అయితే_ముగింపు
    రాయి("}\n");

    రాయి("caps {\n");

    /* reset the seen list */
    reset_చూసిన_list();

#నిర్వచితమైతే CONFIG_PRINTING
    print_caps();
    రాయి("}\n");

    obj_irq_print_maps();
#అయితే_ముగింపు /* CONFIG_PRINTING */
}


#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
