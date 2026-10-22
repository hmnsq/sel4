/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_DEBUG_BUILD

#অন্তর্ভাব <machine/capdl.h>
#অন্তর্ভাব <machine/registerset.h>
#অন্তর্ভাব <machine/timer.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#অন্তর্ভাব <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
পূর্ণাঙ্ক watermark = 0;

শূন্য add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    যদি (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

শূন্য reset_seen_list(শূন্য)
{
    memset(seen_list, 0, SEEN_SZ * পরিমাণ(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    প্রতি (পূর্ণাঙ্ক i = 0; i < watermark; i++) {
        যদি (same_cap(seen_list[i], c)) {
            প্রত্যাগম true;
        }
    }
    প্রত্যাগম false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    প্রত্যাগম (a.শব্দগুলি[0] == b.শব্দগুলি[0] && a.শব্দগুলি[1] == b.শব্দগুলি[1]);
}

/* Return true if strings are the same */
স্থির অন্তঃস্থ bool_t strings_equal(ধ্রুব বর্ণ *str1, ধ্রুব বর্ণ *str2)
{
    যাবত্ (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    প্রত্যাগম !(*(ধ্রুব unsigned বর্ণ *)str1 - * (ধ্রুব unsigned বর্ণ *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t মূল_or_idle_tcb(tcb_t *tcb)
{
    প্রত্যাগম (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

শূন্য obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned পূর্ণাঙ্ক)tcb->tcbIPCBuffer,
           (long unsigned পূর্ণাঙ্ক)getRestartPC(tcb),
           (long unsigned পূর্ণাঙ্ক)get_tcb_sp(tcb),
           (long unsigned পূর্ণাঙ্ক)tcb->tcbPriority,
           (long unsigned পূর্ণাঙ্ক)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned পূর্ণাঙ্ক)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    যদি (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

স্থির অন্তঃস্থ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    যাবত্ (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    প্রত্যাগম sum;
}

শূন্য obj_sc_print_attrs(cap_t sc_cap)
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

শূন্য obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (শূন্য *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned পূর্ণাঙ্ক)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned পূর্ণাঙ্ক)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    প্রতি (cte_t *nextPtr = CTE_PTR(mdb_গ্রন্থি_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_গ্রন্থি_get_mdb_Next(slot->cteMDBNode))) {
        যদি (!sameRegionAs(slot->cap, nextPtr->cap)) {
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

শূন্য obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned পূর্ণাঙ্ক)cap_cnode_cap_get_capCNodeRadix(cnode));
}

শূন্য obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    যদি (seen(cnode)) {
        প্রত্যাগম;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (শূন্য *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    প্রতি (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        যদি (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } অন্যথা যদি (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } অন্যথা যদি (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

শূন্য cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned পূর্ণাঙ্ক)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned পূর্ণাঙ্ক)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

শূন্য cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned পূর্ণাঙ্ক badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

শূন্য cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned পূর্ণাঙ্ক badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

শূন্য obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    যদি (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (শূন্য *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    যদি (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_মূল_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    যদি (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    যদি (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (শূন্য *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    যদি (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    যদি (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (শূন্য *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# অন্যথা
    /* Reply cap slot */
    যদি (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (শূন্য *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    যদি (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
শূন্য obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t মূল = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    যদি (cap_get_capType(মূল) != cap_cnode_cap) {
        প্রত্যাগম;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(মূল);
    যদি (seen(মূল)) {
        প্রত্যাগম;
    }
    add_to_seen(মূল);

    printf("%p_cnode {\n", (শূন্য *)cap_cnode_cap_get_capCNodePtr(মূল));

    প্রতি (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        যদি (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    প্রতি (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        যদি (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

শূন্য obj_irq_print_maps(শূন্য)
{
    printf("irq maps {\n");

    প্রতি (seL4_Word target = 0; target < CONFIG_MAX_অঙ্ক_NODES; target++) {
        প্রতি (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            যদি (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                যদি (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#যদি defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned পূর্ণাঙ্ক)irq.irq,
#অন্যথা
                           (long unsigned পূর্ণাঙ্ক)irq,
#endif
                           (long unsigned পূর্ণাঙ্ক)target);
                }
            }
        }
    }
    printf("}\n");
}

শূন্য obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    যদি (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#যদি defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned পূর্ণাঙ্ক)irq.irq,
#অন্যথা
               (long unsigned পূর্ণাঙ্ক)irq,
#endif
               (long unsigned পূর্ণাঙ্ক)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        যদি (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

শূন্য print_objects(শূন্য)
{
    প্রতি (tcb_t *curr = গ্রন্থি_STATE(ksDebugTCBs); curr != রিক্ত; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        যদি (মূল_or_idle_tcb(curr)) {
            অনুবর্ত;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    প্রতি (tcb_t *curr = গ্রন্থি_STATE(ksDebugTCBs); curr != রিক্ত; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        যদি (মূল_or_idle_tcb(curr)) {
            অনুবর্ত;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        যদি (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

শূন্য print_caps(শূন্য)
{
    প্রতি (tcb_t *curr = গ্রন্থি_STATE(ksDebugTCBs); curr != রিক্ত; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        যদি (মূল_or_idle_tcb(curr)) {
            অনুবর্ত;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

শূন্য print_cap(cap_t cap)
{
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_endpoint_cap: {
        printf("%p_ep ",
               (শূন্য *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        ভঙ্গ;
    }
    পক্ষ cap_notification_cap: {
        printf("%p_notification ",
               (শূন্য *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        ভঙ্গ;
    }
    পক্ষ cap_untyped_cap: {
        printf("%p_untyped\n",
               (শূন্য *)cap_untyped_cap_get_capPtr(cap));
        ভঙ্গ;
    }
    পক্ষ cap_thread_cap: {
        printf("%p_tcb\n",
               (শূন্য *)cap_thread_cap_get_capTCBPtr(cap));
        ভঙ্গ;
    }
    পক্ষ cap_cnode_cap: {
        printf("%p_cnode ",
               (শূন্য *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        ভঙ্গ;
    }
#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_reply_cap: {
        printf("%p_reply\n",
               (শূন্য *)cap_reply_cap_get_capReplyPtr(cap));
        ভঙ্গ;
    }
    পক্ষ cap_sched_context_cap: {
        printf("%p_sc\n",
               (শূন্য *)cap_sched_context_cap_get_capSCPtr(cap));
        ভঙ্গ;
    }
    পক্ষ cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned পূর্ণাঙ্ক)cap_sched_control_cap_get_core(cap));
        ভঙ্গ;
    }
#endif
    পক্ষ cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        ভঙ্গ;
    }
    পক্ষ cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (শূন্য *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned পূর্ণাঙ্ক)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ভঙ্গ;
    }
    স্বভাব: {
        print_cap_arch(cap);
        ভঙ্গ;
    }
    }
}

শূন্য print_object(cap_t cap)
{
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (শূন্য *)cap_endpoint_cap_get_capEPPtr(cap));
        ভঙ্গ;
    }
    পক্ষ cap_notification_cap: {
        printf("%p_notification = notification\n",
               (শূন্য *)cap_notification_cap_get_capNtfnPtr(cap));
        ভঙ্গ;
    }
    পক্ষ cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        ভঙ্গ;
    }
    পক্ষ cap_cnode_cap: {
        প্রতিজ্ঞা(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (শূন্য *)cap_reply_cap_get_capReplyPtr(cap));
        ভঙ্গ;
    }
    পক্ষ cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (শূন্য *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        ভঙ্গ;
    }
#endif
    পক্ষ cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (শূন্য *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned পূর্ণাঙ্ক)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ভঙ্গ;
    }
    স্বভাব:
        print_object_arch(cap);
        ভঙ্গ;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
