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
पूर्णाङ्क watermark = 0;

शून्य add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    सचे (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

शून्य reset_seen_list(शून्य)
{
    memset(seen_list, 0, SEEN_SZ * परिमाण(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    अत्थाय (पूर्णाङ्क i = 0; i < watermark; i++) {
        सचे (same_cap(seen_list[i], c)) {
            पटिनिवत्त true;
        }
    }
    पटिनिवत्त false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    पटिनिवत्त (a.शब्दाः[0] == b.शब्दाः[0] && a.शब्दाः[1] == b.शब्दाः[1]);
}

/* Return true if strings are the same */
स्थिर अन्तःस्थ bool_t strings_equal(ध्रुव वर्ण *str1, ध्रुव वर्ण *str2)
{
    याव (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    पटिनिवत्त !(*(ध्रुव unsigned वर्ण *)str1 - * (ध्रुव unsigned वर्ण *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t मूलम्_or_idle_tcb(tcb_t *tcb)
{
    पटिनिवत्त (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

शून्य obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned पूर्णाङ्क)tcb->tcbIPCBuffer,
           (long unsigned पूर्णाङ्क)getRestartPC(tcb),
           (long unsigned पूर्णाङ्क)get_tcb_sp(tcb),
           (long unsigned पूर्णाङ्क)tcb->tcbPriority,
           (long unsigned पूर्णाङ्क)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned पूर्णाङ्क)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    सचे (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

स्थिर अन्तःस्थ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    याव (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    पटिनिवत्त sum;
}

शून्य obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    printf("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_word" bits)\n",
           ticksToUs(period), period,
           ticksToUs(budget), budget,
           (word_t)cap_sched_context_cap_get_capSCSizeBits(sc_cap));
}
#endif /* CONFIG_KERNEL_MCS */

शून्य obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (शून्य *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned पूर्णाङ्क)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned पूर्णाङ्क)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    अत्थाय (cte_t *nextPtr = CTE_PTR(mdb_ग्रन्थिः_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_ग्रन्थिः_get_mdb_Next(slot->cteMDBNode))) {
        सचे (!sameRegionAs(slot->cap, nextPtr->cap)) {
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
    printf("}\n");
}

शून्य obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned पूर्णाङ्क)cap_cnode_cap_get_capCNodeRadix(cnode));
}

शून्य obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    सचे (seen(cnode)) {
        पटिनिवत्त;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (शून्य *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    अत्थाय (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        सचे (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } अञ्ञथा सचे (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } अञ्ञथा सचे (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

शून्य cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned पूर्णाङ्क)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned पूर्णाङ्क)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

शून्य cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned पूर्णाङ्क badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

शून्य cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned पूर्णाङ्क badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

शून्य obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    सचे (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (शून्य *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    सचे (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_मूलम्_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    सचे (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    सचे (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    सचे (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    सचे (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# अञ्ञथा
    /* Reply cap slot */
    सचे (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (शून्य *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    सचे (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
शून्य obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t मूलम् = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    सचे (cap_get_capType(मूलम्) != cap_cnode_cap) {
        पटिनिवत्त;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(मूलम्);
    सचे (seen(मूलम्)) {
        पटिनिवत्त;
    }
    add_to_seen(मूलम्);

    printf("%p_cnode {\n", (शून्य *)cap_cnode_cap_get_capCNodePtr(मूलम्));

    अत्थाय (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        सचे (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    अत्थाय (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        सचे (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

शून्य obj_irq_print_maps(शून्य)
{
    printf("irq maps {\n");

    अत्थाय (seL4_Word target = 0; target < CONFIG_MAX_अङ्कः_NODES; target++) {
        अत्थाय (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            सचे (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                सचे (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#सचे defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned पूर्णाङ्क)irq.irq,
#अञ्ञथा
                           (long unsigned पूर्णाङ्क)irq,
#endif
                           (long unsigned पूर्णाङ्क)target);
                }
            }
        }
    }
    printf("}\n");
}

शून्य obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    सचे (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#सचे defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned पूर्णाङ्क)irq.irq,
#अञ्ञथा
               (long unsigned पूर्णाङ्क)irq,
#endif
               (long unsigned पूर्णाङ्क)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        सचे (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

शून्य print_objects(शून्य)
{
    अत्थाय (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        सचे (मूलम्_or_idle_tcb(curr)) {
            अनुवर्त;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    अत्थाय (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        सचे (मूलम्_or_idle_tcb(curr)) {
            अनुवर्त;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        सचे (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

शून्य print_caps(शून्य)
{
    अत्थाय (tcb_t *curr = ग्रन्थिः_STATE(ksDebugTCBs); curr != रिक्त; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        सचे (मूलम्_or_idle_tcb(curr)) {
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
        printf("%p_ep ",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        भङ्ग;
    }
    पक्ष cap_notification_cap: {
        printf("%p_notification ",
               (शून्य *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        भङ्ग;
    }
    पक्ष cap_untyped_cap: {
        printf("%p_untyped\n",
               (शून्य *)cap_untyped_cap_get_capPtr(cap));
        भङ्ग;
    }
    पक्ष cap_thread_cap: {
        printf("%p_tcb\n",
               (शून्य *)cap_thread_cap_get_capTCBPtr(cap));
        भङ्ग;
    }
    पक्ष cap_cnode_cap: {
        printf("%p_cnode ",
               (शून्य *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        भङ्ग;
    }
#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_reply_cap: {
        printf("%p_reply\n",
               (शून्य *)cap_reply_cap_get_capReplyPtr(cap));
        भङ्ग;
    }
    पक्ष cap_sched_context_cap: {
        printf("%p_sc\n",
               (शून्य *)cap_sched_context_cap_get_capSCPtr(cap));
        भङ्ग;
    }
    पक्ष cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned पूर्णाङ्क)cap_sched_control_cap_get_core(cap));
        भङ्ग;
    }
#endif
    पक्ष cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        भङ्ग;
    }
    पक्ष cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (शून्य *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned पूर्णाङ्क)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
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
        printf("%p_ep = ep\n",
               (शून्य *)cap_endpoint_cap_get_capEPPtr(cap));
        भङ्ग;
    }
    पक्ष cap_notification_cap: {
        printf("%p_notification = notification\n",
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
#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (शून्य *)cap_reply_cap_get_capReplyPtr(cap));
        भङ्ग;
    }
    पक्ष cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (शून्य *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        भङ्ग;
    }
#endif
    पक्ष cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (शून्य *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned पूर्णाङ्क)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        भङ्ग;
    }
    स्वभाव:
        print_object_arch(cap);
        भङ्ग;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
