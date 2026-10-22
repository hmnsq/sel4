/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD

#अन्तर्भाव <machine/capdl.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <machine/timer.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#अन्तर्भाव <kernel/sporadic.h>
#यदि_अन्तः

#परिभाषय आलोकितम्_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t आलोकितम्_list[आलोकितम्_SZ];
पूर्णाङ्क watermark = 0;

शून्य सङ्कलय_गन्तव्यम्_आलोकितम्(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    यदि (watermark < आलोकितम्_SZ) {
        आलोकितम्_list[watermark] = c;
        watermark++;
    }
}

शून्य reset_आलोकितम्_list(शून्य)
{
    स्मृतिं_पूरय(आलोकितम्_list, 0, आलोकितम्_SZ * परिमाण(आलोकितम्_list[0]));
    watermark = 0;
}

bool_t आलोकितम्(cap_t c)
{
    प्रति (पूर्णाङ्क i = 0; i < watermark; i++) {
        यदि (same_cap(आलोकितम्_list[i], c)) {
            प्रत्यागम true;
        }
    }
    प्रत्यागम false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    प्रत्यागम (a.शब्दाः[0] == b.शब्दाः[0] && a.शब्दाः[1] == b.शब्दाः[1]);
}

/* Return true if strings are the same */
स्थिर अन्तःस्थ bool_t वाक्यानि_equal(ध्रुव वर्ण *str1, ध्रुव वर्ण *str2)
{
    यावत् (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    प्रत्यागम !(*(ध्रुव अचिह्नित वर्ण *)str1 - * (ध्रुव अचिह्नित वर्ण *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t मूलम्_or_idle_tcb(tcb_t *tcb)
{
    प्रत्यागम (वाक्यानि_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || वाक्यानि_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#यदि_परिभाषितम् CONFIG_PRINTING

शून्य obj_tcb_print_attrs(tcb_t *tcb)
{
    प्रारूप_लिख("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (दीर्घ अचिह्नित पूर्णाङ्क)tcb->tcbIPCBuffer,
           (दीर्घ अचिह्नित पूर्णाङ्क)getRestartPC(tcb),
           (दीर्घ अचिह्नित पूर्णाङ्क)get_tcb_sp(tcb),
           (दीर्घ अचिह्नित पूर्णाङ्क)tcb->tcb_प्राथमिकता,
           (दीर्घ अचिह्नित पूर्णाङ्क)tcb->tcbMCP);

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    प्रारूप_लिख(", affinity: %lu", (दीर्घ अचिह्नित पूर्णाङ्क)tcb->tcbAffinity);
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

    /* init */

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    यदि (cap_get_capType(ep_cap) != cap_null_cap) {
        प्रारूप_लिख(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#यदि_अन्तः

    प्रारूप_लिख(", dom: %ld)\n", tcb->tcbDomain);
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS

स्थिर अन्तःस्थ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t योग_2 = refill_शीर्ष(sc)->rAmount;
    word_t current = sc->sc_Refill_शीर्ष;

    यावत् (current != sc->sc_Refill_पुच्छ) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        योग_2 += refill_सूचकांक(sc, current)->rAmount;
    }

    प्रत्यागम योग_2;
}

शून्य obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    प्रारूप_लिख("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_word" bits)\n",
           ticks_गन्तव्यम्_Us(period), period,
           ticks_गन्तव्यम्_Us(budget), budget,
           (word_t)cap_sched_context_cap_get_cap_SCSize_द्वयङ्काः(sc_cap));
}
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

शून्य obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    प्रारूप_लिख("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (शून्य *)cap_untyped_cap_get_capPtr(slot->cap),
           (दीर्घ अचिह्नित पूर्णाङ्क)cap_untyped_cap_get_cap_खण्डम्_आकार(slot->cap),
           (दीर्घ अचिह्नित पूर्णाङ्क)cap_untyped_cap_get_cap_खण्डम्_आकार(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    प्रति (cte_t *अग्र_Ptr = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(slot->cteMDBNode));
         अग्र_Ptr && is_MDBParent_सम्बन्धि(slot, अग्र_Ptr);
         अग्र_Ptr = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(slot->cteMDBNode))) {
        यदि (!sameRegionAs(slot->cap, अग्र_Ptr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(अग्र_Ptr->cap);
        }
    }
    प्रारूप_लिख("}\n");
}

शून्य obj_cnode_print_attrs(cap_t cnode)
{
    प्रारूप_लिख("(%lu bits)\n", (दीर्घ अचिह्नित पूर्णाङ्क)cap_cnode_cap_get_capCNodeRadix(cnode));
}

शून्य obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    यदि (आलोकितम्(cnode)) {
        प्रत्यागम;
    }
    सङ्कलय_गन्तव्यम्_आलोकितम्(cnode);
    प्रारूप_लिख("%p_cnode = cnode ", (शून्य *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    प्रति (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        यदि (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } अन्यथा यदि (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } अन्यथा यदि (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

शून्य cap_cnode_print_attrs(cap_t cnode)
{
    प्रारूप_लिख("(guard: %lu, guard_size: %lu)\n",
           (दीर्घ अचिह्नित पूर्णाङ्क)cap_cnode_cap_get_capCNodeGuard(cnode),
           (दीर्घ अचिह्नित पूर्णाङ्क)cap_cnode_cap_get_cap_CNode_Guard_आकार(cnode));
}

शून्य cap_ep_print_attrs(cap_t ep)
{
    प्रारूप_लिख("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? अक्षरं_लिख('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? अक्षरं_लिख('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? अक्षरं_लिख('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(ep) ? अक्षरं_लिख('P') : 0;
    दीर्घ अचिह्नित पूर्णाङ्क badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? प्रारूप_लिख(", badge: %lu)\n", badge) : प्रारूप_लिख(")\n");
}

शून्य cap_ntfn_print_attrs(cap_t ntfn)
{
    प्रारूप_लिख("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? अक्षरं_लिख('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? अक्षरं_लिख('W') : 0;
    दीर्घ अचिह्नित पूर्णाङ्क badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? प्रारूप_लिख(", badge: %lu)\n", badge) : प्रारूप_लिख(")\n");
}

/*
 * print object slots
 */

शून्य obj_tcb_print_slots(tcb_t *tcb)
{
    प्रारूप_लिख("%p_tcb {\n", tcb);

    /* CSpace root */
    यदि (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        प्रारूप_लिख("cspace: %p_cnode ",
               (शून्य *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    यदि (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        प्रारूप_लिख("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_मूलम्_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    यदि (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_बफर)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_बफर_slot(tcb);
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    यदि (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        प्रारूप_लिख("fault_ep_slot: %p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    यदि (tcb->tcbSchedContext) {
        प्रारूप_लिख("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    यदि (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        प्रारूप_लिख("temp_fault_ep_slot: %p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# अन्यथा
    /* Reply cap slot */
    यदि (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_प्रत्युत्तरम्)->cap) != cap_null_cap) {
        प्रारूप_लिख("reply_slot: %p_reply\n",
               (शून्य *)cap_प्रत्युत्तरम्_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_प्रत्युत्तरम्)->cap));
    }

    /* TCB of most recent IPC sender */
    यदि (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        प्रारूप_लिख("caller_slot: %p_tcb\n", caller);
    }
#यदि_अन्तः /* CONFIG_KERNEL_MCS */
    प्रारूप_लिख("}\n");
}

/* TBD: deal with nested cnodes */
शून्य obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t मूलम् = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    यदि (cap_get_capType(मूलम्) != cap_cnode_cap) {
        प्रत्यागम;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(मूलम्);
    यदि (आलोकितम्(मूलम्)) {
        प्रत्यागम;
    }
    सङ्कलय_गन्तव्यम्_आलोकितम्(मूलम्);

    प्रारूप_लिख("%p_cnode {\n", (शून्य *)cap_cnode_cap_get_capCNodePtr(मूलम्));

    प्रति (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        यदि (cap_get_capType(c.cap) != cap_null_cap) {
            प्रारूप_लिख("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    प्रारूप_लिख("}\n");

    प्रति (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        यदि (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

शून्य obj_irq_print_maps(शून्य)
{
    प्रारूप_लिख("irq maps {\n");

    प्रति (seL4_Word target = 0; target < CONFIG_MAX_अङ्कः_NODES; target++) {
        प्रति (अचिह्नित i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_गन्तव्यम्_IRQT(target, i);
            यदि (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_गन्तव्यम्_IDX(irq)].cap;
                यदि (cap_get_capType(cap) != cap_null_cap) {
                    प्रारूप_लिख("%d: 0x%lx_%lu_irq\n",
                           i,
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (दीर्घ अचिह्नित पूर्णाङ्क)irq.irq,
#अन्यथा
                           (दीर्घ अचिह्नित पूर्णाङ्क)irq,
#यदि_अन्तः
                           (दीर्घ अचिह्नित पूर्णाङ्क)target);
                }
            }
        }
    }
    प्रारूप_लिख("}\n");
}

शून्य obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_गन्तव्यम्_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    यदि (isIRQActive(irq)) {
        प्रारूप_लिख("0x%lx_%lu_irq {\n",
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (दीर्घ अचिह्नित पूर्णाङ्क)irq.irq,
#अन्यथा
               (दीर्घ अचिह्नित पूर्णाङ्क)irq,
#यदि_अन्तः
               (दीर्घ अचिह्नित पूर्णाङ्क)IRQT_गन्तव्यम्_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_गन्तव्यम्_IDX(irq)].cap;
        यदि (cap_get_capType(ntfn_cap) != cap_null_cap) {
            प्रारूप_लिख("0x0: ");
            print_cap(ntfn_cap);
        }
        प्रारूप_लिख("}\n");
    }
}

शून्य print_objects(शून्य)
{
    प्रति (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        यदि (मूलम्_or_idle_tcb(curr)) {
            अनुवर्त;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    प्रति (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        यदि (मूलम्_or_idle_tcb(curr)) {
            अनुवर्त;
        }

        /* print the tcb as objects */
        प्रारूप_लिख("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        यदि (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

शून्य print_caps(शून्य)
{
    प्रति (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        यदि (मूलम्_or_idle_tcb(curr)) {
            अनुवर्त;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

शून्य print_cap(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap: {
        प्रारूप_लिख("%p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        भङ्ग;
    }
    पक्ष cap_notification_cap: {
        प्रारूप_लिख("%p_notification ",
               (शून्य *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        भङ्ग;
    }
    पक्ष cap_untyped_cap: {
        प्रारूप_लिख("%p_untyped\n",
               (शून्य *)cap_untyped_cap_get_capPtr(cap));
        भङ्ग;
    }
    पक्ष cap_thread_cap: {
        प्रारूप_लिख("%p_tcb\n",
               (शून्य *)cap_thread_cap_get_capTCBPtr(cap));
        भङ्ग;
    }
    पक्ष cap_cnode_cap: {
        प्रारूप_लिख("%p_cnode ",
               (शून्य *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        भङ्ग;
    }
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_प्रत्युत्तरम्_cap: {
        प्रारूप_लिख("%p_reply\n",
               (शून्य *)cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(cap));
        भङ्ग;
    }
    पक्ष cap_sched_context_cap: {
        प्रारूप_लिख("%p_sc\n",
               (शून्य *)cap_sched_context_cap_get_capSCPtr(cap));
        भङ्ग;
    }
    पक्ष cap_sched_control_cap: {
        प्रारूप_लिख("%lu_sched_control\n",
               (दीर्घ अचिह्नित पूर्णाङ्क)cap_sched_control_cap_get_core(cap));
        भङ्ग;
    }
#यदि_अन्तः
    पक्ष cap_irq_control_cap: {
        प्रारूप_लिख("irq_control\n"); /* only one in the system */
        भङ्ग;
    }
    पक्ष cap_irq_handler_cap: {
        प्रारूप_लिख("%p_%lu_irq\n",
               (शून्य *)cap_irq_handler_cap_get_capIRQ(cap),
               (दीर्घ अचिह्नित पूर्णाङ्क)IRQT_गन्तव्यम्_CORE(IDX_गन्तव्यम्_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        भङ्ग;
    }
    स्वभाव: {
        print_cap_arch(cap);
        भङ्ग;
    }
    }
}

शून्य print_object(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap: {
        प्रारूप_लिख("%p_ep = ep\n",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(cap));
        भङ्ग;
    }
    पक्ष cap_notification_cap: {
        प्रारूप_लिख("%p_notification = notification\n",
               (शून्य *)cap_notification_cap_get_capNtfnPtr(cap));
        भङ्ग;
    }
    पक्ष cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        भङ्ग;
    }
    पक्ष cap_cnode_cap: {
        प्रतिज्ञा(!"should not happend");
    }
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_प्रत्युत्तरम्_cap: {
        प्रारूप_लिख("%p_reply = rtreply\n",
               (शून्य *)cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(cap));
        भङ्ग;
    }
    पक्ष cap_sched_context_cap: {
        प्रारूप_लिख("%p_sc = sc ",
               (शून्य *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        भङ्ग;
    }
#यदि_अन्तः
    पक्ष cap_irq_handler_cap: {
        प्रारूप_लिख("%p_%lu_irq = irq\n",
               (शून्य *)cap_irq_handler_cap_get_capIRQ(cap),
               (दीर्घ अचिह्नित पूर्णाङ्क)IRQT_गन्तव्यम्_CORE(IDX_गन्तव्यम्_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        भङ्ग;
    }
    स्वभाव:
        print_object_arch(cap);
        भङ्ग;
    }
}

#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
