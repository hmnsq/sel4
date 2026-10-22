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
    తిరిగి tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#నిర్వచితమైతే CONFIG_PRINTING

స్థిర శూన్య obj_చట్రం_print_attrs(vm_పుట_కొలత_t చట్రం_కొలత, paddr_t చట్రం_పునాది);
స్థిర శూన్య cap_చట్రం_print_attrs_pt(pte_t *pt_స్థానం);
స్థిర శూన్య cap_చట్రం_print_attrs_impl(మాట_t SH, మాట_t AP, మాట_t NXN);
స్థిర శూన్య cap_చట్రం_print_attrs_vptr(మాట_t vptr, cap_t vspace);

స్థిర శూన్య cap_చట్రం_print_attrs_vptr_2(మాట_t vptr, vspace_మూలం_t *vspace_మూలం_2);

స్థిర శూన్య arm64_obj_pt_print_slots(pte_t *pd_స్థానం);
స్థిర శూన్య arm64_obj_pd_print_slots(pte_t *pud_స్థానం);
స్థిర శూన్య arm64_obj_pud_print_slots(శూన్య *pgd_స్థానం_or_vspace);

స్థిర శూన్య arm64_cap_pt_print_slots(pte_t *pd_స్థానం, vptr_t vptr);
స్థిర శూన్య arm64_cap_pd_print_slots(pte_t *pud_స్థానం, vptr_t vptr);
స్థిర శూన్య arm64_cap_pud_print_slots(శూన్య *pgd_స్థానం_or_vspace, vptr_t vptr);

/* Stage-1 access permissions:
 * AP[2:1]  higher EL        EL0
 *   00       rw            None
 *   01       rw            rw
 *   10       r             None
 *   11       r             r
 *
 * Stage-2 access permissions:
 * S2AP    Access from Nonsecure EL1 or Non-secure EL0
 *  00                      None
 *  01                      r
 *  10                      w
 *  11                      rw
 *
 *  For VMs or native seL4 applications, if hypervisor support
 *  is enabled, we use the S2AP. The kernel itself running in
 *  EL2 still uses the Stage-1 AP format.
 */
/* use when only have access to pte of frames */
స్థిర శూన్య cap_చట్రం_print_attrs_pt(pte_t *pt_స్థానం)
{
    cap_చట్రం_print_attrs_impl(pte_pte_పుట_ptr_get_SH(pt_స్థానం),
                               pte_pte_పుట_ptr_get_AP(pt_స్థానం),
                               pte_pte_పుట_ptr_get_UXN(pt_స్థానం));
}

స్థిర శూన్య cap_చట్రం_print_attrs_impl(మాట_t SH, మాట_t AP, మాట_t NXN)
{
    రాయి("(");

    /* rights */
    ఎంపిక (AP) {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి 0b00:
        ఆపు;
    స్థితి 0b01:
        రాయి("R");
        ఆపు;
    స్థితి 0b10:
        రాయి("W");
        ఆపు;
    స్థితి 0b11:
        రాయి("RW");
        ఆపు;
#లేకపోతే
    స్థితి 0b00:
        ఆపు;
    స్థితి 0b01:
        రాయి("RW");
    స్థితి 0b10:
        ఆపు;
    స్థితి 0b11:
        రాయి("R");
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    అప్రమేయ:
        ఆపు;
    }

    ఒకవేళ (!NXN) {
        రాయి("X");
    }

    /* Only has effect if SMP enabled */
    ఒకవేళ (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        రాయి(", uncached");
    }

    రాయి(")\n");
}

/* use when only have access to vptr of frames */
స్థిర శూన్య cap_చట్రం_print_attrs_vptr_2(మాట_t vptr, vspace_మూలం_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    ఒకవేళ (!pte_ptr_get_valid(ret.pt_స్థానం) ||
        (pte_pte_పట్టిక_ptr_get_present(ret.pt_స్థానం) && ret.pt_బిట్లు_ఎడమ > పుట_బిట్లు)) {
        ప్రతిజ్ఞా(0);
    }

    మాట_t పట్టిక_సూచీ;
    ఎంపిక (ret.pt_బిట్లు_ఎడమ) {

    స్థితి ARMHuge_పుట:
        పట్టిక_సూచీ = GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(1));
        ఆపు;
    స్థితి ARMLarge_పుట:
        పట్టిక_సూచీ = GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(2));
        ఆపు;
    స్థితి ARMSmall_పుట:
        పట్టిక_సూచీ = GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(3));
        ఆపు;
    అప్రమేయ:
        ప్రతిజ్ఞా(0);

    }
    రాయి("frame_%p_%04lu ", ret.pt_స్థానం, పట్టిక_సూచీ);
    cap_చట్రం_print_attrs_pt(ret.pt_స్థానం);
}

శూన్య cap_చట్రం_print_attrs_vptr(మాట_t vptr, cap_t vspace)
{
    cap_చట్రం_print_attrs_vptr_2(vptr, VSPACE_PTR(pptr_యొక్క_cap(vspace)));
}

/*
 * print object slots
 */
స్థిర శూన్య arm64_cap_pt_print_slots(pte_t *pd_స్థానం, vptr_t vptr)
{
    pte_t *pt = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pd_స్థానం));
    రాయి("pt_%p_%04lu {\n", pd_స్థానం, GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    కొరకు (మాట_t i = 0; i < బిట్టు(PT_సూచీ_బిట్లు); i ++) {
        pte_t *pt_స్థానం = pt + i;

        ఒకవేళ (pte_4k_పుట_ptr_get_present(pt_స్థానం)) {
            // print pte entries
            రాయి("0x%lx: frame_%p_%04lu", i, pt_స్థానం, i);
            cap_చట్రం_print_attrs_pt(pt_స్థానం);
        }
    }
    రాయి("}\n"); /* pt */
}

స్థిర శూన్య arm64_cap_pd_print_slots(pte_t *pud_స్థానం, vptr_t vptr)
{
    రాయి("pd_%p_%04lu {\n", pud_స్థానం, GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pud_స్థానం));

    కొరకు (మాట_t i = 0; i < బిట్టు(PT_సూచీ_బిట్లు); i++) {
        pte_t *pd_స్థానం = pd + i;

        ఎంపిక (pte_ptr_get_pte_type(pd_స్థానం)) {

        స్థితి pte_pte_పుట:
            రాయి("0x%lx: frame_%p_%04lu", i, pd_స్థానం, i);
            cap_చట్రం_print_attrs_pt(pd_స్థానం);
            ఆపు;

        స్థితి pte_pte_పట్టిక:
            రాయి("0x%lx: pt_%p_%04lu\n", i, pd_స్థానం, i);
            ఆపు;
        }
    }

    రాయి("}\n"); /* pd */

    కొరకు (మాట_t i = 0; i < బిట్టు(PT_సూచీ_బిట్లు); i++) {
        pte_t *pd_స్థానం = pd + i;
        ఒకవేళ (pte_ptr_get_pte_type(pd_స్థానం) == pte_pte_పట్టిక) {
            arm64_cap_pt_print_slots(pd_స్థానం, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

స్థిర శూన్య arm64_cap_pud_print_slots(శూన్య *pgd_స్థానం_or_vspace, vptr_t vptr)
{
#నిర్వచితమైతే AARCH64_VSPACE_S2_ప్రారంభం_L1
    pte_t *pud = pgd_స్థానం_or_vspace;
    మాట_t సూచీ_బిట్లు = se_L4_VSpace_సూచీ_బిట్లు;
    రాయి("%p_pd {\n", pgd_స్థానం_or_vspace);
#లేకపోతే
    pte_t *pud = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pgd_స్థానం_or_vspace));
    మాట_t సూచీ_బిట్లు = se_L4_పుట_పట్టిక_సూచీ_బిట్లు;
    రాయి("pud_%p_%04lu {\n", pgd_స్థానం_or_vspace, GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#అయితే_ముగింపు

    కొరకు (మాట_t i = 0; i < బిట్టు(సూచీ_బిట్లు); i++) {
        pte_t *pud_స్థానం = pud + i;
        ఒకవేళ (pte_ptr_get_pte_type(pud_స్థానం) == pte_pte_పట్టిక) {
            రాయి("0x%lx: pd_%p_%04lu\n", i, pud_స్థానం, i);
        }
    }

    రాయి("}\n"); /* pgd/pud */

    కొరకు (మాట_t i = 0; i < బిట్టు(సూచీ_బిట్లు); i++) {
        pte_t *pud_స్థానం = pud + GET_UPT_సూచీ(i, ULVL_FRM_ARM_PT_LVL(1));
        ఒకవేళ (pte_ptr_get_pte_type(pud_స్థానం) == pte_pte_పట్టిక) {
            arm64_cap_pd_print_slots(pud_స్థానం, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

శూన్య obj_vtable_print_slots(tcb_t *tcb)
{
    ఒకవేళ (is_VTable_మూలం(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        కూడు_గమ్యం_చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_మూలం_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#నిర్వచితమైతే AARCH64_VSPACE_S2_ప్రారంభం_L1
        arm64_cap_pud_print_slots(vspace, 0);
#లేకపోతే
        రాయి("%p_pd {\n", vspace);
        కొరకు (మాట_t i = 0; i < PT_సూచీ_బిట్లు; i++) {
            pte_t *pt_స్థానం = vspace + i;
            ఒకవేళ (pte_pte_పట్టిక_ptr_get_present(pt_స్థానం)) {
                రాయి("0x%lx: pud_%p_%04lu\n", i, pt_స్థానం, i);
            }
        }
        రాయి("}\n"); /* pd */

        కొరకు (మాట_t i = 0; i < PT_సూచీ_బిట్లు; i++) {
            pte_t *pt_స్థానం = vspace + i;
            ఒకవేళ (pte_pte_పట్టిక_ptr_get_present(pt_స్థానం)) {
                arm64_cap_pud_print_slots(pt_స్థానం, i * GET_ULVL_PGSIZE(0));
            }
        }
#అయితే_ముగింపు
    }
}

శూన్య print_ipc_బఫరు_స్థానం(tcb_t *tcb)
{
    మాట_t vptr = tcb->tcbIPCBuffer;
    రాయి("ipc_buffer_slot: ");
    cap_చట్రం_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

శూన్య print_cap_arch(cap_t cap)
{

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_పట్టిక_cap: {
        asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap);
        pte_t *లక్ష్యం_pt = PT_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap));

        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        pte_t *pt_స్థానం = ఖాళీ;
        pte_t *pt = (pte_t *)కనుగొను_ret.vspace_మూలం;
        మాట_t level;
        కొరకు (level = 0; level < UPT_స్థాయిలు - 1 && pt != లక్ష్యం_pt; level++) {
            pt_స్థానం = pt + GET_UPT_సూచీ(vptr, level);
            ఒకవేళ (unlikely(!pte_pte_పట్టిక_ptr_get_present(pt_స్థానం))) {
                /* couldn't find it */
                ఆపు;
            }
            pt = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pt_స్థానం));
        }
        ఒకవేళ (pt != లక్ష్యం_pt) {
            /* didn't find it */
            ఆపు;
        }


        ఒకవేళ (asid) {
            రాయి("pt_%p_%04lu (asid: %lu)\n",
                   లక్ష్యం_pt, GET_UPT_సూచీ(vptr, level), (దీర్ఘ గుర్తులేని పూర్ణాంకం)asid);
        } లేకపోతే {
            రాయి("pt_%p_%04lu\n", లక్ష్యం_pt, GET_UPT_సూచీ(vptr, level));
        }
        ఆపు;
    }
    స్థితి cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
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
        రాయి("[unknown cap %lu]\n", (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_get_capType(cap));
        ఆపు;
    }
    }
}

శూన్య print_object_arch(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_చట్రం_cap:
    స్థితి cap_పుట_పట్టిక_cap:
    స్థితి cap_vspace_cap:
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
        రాయి("[unknown object %lu]\n", (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_get_capType(cap));
        ఆపు;
    }
    }
}

శూన్య obj_చట్రం_print_attrs(vm_పుట_కొలత_t చట్రం_కొలత, paddr_t చట్రం_పునాది)
{
    రాయి("(");

    /* VM size */
    ఎంపిక (చట్రం_కొలత) {
    స్థితి ARMHuge_పుట:
        రాయి("1G");
        ఆపు;
    స్థితి ARMLarge_పుట:
        రాయి("2M");
        ఆపు;
    స్థితి ARMSmall_పుట:
        రాయి("4k");
        ఆపు;
    }

    రాయి(", paddr: 0x%p)\n", (శూన్య *)చట్రం_పునాది);
}

శూన్య arm64_obj_pt_print_slots(pte_t *pd_స్థానం)
{
    pte_t *pt = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pd_స్థానం));

    కొరకు (మాట_t i = 0; i < బిట్టు(PT_సూచీ_బిట్లు); i++) {
        pte_t *pt_స్థానం = pt + i;

        ఒకవేళ (pte_4k_పుట_ptr_get_present(pt_స్థానం)) {
            రాయి("frame_%p_%04lu = frame ", pt_స్థానం, i);
            obj_చట్రం_print_attrs(ARMSmall_పుట, pte_పుట_ptr_get_పుట_పునాది_చిరునామా(pt_స్థానం));
        }
    }
}

శూన్య arm64_obj_pd_print_slots(pte_t *pud_స్థానం)
{
    pte_t *pd = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pud_స్థానం));

    కొరకు (మాట_t i = 0; i < బిట్టు(PT_సూచీ_బిట్లు); i++) {
        pte_t *pd_స్థానం = pd + i;

        ఒకవేళ (pte_ptr_get_pte_type(pd_స్థానం) == pte_pte_పుట) {
            రాయి("frame_%p_%04lu = frame ", pd_స్థానం, i);
            obj_చట్రం_print_attrs(ARMLarge_పుట, pte_పుట_ptr_get_పుట_పునాది_చిరునామా(pd_స్థానం));
        }

        ఒకవేళ (pte_ptr_get_pte_type(pd_స్థానం) == pte_pte_పట్టిక) {
            రాయి("pt_%p_%04lu = pt\n", pd_స్థానం, i);
            arm64_obj_pt_print_slots(pd_స్థానం);
        }
    }
}

శూన్య arm64_obj_pud_print_slots(శూన్య *pgd_స్థానం_or_vspace)
{
    pte_t *pud = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pgd_స్థానం_or_vspace));
#నిర్వచితమైతే AARCH64_VSPACE_S2_ప్రారంభం_L1
    మాట_t సూచీ_బిట్లు = se_L4_VSpace_సూచీ_బిట్లు;
#లేకపోతే
    మాట_t సూచీ_బిట్లు = se_L4_పుట_పట్టిక_సూచీ_బిట్లు;
#అయితే_ముగింపు
    కొరకు (మాట_t i = 0; i < బిట్టు(సూచీ_బిట్లు); i++) {
        pte_t *pud_స్థానం = pud + i;

        ఎంపిక (pte_ptr_get_pte_type(pud_స్థానం)) {
        స్థితి pte_pte_పుట:
            రాయి("frame_%p_%04lu = frame ", pud_స్థానం, i);
            obj_చట్రం_print_attrs(ARMHuge_పుట, pte_పుట_ptr_get_పుట_పునాది_చిరునామా(pud_స్థానం));
            ఆపు;

        స్థితి pte_pte_పట్టిక: {
            రాయి("pd_%p_%04lu = pd\n", pud_స్థానం, i);
            arm64_obj_pd_print_slots(pud_స్థానం);

        }
        }
    }
}

శూన్య obj_tcb_print_vtable(tcb_t *tcb)
{
    ఒకవేళ (is_VTable_మూలం(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        కూడు_గమ్యం_చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_మూలం_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#నిర్వచితమైతే AARCH64_VSPACE_S2_ప్రారంభం_L1
        రాయి("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#లేకపోతే
        రాయి("%p_pd = pgd\n", vspace);
        కొరకు (మాట_t i = 0; i < PT_సూచీ_బిట్లు; i++) {
            pte_t *pt_స్థానం = vspace + i;
            ఒకవేళ (pte_pte_పట్టిక_ptr_get_present(pt_స్థానం)) {
                రాయి("pud_%p_%04lu = pud\n", pt_స్థానం, i);
                arm64_obj_pud_print_slots(pt_స్థానం);
            }
        }
#అయితే_ముగింపు
    }
}

#అయితే_ముగింపు /* CONFIG_PRINTING */

శూన్య debug_capDL(శూన్య)
{
    రాయి("arch aarch64\n");
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
