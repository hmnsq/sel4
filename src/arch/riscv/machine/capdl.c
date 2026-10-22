/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_DEBUG_BUILD

#అన్తర్భావ <machine/capdl.h>
#అన్తర్భావ <arch/machine/capdl.h>

మాట_t get_tcb_sp(tcb_t *tcb)
{
    తిరిగి tcb->tcbArch.tcbContext.registers[SP];
}

#నిర్వచితమైతే CONFIG_PRINTING

స్థిర శూన్య obj_asidpool_print_attrs(cap_t asid_cap);
స్థిర శూన్య obj_చట్రం_print_attrs(paddr_t paddr);
స్థిర శూన్య riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, పూర్ణాంకం level);
స్థిర శూన్య cap_చట్రం_print_attrs_vptr(మాట_t vptr, pte_t *lvl1pt);
స్థిర శూన్య cap_చట్రం_print_attrs_pt(pte_t *pt_స్థానం);

స్థిర శూన్య obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    రాయి("(asid_high: 0x%lx)\n", ASID_ఎగువ(asid));
}

శూన్య print_ipc_బఫరు_స్థానం(tcb_t *tcb)
{
    మాట_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);

    రాయి("ipc_buffer_slot: ");
    cap_చట్రం_print_attrs_vptr(vptr, కనుగొను_ret.vspace_మూలం);
}

స్థిర శూన్య riscv_cap_pt_print_slots(pte_t *పై_Pt_స్థానం, మాట_t pt_సూచీ, పూర్ణాంకం level)
{
    pte_t *pt;
    ఒకవేళ (level == CONFIG_PT_స్థాయిలు) {
        రాయి("%p_pd {\n", పై_Pt_స్థానం);
        pt = పై_Pt_స్థానం;
    } లేకపోతే {
        రాయి("pt_%p_%04lu {\n", పై_Pt_స్థానం, pt_సూచీ);
        pt = getPPtrFromHWPTE(పై_Pt_స్థానం);
    }
    level -= 1;

    మాట_t pt_బిట్లు_ఎడమ = PT_సూచీ_బిట్లు * level + se_L4_పుట_బిట్లు;

    /* - 1 to avoid overflowing */
    కొరకు (మాట_t i = 0; i < బిట్టు(pt_బిట్లు_ఎడమ + PT_సూచీ_బిట్లు) - 1; i += (1 << (pt_బిట్లు_ఎడమ))) {
        మాట_t pt_స్థానం_సూచీ = ((i >> pt_బిట్లు_ఎడమ) & ముసుగు(PT_సూచీ_బిట్లు));
        pte_t *pt_స్థానం = pt + pt_స్థానం_సూచీ;
        ఒకవేళ (pte_ptr_get_valid(pt_స్థానం)) {
            ఒకవేళ (level) { /* pt */
                రాయి("0x%lx: pt_%p_%04lu\n", pt_స్థానం_సూచీ, pt_స్థానం, pt_స్థానం_సూచీ);
            } లేకపోతే { /*చట్రం*/
                రాయి("0x%lx: frame_%p_%04lu", pt_స్థానం_సూచీ, pt_స్థానం, pt_స్థానం_సూచీ);
                cap_చట్రం_print_attrs_pt(pt_స్థానం);
            }
        }
    }
    రాయి("}\n"); /* lvl1pt/pt */

    కొరకు (మాట_t i = 0; i < బిట్టు(pt_బిట్లు_ఎడమ + PT_సూచీ_బిట్లు) - 1; i += (1 << (pt_బిట్లు_ఎడమ))) {
        మాట_t pt_స్థానం_సూచీ = ((i >> pt_బిట్లు_ఎడమ) & ముసుగు(PT_సూచీ_బిట్లు));
        pte_t *pt_స్థానం = pt + pt_స్థానం_సూచీ;
        ఒకవేళ (pte_ptr_get_valid(pt_స్థానం)) {
            ఒకవేళ (level) { /* pt */
                riscv_cap_pt_print_slots(pt_స్థానం, pt_స్థానం_సూచీ, level);
            }
        }
    }
}

శూన్య obj_vtable_print_slots(tcb_t *tcb)
{
    ఒకవేళ (is_Valid_VTable_మూలం(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_యొక్క_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        కూడు_గమ్యం_చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_స్థాయిలు);
    }
}

స్థిర శూన్య cap_చట్రం_print_attrs_pt(pte_t *pt_స్థానం)
{
    రాయి("(");

    /* rights */
    ఒకవేళ (pte_ptr_get_read(pt_స్థానం)) {
        రాయి("R");
    }

    ఒకవేళ (pte_ptr_get_write(pt_స్థానం)) {
        రాయి("W");
    }

    ఒకవేళ (pte_ptr_get_execute(pt_స్థానం)) {
        రాయి("X");
    }

    /* cacheable not supported yet */

    రాయి(")\n");
}

స్థిర శూన్య cap_చట్రం_print_attrs_vptr(మాట_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    ప్రతిజ్ఞా(lu_ret.pt_బిట్లు_ఎడమ == se_L4_పుట_బిట్లు);
    మాట_t స్థానం = ((vptr >> lu_ret.pt_బిట్లు_ఎడమ) & ముసుగు(PT_సూచీ_బిట్లు));

    రాయి("frame_%p_%04lu ", lu_ret.pt_స్థానం, స్థానం);
    cap_చట్రం_print_attrs_pt(lu_ret.pt_స్థానం);
}

శూన్య print_cap_arch(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_పట్టిక_cap: {
        asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap);
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap);

        మాట_t pt_బిట్లు_ఎడమ = PT_సూచీ_బిట్లు * CONFIG_PT_స్థాయిలు + se_L4_పుట_బిట్లు;
        మాట_t స్థానం = ((vptr >> pt_బిట్లు_ఎడమ) & ముసుగు(PT_సూచీ_బిట్లు));
        ఒకవేళ (asid) {
            రాయి("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(కనుగొను_ret.vspace_మూలం, vptr).pt_స్థానం, స్థానం, (దీర్ఘ గుర్తులేని పూర్ణాంకం)asid);
        } లేకపోతే {
            రాయి("pt_%p_%04lu\n", lookupPTSlot(కనుగొను_ret.vspace_మూలం, vptr).pt_స్థానం, స్థానం);
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
        cap_చట్రం_print_attrs_vptr(vptr, కనుగొను_ret.vspace_మూలం);
        ఆపు;
    }
    స్థితి cap_asid_pool_cap: {
        రాయి("%p_asid_pool\n", (శూన్య *)cap_asid_pool_cap_get_capASIDPool(cap));
        ఆపు;
    }
    /* riscv specific caps */
    /* nothing */
    అప్రమేయ: {
        రాయి("[unknown cap %lu]\n", (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_get_capType(cap));
        ఆపు;
    }
    }
}

స్థిర శూన్య obj_చట్రం_print_attrs(paddr_t paddr)
{
    రాయి("(4k, paddr: 0x%p)\n", (శూన్య *)paddr);
}

శూన్య print_object_arch(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_చట్రం_cap:
    స్థితి cap_పుట_పట్టిక_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ఆపు;

    స్థితి cap_asid_pool_cap: {
        రాయి("%p_asid_pool = asid_pool ",
               (శూన్య *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ఆపు;
    }
    /* riscv specific caps */
    /* nothing */
    అప్రమేయ: {
        రాయి("[unknown object %lu]\n", (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_get_capType(cap));
        ఆపు;
    }
    }
}

స్థిర శూన్య riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, పూర్ణాంకం level)
{
    మాట_t pt_బిట్లు_ఎడమ = PT_సూచీ_బిట్లు * level + se_L4_పుట_బిట్లు;

    కొరకు (మాట_t i = 0; i < బిట్టు(pt_బిట్లు_ఎడమ + PT_సూచీ_బిట్లు); i += (1 << (pt_బిట్లు_ఎడమ))) {
        మాట_t pt_సూచీ = ((i >> pt_బిట్లు_ఎడమ) & ముసుగు(PT_సూచీ_బిట్లు));
        pte_t *pt_స్థానం = pt + pt_సూచీ;
        ఒకవేళ (pte_ptr_get_valid(pt_స్థానం)) {
            ఒకవేళ (level) { /* pt */
                రాయి("pt_%p_%04lu = pt\n", pt_స్థానం, pt_సూచీ);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(pt_స్థానం), level - 1);
            } లేకపోతే { /*చట్రం*/
                paddr_t paddr = pte_ptr_get_ppn(pt_స్థానం);
                రాయి("frame_%p_%04lu = frame ", pt_స్థానం, pt_సూచీ);
                obj_చట్రం_print_attrs(paddr);
            }
        }
    }
}

శూన్య obj_tcb_print_vtable(tcb_t *tcb)
{
    ఒకవేళ (is_Valid_VTable_మూలం(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        కూడు_గమ్యం_చూసిన(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_యొక్క_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        రాయి("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_స్థాయిలు - 1);
    }
}

#అయితే_ముగింపు /* CONFIG_PRINTING */

శూన్య debug_capDL(శూన్య)
{
    రాయి("arch riscv\n");
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
