/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_DEBUG_BUILD

#అన్తర్భావ <arch/machine/capdl.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <kernel/cspace.h>

మాట_t get_tcb_sp(tcb_t *tcb)
{
    తిరిగి tcb->tcbArch.tcbContext.registers[RSP];
}

#నిర్వచితమైతే CONFIG_PRINTING

స్థిర శూన్య obj_చట్రం_print_attrs(paddr_t paddr, మాట_t పుట_కొలత);
స్థిర శూన్య cap_చట్రం_print_attrs_vptr_2(మాట_t vptr, vspace_మూలం_t *vspace);
స్థిర శూన్య cap_చట్రం_print_attrs_vptr(మాట_t vptr, cap_t vspace);
స్థిర శూన్య obj_asidpool_print_attrs(cap_t asid_cap);
స్థిర శూన్య cap_చట్రం_print_attrs_pdpt(pdpte_t *pdpt_స్థానం);
స్థిర శూన్య cap_చట్రం_print_attrs_pd(pde_t *pd_స్థానం);
స్థిర శూన్య cap_చట్రం_print_attrs_pt(pte_t *pt_స్థానం);
స్థిర శూన్య cap_చట్రం_print_attrs_impl(మాట_t super_user, మాట_t read_write, మాట_t cache_disabled, మాట_t xd);

/* use when only have access to pte of frames */
స్థిర శూన్య cap_చట్రం_print_attrs_pdpt(pdpte_t *pdpt_స్థానం)
{
    cap_చట్రం_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdpt_స్థానం),
                               pdpte_pdpte_1g_ptr_get_read_write(pdpt_స్థానం),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdpt_స్థానం),
                               pdpte_pdpte_1g_ptr_get_xd(pdpt_స్థానం));
}
స్థిర శూన్య cap_చట్రం_print_attrs_pd(pde_t *pd_స్థానం)
{
    cap_చట్రం_print_attrs_impl(pde_pde_large_ptr_get_super_user(pd_స్థానం),
                               pde_pde_large_ptr_get_read_write(pd_స్థానం),
                               pde_pde_large_ptr_get_cache_disabled(pd_స్థానం),
                               pde_pde_large_ptr_get_xd(pd_స్థానం));
}

స్థిర శూన్య cap_చట్రం_print_attrs_pt(pte_t *pt_స్థానం)
{
    cap_చట్రం_print_attrs_impl(pte_ptr_get_super_user(pt_స్థానం),
                               pte_ptr_get_read_write(pt_స్థానం),
                               pte_ptr_get_cache_disabled(pt_స్థానం),
                               pte_ptr_get_xd(pt_స్థానం));
}

స్థిర శూన్య cap_చట్రం_print_attrs_impl(మాట_t super_user, మాట_t read_write, మాట_t cache_disabled, మాట_t xd)
{
    రాయి("(");

    /* rights */
    ఒకవేళ (read_write) {
        రాయి("RW");
    } లేకపోతే ఒకవేళ (super_user) {
        రాయి("R");
    }

    ఒకవేళ (!xd) {
        రాయి("X");
    }

    /* asid, mapping */

    ఒకవేళ (cache_disabled) {
        రాయి(", uncached");
    }

    రాయి(")\n");
}

స్థిర శూన్య obj_చట్రం_print_attrs(paddr_t paddr, మాట_t పుట_కొలత)
{
    రాయి("(");

    /* VM size */
    ఎంపిక (పుట_కొలత) {
    స్థితి se_L4_Huge_పుట_బిట్లు:
        రాయి("1G");
        ఆపు;
    స్థితి se_L4_Large_పుట_బిట్లు:
        రాయి("2M");
        ఆపు;
    స్థితి se_L4_పుట_బిట్లు:
        రాయి("4k");
        ఆపు;
    }

    రాయి(", paddr: 0x%p)\n", (శూన్య *)paddr);
}

స్థిర శూన్య x86_64_obj_pt_print_slots(pde_t *pd_స్థానం)
{
    paddr_t paddr;
    మాట_t పుట_కొలత;
    pte_t *pt = paddr_గమ్యం_pptr(pde_pde_pt_ptr_get_pt_పునాది_చిరునామా(pd_స్థానం));

    కొరకు (మాట_t i = 0; i < బిట్టు(PT_సూచీ_స్థానభ్రంశం + PT_సూచీ_బిట్లు); i += (1UL << PT_సూచీ_స్థానభ్రంశం)) {
        pte_t *pt_స్థానం = pt + GET_PT_సూచీ(i);

        ఒకవేళ (pte_ptr_get_present(pt_స్థానం)) {
            paddr = pte_ptr_get_పుట_పునాది_చిరునామా(pt_స్థానం);
            పుట_కొలత = se_L4_పుట_బిట్లు;
            రాయి("frame_%p_%04lu = frame ", pt_స్థానం, GET_PT_సూచీ(i));
            obj_చట్రం_print_attrs(paddr, పుట_కొలత);
        }
    }
}

స్థిర శూన్య x86_64_obj_pd_print_slots(pdpte_t *pdpt_స్థానం)
{
    paddr_t paddr;
    మాట_t పుట_కొలత;
    pde_t *pd = paddr_గమ్యం_pptr(pdpte_pdpte_pd_ptr_get_pd_పునాది_చిరునామా(pdpt_స్థానం));

    కొరకు (మాట_t i = 0; i < బిట్టు(PD_సూచీ_స్థానభ్రంశం + PD_సూచీ_బిట్లు); i += (1UL << PD_సూచీ_స్థానభ్రంశం)) {
        pde_t *pd_స్థానం = pd + GET_PD_సూచీ(i);

        ఒకవేళ ((pde_ptr_get_పుట_కొలత(pd_స్థానం) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_స్థానం)) {
            paddr = pde_pde_large_ptr_get_పుట_పునాది_చిరునామా(pd_స్థానం);
            పుట_కొలత = se_L4_Large_పుట_బిట్లు;

            రాయి("frame_%p_%04lu = frame ", pd_స్థానం, GET_PD_సూచీ(i));
            obj_చట్రం_print_attrs(paddr, పుట_కొలత);

        } లేకపోతే ఒకవేళ (pde_pde_pt_ptr_get_present(pd_స్థానం)) {
            రాయి("pt_%p_%04lu = pt\n", pd_స్థానం, GET_PD_సూచీ(i));
            x86_64_obj_pt_print_slots(pd_స్థానం);
        }
    }
}

స్థిర శూన్య x86_64_obj_pdpt_print_slots(pml4e_t *pml4_స్థానం)
{
    paddr_t paddr;
    మాట_t పుట_కొలత;
    pdpte_t *pdpt = paddr_గమ్యం_pptr(pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4_స్థానం));

    కొరకు (మాట_t i = 0; i < బిట్టు(PDPT_సూచీ_స్థానభ్రంశం + PDPT_సూచీ_బిట్లు); i += (1UL << PDPT_సూచీ_స్థానభ్రంశం)) {
        pdpte_t *pdpt_స్థానం = pdpt + GET_PDPT_సూచీ(i);

        ఒకవేళ (pdpte_ptr_get_పుట_కొలత(pdpt_స్థానం) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_స్థానం)) {
            paddr = pdpte_pdpte_1g_ptr_get_పుట_పునాది_చిరునామా(pdpt_స్థానం);
            పుట_కొలత = se_L4_Huge_పుట_బిట్లు;

            రాయి("frame_%p_%04lu = frame ", pdpt_స్థానం, GET_PDPT_సూచీ(i));
            obj_చట్రం_print_attrs(paddr, పుట_కొలత);

        } లేకపోతే ఒకవేళ (pdpte_pdpte_pd_ptr_get_present(pdpt_స్థానం)) {
            రాయి("pd_%p_%04lu = pd\n", pdpt_స్థానం, GET_PDPT_సూచీ(i));
            x86_64_obj_pd_print_slots(pdpt_స్థానం);
        }
    }
}

స్థిర శూన్య x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    కొరకు (మాట_t i = 0; i < బిట్టు(PML4_సూచీ_స్థానభ్రంశం + PML4_సూచీ_బిట్లు); i += (1UL << PML4_సూచీ_స్థానభ్రంశం)) {
        pml4e_t *pml4_స్థానం = lookup_PML4_స్థానం(pml4, i);
        ఒకవేళ (pml4e_ptr_get_present(pml4_స్థానం)) {
            రాయి("pdpt_%p_%04lu = pdpt\n", pml4_స్థానం, GET_PML4_సూచీ(i));
            x86_64_obj_pdpt_print_slots(pml4_స్థానం);
        }
    }
}

శూన్య obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    ఒకవేళ (is_Valid_VTable_మూలం(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_పునాది_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        కూడు_గమ్యం_చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        రాయి("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
స్థిర శూన్య cap_చట్రం_print_attrs_vptr_2(మాట_t vptr, vspace_మూలం_t *vspace)
{
    lookupPDPTSlot_ret_t pdpt_స్థానం = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t pt_స్థానం;
    lookupPDSlot_ret_t pd_స్థానం;

    ఒకవేళ (pdpt_స్థానం.స్థితి_2 == EXCEPTION_NONE &&
        pdpte_ptr_get_పుట_కొలత(pdpt_స్థానం.pdpt_స్థానం) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_స్థానం.pdpt_స్థానం)) {
        రాయి("frame_%p_%04lu ", pdpt_స్థానం.pdpt_స్థానం, GET_PDPT_సూచీ(vptr));
        cap_చట్రం_print_attrs_pdpt(pdpt_స్థానం.pdpt_స్థానం);
    } లేకపోతే {
        pd_స్థానం = lookupPDSlot(vspace, vptr);
        ఒకవేళ (pd_స్థానం.స్థితి_2 == EXCEPTION_NONE &&
            ((pde_ptr_get_పుట_కొలత(pd_స్థానం.pd_స్థానం) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_స్థానం.pd_స్థానం))) {
            రాయి("frame_%p_%04lu ", pd_స్థానం.pd_స్థానం, GET_PD_సూచీ(vptr));
            cap_చట్రం_print_attrs_pd(pd_స్థానం.pd_స్థానం);
        } లేకపోతే {
            pt_స్థానం = lookupPTSlot(vspace, vptr);
            ప్రతిజ్ఞా(pt_స్థానం.స్థితి_2 == EXCEPTION_NONE && pte_ptr_get_present(pt_స్థానం.pt_స్థానం));
            రాయి("frame_%p_%04lu ", pt_స్థానం.pt_స్థానం, GET_PT_సూచీ(vptr));
            cap_చట్రం_print_attrs_pt(pt_స్థానం.pt_స్థానం);
        }
    }
}

స్థిర శూన్య cap_చట్రం_print_attrs_vptr(మాట_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    cap_చట్రం_print_attrs_vptr_2(vptr, కనుగొను_ret.vspace_మూలం);
}

శూన్య print_cap_arch(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    /* arch specific caps */
    స్థితి cap_పుట_పట్టిక_cap: {
        asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap);
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap);
        ఒకవేళ (asid) {
            రాయి("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(కనుగొను_ret.vspace_మూలం, vptr).pd_స్థానం, GET_PD_సూచీ(vptr), (దీర్ఘ గుర్తులేని పూర్ణాంకం)asid);
        } లేకపోతే {
            రాయి("pt_%p_%04lu\n", lookupPDSlot(కనుగొను_ret.vspace_మూలం, vptr).pd_స్థానం, GET_PD_సూచీ(vptr));
        }
        ఆపు;
    }
    స్థితి cap_పుట_directory_cap: {
        asid_t asid = cap_పుట_directory_cap_get_cap_PDMapped_ASID(cap);
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        vptr_t vptr = cap_పుట_directory_cap_get_cap_PDMapped_చిరునామా(cap);
        ఒకవేళ (asid) {
            రాయి("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(కనుగొను_ret.vspace_మూలం, vptr).pdpt_స్థానం, GET_PDPT_సూచీ(vptr), (దీర్ఘ గుర్తులేని పూర్ణాంకం)asid);
        } లేకపోతే {
            రాయి("pd_%p_%04lu\n", lookupPDPTSlot(కనుగొను_ret.vspace_మూలం, vptr).pdpt_స్థానం, GET_PDPT_సూచీ(vptr));
        }
        ఆపు;
    }
    స్థితి cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_చిరునామా(cap);
        ఒకవేళ (asid) {
            రాయి("pdpt_%p_%04lu (asid: %lu)\n",
                   lookup_PML4_స్థానం(కనుగొను_ret.vspace_మూలం, vptr), GET_PML4_సూచీ(vptr), (దీర్ఘ గుర్తులేని పూర్ణాంకం)asid);
        } లేకపోతే {
            రాయి("pdpt_%p_%04lu\n", lookup_PML4_స్థానం(కనుగొను_ret.vspace_మూలం, vptr), GET_PML4_సూచీ(vptr));
        }
        ఆపు;
    }
    స్థితి cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        ఒకవేళ (asid) {
            రాయి("%p_pd (asid: %lu)\n",
                   కనుగొను_ret.vspace_మూలం, (దీర్ఘ గుర్తులేని పూర్ణాంకం)asid);
        } లేకపోతే {
            రాయి("%p_pd\n", కనుగొను_ret.vspace_మూలం);
        }
        ఆపు;
    }
    స్థితి cap_asid_control_cap: {
        /* only one in the system */
        రాయి("asid_control\n");
        ఆపు;
    }
    స్థితి cap_చట్రం_cap: {
        vptr_t vptr = cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap);
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(cap_చట్రం_cap_get_cap_FMapped_ASID(cap));
        ప్రతిజ్ఞా(కనుగొను_ret.స్థితి_2 == EXCEPTION_NONE);
        cap_చట్రం_print_attrs_vptr_2(vptr, కనుగొను_ret.vspace_మూలం);
        ఆపు;
    }
    స్థితి cap_asid_pool_cap: {
        రాయి("%p_asid_pool\n", (శూన్య *)cap_asid_pool_cap_get_capASIDPool(cap));
        ఆపు;
    }
#నిర్వచితమైతే CONFIG_VTX
    స్థితి cap_vcpu_cap: {
        రాయి("%p_vcpu\n", (శూన్య *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ఆపు;
    }
#అయితే_ముగింపు
    /* X86 specific caps */
    స్థితి cap_io_పోర్టు_cap: {
        రాయి("%p%p_io_port\n", (శూన్య *)cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(cap),
               (శూన్య *)cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(cap));
        ఆపు;
    }
#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి cap_io_space_cap: {
        రాయి("%p_io_space\n", (శూన్య *)cap_io_space_cap_get_capPCIDevice(cap));
        ఆపు;
    }
    స్థితి cap_io_పుట_పట్టిక_cap: {
        రాయి("%p_iopt\n", (శూన్య *)cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(cap));
        ఆపు;
    }
#అయితే_ముగింపు
    అప్రమేయ: {
        రాయి("[unknown cap %lu]\n", (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_get_capType(cap));
        ఆపు;
    }
    }
}

స్థిర శూన్య obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    రాయి("(asid_high: 0x%lx)\n", ASID_ఎగువ(asid));
}

శూన్య print_object_arch(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    /* arch specific objects */
    స్థితి cap_చట్రం_cap:
    స్థితి cap_పుట_పట్టిక_cap:
    స్థితి cap_పుట_directory_cap:
    స్థితి cap_pdpt_cap:
    స్థితి cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ఆపు;

    స్థితి cap_asid_pool_cap: {
        రాయి("%p_asid_pool = asid_pool ",
               (శూన్య *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ఆపు;
    }
#నిర్వచితమైతే CONFIG_VTX
    స్థితి cap_vcpu_cap: {
        రాయి("%p_vcpu = vcpu\n", (శూన్య *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ఆపు;
    }
#అయితే_ముగింపు
    /* X86 specific caps */
    స్థితి cap_io_పోర్టు_cap: {
        రాయి("%p%p_io_port = io_port ",
               (శూన్య *)cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(cap),
               (శూన్య *)cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(cap));
        x86_obj_ioports_print_attrs(cap);
        ఆపు;
    }
#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి cap_io_space_cap: {
        రాయి("%p_io_space = io_space ", (శూన్య *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        ఆపు;
    }
    స్థితి cap_io_పుట_పట్టిక_cap: {
        రాయి("%p_iopt = iopt ", (శూన్య *)cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        ఆపు;
    }
#అయితే_ముగింపు
    అప్రమేయ: {
        రాయి("[unknown object %lu]\n", (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_get_capType(cap));
        ఆపు;
    }
    }
}

శూన్య print_ipc_బఫరు_స్థానం(tcb_t *tcb)
{
    మాట_t vptr = tcb->tcbIPCBuffer;
    రాయి("ipc_buffer_slot: ");
    cap_చట్రం_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

స్థిర శూన్య x86_64_cap_pt_print_slots(pde_t *pd_స్థానం, vptr_t vptr)
{
    pte_t *pt = paddr_గమ్యం_pptr(pde_pde_pt_ptr_get_pt_పునాది_చిరునామా(pd_స్థానం));
    రాయి("pt_%p_%04lu {\n", pd_స్థానం, GET_PD_సూచీ(vptr));

    కొరకు (మాట_t i = 0; i < బిట్టు(PT_సూచీ_స్థానభ్రంశం + PT_సూచీ_బిట్లు); i += (1UL << PT_సూచీ_స్థానభ్రంశం)) {
        pte_t *pt_స్థానం = pt + GET_PT_సూచీ(i);

        ఒకవేళ (pte_ptr_get_present(pt_స్థానం)) {
            రాయి("0x%lx: frame_%p_%04lu ", GET_PT_సూచీ(i), pt_స్థానం, GET_PT_సూచీ(i));
            cap_చట్రం_print_attrs_pt(pt_స్థానం);
        }
    }
    రాయి("}\n"); /* pt */
}

స్థిర శూన్య x86_64_cap_pd_print_slots(pdpte_t *pdpt_స్థానం, vptr_t vptr)
{
    pde_t *pd = paddr_గమ్యం_pptr(pdpte_pdpte_pd_ptr_get_pd_పునాది_చిరునామా(pdpt_స్థానం));
    రాయి("pd_%p_%04lu {\n", pdpt_స్థానం, GET_PDPT_సూచీ(vptr));

    కొరకు (మాట_t i = 0; i < బిట్టు(PD_సూచీ_స్థానభ్రంశం + PD_సూచీ_బిట్లు); i += (1UL << PD_సూచీ_స్థానభ్రంశం)) {
        pde_t *pd_స్థానం = pd + GET_PD_సూచీ(i);

        ఒకవేళ ((pde_ptr_get_పుట_కొలత(pd_స్థానం) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_స్థానం)) {
            రాయి("0x%lx: frame_%p_%04lu ", GET_PD_సూచీ(i), pd_స్థానం, GET_PD_సూచీ(i));
            cap_చట్రం_print_attrs_pd(pd_స్థానం);

        } లేకపోతే ఒకవేళ (pde_pde_pt_ptr_get_present(pd_స్థానం)) {
            రాయి("0x%lx: pt_%p_%04lu\n", GET_PD_సూచీ(i), pd_స్థానం, GET_PD_సూచీ(i));
        }
    }
    రాయి("}\n"); /* pd */

    కొరకు (మాట_t i = 0; i < బిట్టు(PD_సూచీ_స్థానభ్రంశం + PD_సూచీ_బిట్లు); i += (1UL << PD_సూచీ_స్థానభ్రంశం)) {
        pde_t *pd_స్థానం = pd + GET_PD_సూచీ(i);
        ఒకవేళ ((pde_ptr_get_పుట_కొలత(pd_స్థానం) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_స్థానం)) {
            x86_64_cap_pt_print_slots(pd_స్థానం, i);
        }
    }
}

స్థిర శూన్య x86_64_cap_pdpt_print_slots(pml4e_t *pml4_స్థానం, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_గమ్యం_pptr(pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4_స్థానం));

    రాయి("pdpt_%p_%04lu {\n", pml4_స్థానం, GET_PML4_సూచీ(vptr));
    కొరకు (మాట_t i = 0; i < బిట్టు(PDPT_సూచీ_స్థానభ్రంశం + PDPT_సూచీ_బిట్లు); i += (1UL << PDPT_సూచీ_స్థానభ్రంశం)) {
        pdpte_t *pdpt_స్థానం = pdpt + GET_PDPT_సూచీ(i);

        ఒకవేళ (pdpte_ptr_get_పుట_కొలత(pdpt_స్థానం) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_స్థానం)) {
            రాయి("0x%lx: frame_%p_%04lu ", GET_PDPT_సూచీ(i), pdpt_స్థానం, GET_PDPT_సూచీ(i));
            cap_చట్రం_print_attrs_pdpt(pdpt_స్థానం);

        } లేకపోతే ఒకవేళ (pdpte_pdpte_pd_ptr_get_present(pdpt_స్థానం)) {
            రాయి("0x%lx: pd_%p_%04lu\n", GET_PDPT_సూచీ(i), pdpt_స్థానం, GET_PDPT_సూచీ(i));
        }
    }
    రాయి("}\n"); /* pdpt */

    కొరకు (మాట_t i = 0; i < బిట్టు(PDPT_సూచీ_స్థానభ్రంశం + PDPT_సూచీ_బిట్లు); i += (1UL << PDPT_సూచీ_స్థానభ్రంశం)) {
        pdpte_t *pdpt_స్థానం = pdpt + GET_PDPT_సూచీ(i);

        ఒకవేళ (pdpte_ptr_get_పుట_కొలత(pdpt_స్థానం) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdpt_స్థానం)) {
            x86_64_cap_pd_print_slots(pdpt_స్థానం, i);
        }
    }
}

స్థిర శూన్య x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    రాయి("%p_pd {\n", pml4);
    కొరకు (మాట_t i = 0; i < బిట్టు(PML4_సూచీ_స్థానభ్రంశం + PML4_సూచీ_బిట్లు); i += (1UL << PML4_సూచీ_స్థానభ్రంశం)) {
        pml4e_t *pml4_స్థానం = lookup_PML4_స్థానం(pml4, i);
        ఒకవేళ (pml4e_ptr_get_present(pml4_స్థానం)) {
            రాయి("0x%lx: pdpt_%p_%04lu\n", GET_PML4_సూచీ(i), pml4_స్థానం, GET_PML4_సూచీ(i));
        }
    }
    రాయి("}\n"); /* pml4 */

    కొరకు (మాట_t i = 0; i < బిట్టు(PML4_సూచీ_స్థానభ్రంశం + PML4_సూచీ_బిట్లు); i += (1UL << PML4_సూచీ_స్థానభ్రంశం)) {
        pml4e_t *pml4_స్థానం = lookup_PML4_స్థానం(pml4, i);
        ఒకవేళ (pml4e_ptr_get_present(pml4_స్థానం)) {
            x86_64_cap_pdpt_print_slots(pml4_స్థానం, i);
        }
    }
}

శూన్య obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    ఒకవేళ (is_Valid_VTable_మూలం(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_పునాది_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        కూడు_గమ్యం_చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#అయితే_ముగింపు /* CONFIG_PRINTING */

శూన్య debug_capDL(శూన్య)
{
    రాయి("arch x86_64\n");
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
#అయితే_ముగింపు
}

#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
