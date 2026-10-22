/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_DEBUG_BUILD

#अन्तर्भाव <machine/capdl.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <machine/timer.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#अन्तर्भाव <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
पूर्णांक watermark = 0;

शून्य add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    जर (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

शून्य reset_seen_list(शून्य)
{
    स_रखो(seen_list, 0, SEEN_SZ * आकार(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    साठी (पूर्णांक i = 0; i < watermark; i++) {
        जर (same_cap(seen_list[i], c)) {
            परत true;
        }
    }
    परत false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    परत (a.शब्द[0] == b.शब्द[0] && a.शब्द[1] == b.शब्द[1]);
}

/* Return true if strings are the same */
स्थिर ओळीत bool_t strings_equal(स्थिरांक अक्षर *str1, स्थिरांक अक्षर *str2)
{
    पर्यंत (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    परत !(*(स्थिरांक अचिन्हित अक्षर *)str1 - * (स्थिरांक अचिन्हित अक्षर *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t मूळ_or_idle_tcb(tcb_t *tcb)
{
    परत (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

शून्य obj_tcb_print_attrs(tcb_t *tcb)
{
    म_लिखो("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (दीर्घ अचिन्हित पूर्णांक)tcb->tcbIPCBuffer,
           (दीर्घ अचिन्हित पूर्णांक)getRestartPC(tcb),
           (दीर्घ अचिन्हित पूर्णांक)get_tcb_sp(tcb),
           (दीर्घ अचिन्हित पूर्णांक)tcb->tcbPriority,
           (दीर्घ अचिन्हित पूर्णांक)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    म_लिखो(", affinity: %lu", (दीर्घ अचिन्हित पूर्णांक)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    जर (cap_get_capType(ep_cap) != cap_null_cap) {
        म_लिखो(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    म_लिखो(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

स्थिर ओळीत ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    पर्यंत (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    परत sum;
}

शून्य obj_sc_print_attrs(cap_t sc_cap)
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
#endif /* CONFIG_KERNEL_MCS */

शून्य obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    म_लिखो("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (शून्य *)cap_untyped_cap_get_capPtr(slot->cap),
           (दीर्घ अचिन्हित पूर्णांक)cap_untyped_cap_get_capBlockSize(slot->cap),
           (दीर्घ अचिन्हित पूर्णांक)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    साठी (cte_t *nextPtr = CTE_PTR(mdb_गाठ_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_गाठ_get_mdb_Next(slot->cteMDBNode))) {
        जर (!sameRegionAs(slot->cap, nextPtr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(nextPtr->cap);
        }
    }
    म_लिखो("}\n");
}

शून्य obj_cnode_print_attrs(cap_t cnode)
{
    म_लिखो("(%lu bits)\n", (दीर्घ अचिन्हित पूर्णांक)cap_cnode_cap_get_capCNodeRadix(cnode));
}

शून्य obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    जर (seen(cnode)) {
        परत;
    }
    add_to_seen(cnode);
    म_लिखो("%p_cnode = cnode ", (शून्य *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    साठी (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        जर (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } अन्यथा जर (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } अन्यथा जर (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

शून्य cap_cnode_print_attrs(cap_t cnode)
{
    म_लिखो("(guard: %lu, guard_size: %lu)\n",
           (दीर्घ अचिन्हित पूर्णांक)cap_cnode_cap_get_capCNodeGuard(cnode),
           (दीर्घ अचिन्हित पूर्णांक)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

शून्य cap_ep_print_attrs(cap_t ep)
{
    म_लिखो("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? अक्षर_दो('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? अक्षर_दो('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? अक्षर_दो('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? अक्षर_दो('P') : 0;
    दीर्घ अचिन्हित पूर्णांक badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? म_लिखो(", badge: %lu)\n", badge) : म_लिखो(")\n");
}

शून्य cap_ntfn_print_attrs(cap_t ntfn)
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

शून्य obj_tcb_print_slots(tcb_t *tcb)
{
    म_लिखो("%p_tcb {\n", tcb);

    /* CSpace root */
    जर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        म_लिखो("cspace: %p_cnode ",
               (शून्य *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    जर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        म_लिखो("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_मूळ_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    जर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    जर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        म_लिखो("fault_ep_slot: %p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    जर (tcb->tcbSchedContext) {
        म_लिखो("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    जर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        म_लिखो("temp_fault_ep_slot: %p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# अन्यथा
    /* Reply cap slot */
    जर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        म_लिखो("reply_slot: %p_reply\n",
               (शून्य *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    जर (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        म_लिखो("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    म_लिखो("}\n");
}

/* TBD: deal with nested cnodes */
शून्य obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t मूळ = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    जर (cap_get_capType(मूळ) != cap_cnode_cap) {
        परत;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(मूळ);
    जर (seen(मूळ)) {
        परत;
    }
    add_to_seen(मूळ);

    म_लिखो("%p_cnode {\n", (शून्य *)cap_cnode_cap_get_capCNodePtr(मूळ));

    साठी (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        जर (cap_get_capType(c.cap) != cap_null_cap) {
            म_लिखो("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    म_लिखो("}\n");

    साठी (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        जर (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

शून्य obj_irq_print_maps(शून्य)
{
    म_लिखो("irq maps {\n");

    साठी (seL4_Word target = 0; target < CONFIG_MAX_अंक_NODES; target++) {
        साठी (अचिन्हित i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            जर (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                जर (cap_get_capType(cap) != cap_null_cap) {
                    म_लिखो("%d: 0x%lx_%lu_irq\n",
                           i,
#जर defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (दीर्घ अचिन्हित पूर्णांक)irq.irq,
#अन्यथा
                           (दीर्घ अचिन्हित पूर्णांक)irq,
#endif
                           (दीर्घ अचिन्हित पूर्णांक)target);
                }
            }
        }
    }
    म_लिखो("}\n");
}

शून्य obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    जर (isIRQActive(irq)) {
        म_लिखो("0x%lx_%lu_irq {\n",
#जर defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (दीर्घ अचिन्हित पूर्णांक)irq.irq,
#अन्यथा
               (दीर्घ अचिन्हित पूर्णांक)irq,
#endif
               (दीर्घ अचिन्हित पूर्णांक)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        जर (cap_get_capType(ntfn_cap) != cap_null_cap) {
            म_लिखो("0x0: ");
            print_cap(ntfn_cap);
        }
        म_लिखो("}\n");
    }
}

शून्य print_objects(शून्य)
{
    साठी (tcb_t *curr = गाठ_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        जर (मूळ_or_idle_tcb(curr)) {
            सुरूठेवा;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    साठी (tcb_t *curr = गाठ_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        जर (मूळ_or_idle_tcb(curr)) {
            सुरूठेवा;
        }

        /* print the tcb as objects */
        म_लिखो("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        जर (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

शून्य print_caps(शून्य)
{
    साठी (tcb_t *curr = गाठ_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        जर (मूळ_or_idle_tcb(curr)) {
            सुरूठेवा;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

शून्य print_cap(cap_t cap)
{
    निवड (cap_get_capType(cap)) {
    स्थिती cap_endpoint_cap: {
        म_लिखो("%p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        थांबा;
    }
    स्थिती cap_notification_cap: {
        म_लिखो("%p_notification ",
               (शून्य *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        थांबा;
    }
    स्थिती cap_untyped_cap: {
        म_लिखो("%p_untyped\n",
               (शून्य *)cap_untyped_cap_get_capPtr(cap));
        थांबा;
    }
    स्थिती cap_thread_cap: {
        म_लिखो("%p_tcb\n",
               (शून्य *)cap_thread_cap_get_capTCBPtr(cap));
        थांबा;
    }
    स्थिती cap_cnode_cap: {
        म_लिखो("%p_cnode ",
               (शून्य *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        थांबा;
    }
#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_reply_cap: {
        म_लिखो("%p_reply\n",
               (शून्य *)cap_reply_cap_get_capReplyPtr(cap));
        थांबा;
    }
    स्थिती cap_sched_context_cap: {
        म_लिखो("%p_sc\n",
               (शून्य *)cap_sched_context_cap_get_capSCPtr(cap));
        थांबा;
    }
    स्थिती cap_sched_control_cap: {
        म_लिखो("%lu_sched_control\n",
               (दीर्घ अचिन्हित पूर्णांक)cap_sched_control_cap_get_core(cap));
        थांबा;
    }
#endif
    स्थिती cap_irq_control_cap: {
        म_लिखो("irq_control\n"); /* only one in the system */
        थांबा;
    }
    स्थिती cap_irq_handler_cap: {
        म_लिखो("%p_%lu_irq\n",
               (शून्य *)cap_irq_handler_cap_get_capIRQ(cap),
               (दीर्घ अचिन्हित पूर्णांक)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        थांबा;
    }
    पूर्वनिर्धारित: {
        print_cap_arch(cap);
        थांबा;
    }
    }
}

शून्य print_object(cap_t cap)
{
    निवड (cap_get_capType(cap)) {
    स्थिती cap_endpoint_cap: {
        म_लिखो("%p_ep = ep\n",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(cap));
        थांबा;
    }
    स्थिती cap_notification_cap: {
        म_लिखो("%p_notification = notification\n",
               (शून्य *)cap_notification_cap_get_capNtfnPtr(cap));
        थांबा;
    }
    स्थिती cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        थांबा;
    }
    स्थिती cap_cnode_cap: {
        प्रतिज्ञा(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_reply_cap: {
        म_लिखो("%p_reply = rtreply\n",
               (शून्य *)cap_reply_cap_get_capReplyPtr(cap));
        थांबा;
    }
    स्थिती cap_sched_context_cap: {
        म_लिखो("%p_sc = sc ",
               (शून्य *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        थांबा;
    }
#endif
    स्थिती cap_irq_handler_cap: {
        म_लिखो("%p_%lu_irq = irq\n",
               (शून्य *)cap_irq_handler_cap_get_capIRQ(cap),
               (दीर्घ अचिन्हित पूर्णांक)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        थांबा;
    }
    पूर्वनिर्धारित:
        print_object_arch(cap);
        थांबा;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
