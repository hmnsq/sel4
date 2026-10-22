/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_DEBUG_BUILD

#అన్తర్భావ <machine/capdl.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <machine/timer.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <kernel/cspace.h>
#నిర్వచితమైతే CONFIG_KERNEL_MCS
#అన్తర్భావ <kernel/sporadic.h>
#అయితే_ముగింపు

#నిర్వచించు చూసిన_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t చూసిన_list[చూసిన_SZ];
పూర్ణాంకం watermark = 0;

శూన్య కూడు_గమ్యం_చూసిన(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    ఒకవేళ (watermark < చూసిన_SZ) {
        చూసిన_list[watermark] = c;
        watermark++;
    }
}

శూన్య reset_చూసిన_list(శూన్య)
{
    memset(చూసిన_list, 0, చూసిన_SZ * పరిమాణం(చూసిన_list[0]));
    watermark = 0;
}

bool_t చూసిన(cap_t c)
{
    కొరకు (పూర్ణాంకం i = 0; i < watermark; i++) {
        ఒకవేళ (same_cap(చూసిన_list[i], c)) {
            తిరిగి true;
        }
    }
    తిరిగి false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    తిరిగి (a.పదాలు[0] == b.పదాలు[0] && a.పదాలు[1] == b.పదాలు[1]);
}

/* Return true if strings are the same */
స్థిర అంతఃవరుస bool_t అక్షరశ్రేణులు_equal(స్థిరరాశి అక్షరం *str1, స్థిరరాశి అక్షరం *str2)
{
    అయితే (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    తిరిగి !(*(స్థిరరాశి గుర్తులేని అక్షరం *)str1 - * (స్థిరరాశి గుర్తులేని అక్షరం *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t మూలం_or_idle_tcb(tcb_t *tcb)
{
    తిరిగి (అక్షరశ్రేణులు_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || అక్షరశ్రేణులు_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#నిర్వచితమైతే CONFIG_PRINTING

శూన్య obj_tcb_print_attrs(tcb_t *tcb)
{
    రాయి("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)tcb->tcbIPCBuffer,
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)getRestartPC(tcb),
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)get_tcb_sp(tcb),
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)tcb->tcb_ప్రాధాన్యత,
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)tcb->tcbMCP);

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    రాయి(", affinity: %lu", (దీర్ఘ గుర్తులేని పూర్ణాంకం)tcb->tcbAffinity);
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    /* init */

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    ఒకవేళ (cap_get_capType(ep_cap) != cap_null_cap) {
        రాయి(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#అయితే_ముగింపు

    రాయి(", dom: %ld)\n", tcb->tcbDomain);
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS

స్థిర అంతఃవరుస ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t కూడిక_2 = refill_తల(sc)->rAmount;
    మాట_t current = sc->sc_Refill_తల;

    అయితే (current != sc->sc_Refill_తోక) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        కూడిక_2 += refill_సూచీ(sc, current)->rAmount;
    }

    తిరిగి కూడిక_2;
}

శూన్య obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    రాయి("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_మాట" bits)\n",
           ticks_గమ్యం_Us(period), period,
           ticks_గమ్యం_Us(budget), budget,
           (మాట_t)cap_sched_context_cap_get_cap_SCSize_బిట్లు(sc_cap));
}
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

శూన్య obj_ut_print_attrs(cte_t *స్థానం, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    రాయి("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (శూన్య *)cap_untyped_cap_get_capPtr(స్థానం->cap),
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_untyped_cap_get_cap_ఖండం_కొలత(స్థానం->cap),
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_untyped_cap_get_cap_ఖండం_కొలత(స్థానం->cap),
           మాట_PTR(cap_untyped_cap_get_capPtr(స్థానం->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    కొరకు (cte_t *తదుపరి_Ptr = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(స్థానం->cteMDBNode));
         తదుపరి_Ptr && is_MDBParent_యొక్క(స్థానం, తదుపరి_Ptr);
         తదుపరి_Ptr = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(స్థానం->cteMDBNode))) {
        ఒకవేళ (!sameRegionAs(స్థానం->cap, తదుపరి_Ptr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(తదుపరి_Ptr->cap);
        }
    }
    రాయి("}\n");
}

శూన్య obj_cnode_print_attrs(cap_t cnode)
{
    రాయి("(%lu bits)\n", (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_cnode_cap_get_capCNodeRadix(cnode));
}

శూన్య obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    ఒకవేళ (చూసిన(cnode)) {
        తిరిగి;
    }
    కూడు_గమ్యం_చూసిన(cnode);
    రాయి("%p_cnode = cnode ", (శూన్య *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    మాట_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    కొరకు (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_స్థానం_ret_t c = lookup_Cap_And_స్థానం(tcb, i);
        ఒకవేళ (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.స్థానం, tcb);

        } లేకపోతే ఒకవేళ (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } లేకపోతే ఒకవేళ (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

శూన్య cap_cnode_print_attrs(cap_t cnode)
{
    రాయి("(guard: %lu, guard_size: %lu)\n",
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_cnode_cap_get_capCNodeGuard(cnode),
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_cnode_cap_get_cap_CNode_Guard_కొలత(cnode));
}

శూన్య cap_ep_print_attrs(cap_t ep)
{
    రాయి("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? అక్షరం_రాయి('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? అక్షరం_రాయి('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? అక్షరం_రాయి('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_జవాబు(ep) ? అక్షరం_రాయి('P') : 0;
    దీర్ఘ గుర్తులేని పూర్ణాంకం badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? రాయి(", badge: %lu)\n", badge) : రాయి(")\n");
}

శూన్య cap_ntfn_print_attrs(cap_t ntfn)
{
    రాయి("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? అక్షరం_రాయి('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? అక్షరం_రాయి('W') : 0;
    దీర్ఘ గుర్తులేని పూర్ణాంకం badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? రాయి(", badge: %lu)\n", badge) : రాయి(")\n");
}

/*
 * print object slots
 */

శూన్య obj_tcb_print_slots(tcb_t *tcb)
{
    రాయి("%p_tcb {\n", tcb);

    /* CSpace root */
    ఒకవేళ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        రాయి("cspace: %p_cnode ",
               (శూన్య *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    ఒకవేళ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        రాయి("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_మూలం_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    ఒకవేళ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_బఫరు)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_బఫరు_స్థానం(tcb);
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    ఒకవేళ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        రాయి("fault_ep_slot: %p_ep ",
               (శూన్య *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    ఒకవేళ (tcb->tcbSchedContext) {
        రాయి("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    ఒకవేళ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        రాయి("temp_fault_ep_slot: %p_ep ",
               (శూన్య *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# లేకపోతే
    /* Reply cap slot */
    ఒకవేళ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_జవాబు)->cap) != cap_null_cap) {
        రాయి("reply_slot: %p_reply\n",
               (శూన్య *)cap_జవాబు_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_జవాబు)->cap));
    }

    /* TCB of most recent IPC sender */
    ఒకవేళ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        రాయి("caller_slot: %p_tcb\n", caller);
    }
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */
    రాయి("}\n");
}

/* TBD: deal with nested cnodes */
శూన్య obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t మూలం = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    ఒకవేళ (cap_get_capType(మూలం) != cap_cnode_cap) {
        తిరిగి;
    }

    మాట_t radix = cap_cnode_cap_get_capCNodeRadix(మూలం);
    ఒకవేళ (చూసిన(మూలం)) {
        తిరిగి;
    }
    కూడు_గమ్యం_చూసిన(మూలం);

    రాయి("%p_cnode {\n", (శూన్య *)cap_cnode_cap_get_capCNodePtr(మూలం));

    కొరకు (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_స్థానం_ret_t c = lookup_Cap_And_స్థానం(tcb, i);
        ఒకవేళ (cap_get_capType(c.cap) != cap_null_cap) {
            రాయి("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    రాయి("}\n");

    కొరకు (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_స్థానం_ret_t c = lookup_Cap_And_స్థానం(tcb, i);
        ఒకవేళ (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

శూన్య obj_irq_print_maps(శూన్య)
{
    రాయి("irq maps {\n");

    కొరకు (se_L4_మాట లక్ష్యం = 0; లక్ష్యం < CONFIG_MAX_అంకె_NODES; లక్ష్యం++) {
        కొరకు (గుర్తులేని i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_గమ్యం_IRQT(లక్ష్యం, i);
            ఒకవేళ (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_గమ్యం_IDX(irq)].cap;
                ఒకవేళ (cap_get_capType(cap) != cap_null_cap) {
                    రాయి("%d: 0x%lx_%lu_irq\n",
                           i,
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (దీర్ఘ గుర్తులేని పూర్ణాంకం)irq.irq,
#లేకపోతే
                           (దీర్ఘ గుర్తులేని పూర్ణాంకం)irq,
#అయితే_ముగింపు
                           (దీర్ఘ గుర్తులేని పూర్ణాంకం)లక్ష్యం);
                }
            }
        }
    }
    రాయి("}\n");
}

శూన్య obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_గమ్యం_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    ఒకవేళ (isIRQActive(irq)) {
        రాయి("0x%lx_%lu_irq {\n",
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (దీర్ఘ గుర్తులేని పూర్ణాంకం)irq.irq,
#లేకపోతే
               (దీర్ఘ గుర్తులేని పూర్ణాంకం)irq,
#అయితే_ముగింపు
               (దీర్ఘ గుర్తులేని పూర్ణాంకం)IRQT_గమ్యం_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_గమ్యం_IDX(irq)].cap;
        ఒకవేళ (cap_get_capType(ntfn_cap) != cap_null_cap) {
            రాయి("0x0: ");
            print_cap(ntfn_cap);
        }
        రాయి("}\n");
    }
}

శూన్య print_objects(శూన్య)
{
    కొరకు (tcb_t *curr = కణుపు_STATE(ksDebugTCBs); curr != ఖాళీ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_తదుపరి) {
        ఒకవేళ (మూలం_or_idle_tcb(curr)) {
            కొనసాగు;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    కొరకు (tcb_t *curr = కణుపు_STATE(ksDebugTCBs); curr != ఖాళీ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_తదుపరి) {
        ఒకవేళ (మూలం_or_idle_tcb(curr)) {
            కొనసాగు;
        }

        /* print the tcb as objects */
        రాయి("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        ఒకవేళ (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

శూన్య print_caps(శూన్య)
{
    కొరకు (tcb_t *curr = కణుపు_STATE(ksDebugTCBs); curr != ఖాళీ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_తదుపరి) {
        ఒకవేళ (మూలం_or_idle_tcb(curr)) {
            కొనసాగు;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

శూన్య print_cap(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_endpoint_cap: {
        రాయి("%p_ep ",
               (శూన్య *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        ఆపు;
    }
    స్థితి cap_notification_cap: {
        రాయి("%p_notification ",
               (శూన్య *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        ఆపు;
    }
    స్థితి cap_untyped_cap: {
        రాయి("%p_untyped\n",
               (శూన్య *)cap_untyped_cap_get_capPtr(cap));
        ఆపు;
    }
    స్థితి cap_thread_cap: {
        రాయి("%p_tcb\n",
               (శూన్య *)cap_thread_cap_get_capTCBPtr(cap));
        ఆపు;
    }
    స్థితి cap_cnode_cap: {
        రాయి("%p_cnode ",
               (శూన్య *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        ఆపు;
    }
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_జవాబు_cap: {
        రాయి("%p_reply\n",
               (శూన్య *)cap_జవాబు_cap_get_cap_జవాబు_Ptr(cap));
        ఆపు;
    }
    స్థితి cap_sched_context_cap: {
        రాయి("%p_sc\n",
               (శూన్య *)cap_sched_context_cap_get_capSCPtr(cap));
        ఆపు;
    }
    స్థితి cap_sched_control_cap: {
        రాయి("%lu_sched_control\n",
               (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_sched_control_cap_get_core(cap));
        ఆపు;
    }
#అయితే_ముగింపు
    స్థితి cap_irq_control_cap: {
        రాయి("irq_control\n"); /* only one in the system */
        ఆపు;
    }
    స్థితి cap_irq_handler_cap: {
        రాయి("%p_%lu_irq\n",
               (శూన్య *)cap_irq_handler_cap_get_capIRQ(cap),
               (దీర్ఘ గుర్తులేని పూర్ణాంకం)IRQT_గమ్యం_CORE(IDX_గమ్యం_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ఆపు;
    }
    అప్రమేయ: {
        print_cap_arch(cap);
        ఆపు;
    }
    }
}

శూన్య print_object(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_endpoint_cap: {
        రాయి("%p_ep = ep\n",
               (శూన్య *)cap_endpoint_cap_get_capEPPtr(cap));
        ఆపు;
    }
    స్థితి cap_notification_cap: {
        రాయి("%p_notification = notification\n",
               (శూన్య *)cap_notification_cap_get_capNtfnPtr(cap));
        ఆపు;
    }
    స్థితి cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        ఆపు;
    }
    స్థితి cap_cnode_cap: {
        ప్రతిజ్ఞా(!"should not happend");
    }
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_జవాబు_cap: {
        రాయి("%p_reply = rtreply\n",
               (శూన్య *)cap_జవాబు_cap_get_cap_జవాబు_Ptr(cap));
        ఆపు;
    }
    స్థితి cap_sched_context_cap: {
        రాయి("%p_sc = sc ",
               (శూన్య *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        ఆపు;
    }
#అయితే_ముగింపు
    స్థితి cap_irq_handler_cap: {
        రాయి("%p_%lu_irq = irq\n",
               (శూన్య *)cap_irq_handler_cap_get_capIRQ(cap),
               (దీర్ఘ గుర్తులేని పూర్ణాంకం)IRQT_గమ్యం_CORE(IDX_గమ్యం_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ఆపు;
    }
    అప్రమేయ:
        print_object_arch(cap);
        ఆపు;
    }
}

#అయితే_ముగింపు /* CONFIG_PRINTING */

#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
