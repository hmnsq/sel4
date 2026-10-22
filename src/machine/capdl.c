/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_DEBUG_BUILD

#समावेश <machine/capdl.h>
#समावेश <machine/registerset.h>
#समावेश <machine/timer.h>
#समावेश <string.h>
#समावेश <kernel/cspace.h>
#अगर_घोषित CONFIG_KERNEL_MCS
#समावेश <kernel/sporadic.h>
#पूर्ण_अगर

#घोषणा SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
पूर्णांक watermark = 0;

व्योम add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    अगर (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

व्योम reset_seen_list(व्योम)
{
    स_रखो(seen_list, 0, SEEN_SZ * माप(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    क्रम (पूर्णांक i = 0; i < watermark; i++) {
        अगर (same_cap(seen_list[i], c)) {
            वापस true;
        }
    }
    वापस false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    वापस (a.शब्दगण[0] == b.शब्दगण[0] && a.शब्दगण[1] == b.शब्दगण[1]);
}

/* Return true if strings are the same */
अटल अंतरभूत bool_t strings_equal(स्थिर अक्षर *str1, स्थिर अक्षर *str2)
{
    जबतक (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    वापस !(*(स्थिर अचिन्हित अक्षर *)str1 - * (स्थिर अचिन्हित अक्षर *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t मूल_or_idle_tcb(tcb_t *tcb)
{
    वापस (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#अगर_घोषित CONFIG_PRINTING

व्योम obj_tcb_print_attrs(tcb_t *tcb)
{
    म_लिखो("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (दीर्घ अचिन्हित पूर्णांक)tcb->tcbIPCBuffer,
           (दीर्घ अचिन्हित पूर्णांक)getRestartPC(tcb),
           (दीर्घ अचिन्हित पूर्णांक)get_tcb_sp(tcb),
           (दीर्घ अचिन्हित पूर्णांक)tcb->tcbPriority,
           (दीर्घ अचिन्हित पूर्णांक)tcb->tcbMCP);

#अगर_घोषित ENABLE_SMP_SUPPORT
    म_लिखो(", affinity: %lu", (दीर्घ अचिन्हित पूर्णांक)tcb->tcbAffinity);
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

    /* init */

#अगर_घोषित CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    अगर (cap_get_capType(ep_cap) != cap_null_cap) {
        म_लिखो(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#पूर्ण_अगर

    म_लिखो(", dom: %ld)\n", tcb->tcbDomain);
}

#अगर_घोषित CONFIG_KERNEL_MCS

अटल अंतरभूत ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t योग_2 = refill_शीर्ष(sc)->rAmount;
    word_t current = sc->sc_Refill_शीर्ष;

    जबतक (current != sc->sc_Refill_पुच्छ) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        योग_2 += refill_सूचकांक(sc, current)->rAmount;
    }

    वापस योग_2;
}

व्योम obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    म_लिखो("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_word" bits)\n",
           ticksToUs(period), period,
           ticksToUs(budget), budget,
           (word_t)cap_sched_context_cap_get_capSCSizeBits(sc_cap));
}
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

व्योम obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    म_लिखो("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (व्योम *)cap_untyped_cap_get_capPtr(slot->cap),
           (दीर्घ अचिन्हित पूर्णांक)cap_untyped_cap_get_cap_Block_आकार(slot->cap),
           (दीर्घ अचिन्हित पूर्णांक)cap_untyped_cap_get_cap_Block_आकार(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    क्रम (cte_t *अग्र_Ptr = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(slot->cteMDBNode));
         अग्र_Ptr && isMDBParentOf(slot, अग्र_Ptr);
         अग्र_Ptr = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(slot->cteMDBNode))) {
        अगर (!sameRegionAs(slot->cap, अग्र_Ptr->cap)) {
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
    म_लिखो("}\n");
}

व्योम obj_cnode_print_attrs(cap_t cnode)
{
    म_लिखो("(%lu bits)\n", (दीर्घ अचिन्हित पूर्णांक)cap_cnode_cap_get_capCNodeRadix(cnode));
}

व्योम obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    अगर (seen(cnode)) {
        वापस;
    }
    add_to_seen(cnode);
    म_लिखो("%p_cnode = cnode ", (व्योम *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    क्रम (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        अगर (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } अन्यथा अगर (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } अन्यथा अगर (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

व्योम cap_cnode_print_attrs(cap_t cnode)
{
    म_लिखो("(guard: %lu, guard_size: %lu)\n",
           (दीर्घ अचिन्हित पूर्णांक)cap_cnode_cap_get_capCNodeGuard(cnode),
           (दीर्घ अचिन्हित पूर्णांक)cap_cnode_cap_get_cap_CNode_Guard_आकार(cnode));
}

व्योम cap_ep_print_attrs(cap_t ep)
{
    म_लिखो("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? अक्षर_दो('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? अक्षर_दो('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? अक्षर_दो('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? अक्षर_दो('P') : 0;
    दीर्घ अचिन्हित पूर्णांक badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? म_लिखो(", badge: %lu)\n", badge) : म_लिखो(")\n");
}

व्योम cap_ntfn_print_attrs(cap_t ntfn)
{
    म_लिखो("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? अक्षर_दो('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? अक्षर_दो('W') : 0;
    दीर्घ अचिन्हित पूर्णांक badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? म_लिखो(", badge: %lu)\n", badge) : म_लिखो(")\n");
}

/*
 * print object slots
 */

व्योम obj_tcb_print_slots(tcb_t *tcb)
{
    म_लिखो("%p_tcb {\n", tcb);

    /* CSpace root */
    अगर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        म_लिखो("cspace: %p_cnode ",
               (व्योम *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    अगर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        म_लिखो("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_मूल_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    अगर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_बफर)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_बफर_slot(tcb);
    }

#अगर_घोषित CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    अगर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        म_लिखो("fault_ep_slot: %p_ep ",
               (व्योम *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    अगर (tcb->tcbSchedContext) {
        म_लिखो("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    अगर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        म_लिखो("temp_fault_ep_slot: %p_ep ",
               (व्योम *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# अन्यथा
    /* Reply cap slot */
    अगर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        म_लिखो("reply_slot: %p_reply\n",
               (व्योम *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    अगर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        म_लिखो("caller_slot: %p_tcb\n", caller);
    }
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */
    म_लिखो("}\n");
}

/* TBD: deal with nested cnodes */
व्योम obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t मूल = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    अगर (cap_get_capType(मूल) != cap_cnode_cap) {
        वापस;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(मूल);
    अगर (seen(मूल)) {
        वापस;
    }
    add_to_seen(मूल);

    म_लिखो("%p_cnode {\n", (व्योम *)cap_cnode_cap_get_capCNodePtr(मूल));

    क्रम (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        अगर (cap_get_capType(c.cap) != cap_null_cap) {
            म_लिखो("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    म_लिखो("}\n");

    क्रम (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        अगर (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

व्योम obj_irq_print_maps(व्योम)
{
    म_लिखो("irq maps {\n");

    क्रम (seL4_Word target = 0; target < CONFIG_MAX_अंक_NODES; target++) {
        क्रम (अचिन्हित i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            अगर (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                अगर (cap_get_capType(cap) != cap_null_cap) {
                    म_लिखो("%d: 0x%lx_%lu_irq\n",
                           i,
#अगर defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (दीर्घ अचिन्हित पूर्णांक)irq.irq,
#अन्यथा
                           (दीर्घ अचिन्हित पूर्णांक)irq,
#पूर्ण_अगर
                           (दीर्घ अचिन्हित पूर्णांक)target);
                }
            }
        }
    }
    म_लिखो("}\n");
}

व्योम obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    अगर (isIRQActive(irq)) {
        म_लिखो("0x%lx_%lu_irq {\n",
#अगर defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (दीर्घ अचिन्हित पूर्णांक)irq.irq,
#अन्यथा
               (दीर्घ अचिन्हित पूर्णांक)irq,
#पूर्ण_अगर
               (दीर्घ अचिन्हित पूर्णांक)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        अगर (cap_get_capType(ntfn_cap) != cap_null_cap) {
            म_लिखो("0x0: ");
            print_cap(ntfn_cap);
        }
        म_लिखो("}\n");
    }
}

व्योम print_objects(व्योम)
{
    क्रम (tcb_t *curr = गाँठ_STATE(ksDebugTCBs); curr != शून्य; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        अगर (मूल_or_idle_tcb(curr)) {
            जारी;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    क्रम (tcb_t *curr = गाँठ_STATE(ksDebugTCBs); curr != शून्य; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        अगर (मूल_or_idle_tcb(curr)) {
            जारी;
        }

        /* print the tcb as objects */
        म_लिखो("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        अगर (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

व्योम print_caps(व्योम)
{
    क्रम (tcb_t *curr = गाँठ_STATE(ksDebugTCBs); curr != शून्य; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_अग्र) {
        अगर (मूल_or_idle_tcb(curr)) {
            जारी;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

व्योम print_cap(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_endpoint_cap: {
        म_लिखो("%p_ep ",
               (व्योम *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        अवरोध;
    }
    हाल cap_notification_cap: {
        म_लिखो("%p_notification ",
               (व्योम *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        अवरोध;
    }
    हाल cap_untyped_cap: {
        म_लिखो("%p_untyped\n",
               (व्योम *)cap_untyped_cap_get_capPtr(cap));
        अवरोध;
    }
    हाल cap_thread_cap: {
        म_लिखो("%p_tcb\n",
               (व्योम *)cap_thread_cap_get_capTCBPtr(cap));
        अवरोध;
    }
    हाल cap_cnode_cap: {
        म_लिखो("%p_cnode ",
               (व्योम *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        अवरोध;
    }
#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_reply_cap: {
        म_लिखो("%p_reply\n",
               (व्योम *)cap_reply_cap_get_capReplyPtr(cap));
        अवरोध;
    }
    हाल cap_sched_context_cap: {
        म_लिखो("%p_sc\n",
               (व्योम *)cap_sched_context_cap_get_capSCPtr(cap));
        अवरोध;
    }
    हाल cap_sched_control_cap: {
        म_लिखो("%lu_sched_control\n",
               (दीर्घ अचिन्हित पूर्णांक)cap_sched_control_cap_get_core(cap));
        अवरोध;
    }
#पूर्ण_अगर
    हाल cap_irq_control_cap: {
        म_लिखो("irq_control\n"); /* only one in the system */
        अवरोध;
    }
    हाल cap_irq_handler_cap: {
        म_लिखो("%p_%lu_irq\n",
               (व्योम *)cap_irq_handler_cap_get_capIRQ(cap),
               (दीर्घ अचिन्हित पूर्णांक)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        अवरोध;
    }
    शेष: {
        print_cap_arch(cap);
        अवरोध;
    }
    }
}

व्योम print_object(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_endpoint_cap: {
        म_लिखो("%p_ep = ep\n",
               (व्योम *)cap_endpoint_cap_get_capEPPtr(cap));
        अवरोध;
    }
    हाल cap_notification_cap: {
        म_लिखो("%p_notification = notification\n",
               (व्योम *)cap_notification_cap_get_capNtfnPtr(cap));
        अवरोध;
    }
    हाल cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        अवरोध;
    }
    हाल cap_cnode_cap: {
        निश्चित(!"should not happend");
    }
#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_reply_cap: {
        म_लिखो("%p_reply = rtreply\n",
               (व्योम *)cap_reply_cap_get_capReplyPtr(cap));
        अवरोध;
    }
    हाल cap_sched_context_cap: {
        म_लिखो("%p_sc = sc ",
               (व्योम *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        अवरोध;
    }
#पूर्ण_अगर
    हाल cap_irq_handler_cap: {
        म_लिखो("%p_%lu_irq = irq\n",
               (व्योम *)cap_irq_handler_cap_get_capIRQ(cap),
               (दीर्घ अचिन्हित पूर्णांक)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        अवरोध;
    }
    शेष:
        print_object_arch(cap);
        अवरोध;
    }
}

#पूर्ण_अगर /* CONFIG_PRINTING */

#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
