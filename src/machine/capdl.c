/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ଅନ୍ତର୍ଭାଵ <machine/capdl.h>
#ଅନ୍ତର୍ଭାଵ <machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <machine/timer.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#ଅନ୍ତର୍ଭାଵ <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
ପୂର୍ଣ୍ଣସଂଖ୍ୟା watermark = 0;

ଶୂନ୍ୟ add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    ଯଦି (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

ଶୂନ୍ୟ reset_seen_list(ଶୂନ୍ୟ)
{
    memset(seen_list, 0, SEEN_SZ * ଆକାର(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < watermark; i++) {
        ଯଦି (same_cap(seen_list[i], c)) {
            ଫେରନ୍ତୁ true;
        }
    }
    ଫେରନ୍ତୁ false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    ଫେରନ୍ତୁ (a.ଶବ୍ଦଗୁଡ଼ିକ[0] == b.ଶବ୍ଦଗୁଡ଼ିକ[0] && a.ଶବ୍ଦଗୁଡ଼ିକ[1] == b.ଶବ୍ଦଗୁଡ଼ିକ[1]);
}

/* Return true if strings are the same */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool_t strings_equal(ଅଚଳ ଅକ୍ଷର *str1, ଅଚଳ ଅକ୍ଷର *str2)
{
    ଯେତେବେଳେ (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    ଫେରନ୍ତୁ !(*(ଅଚଳ unsigned ଅକ୍ଷର *)str1 - * (ଅଚଳ unsigned ଅକ୍ଷର *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t ମୂଳ_or_idle_tcb(tcb_t *tcb)
{
    ଫେରନ୍ତୁ (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

ଶୂନ୍ୟ obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tcb->tcbIPCBuffer,
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getRestartPC(tcb),
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)get_tcb_sp(tcb),
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tcb->tcbPriority,
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    ଯଦି (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    ଯେତେବେଳେ (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    ଫେରନ୍ତୁ sum;
}

ଶୂନ୍ୟ obj_sc_print_attrs(cap_t sc_cap)
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

ଶୂନ୍ୟ obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (ଶୂନ୍ୟ *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    ପାଇଁ (cte_t *nextPtr = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(slot->cteMDBNode))) {
        ଯଦି (!sameRegionAs(slot->cap, nextPtr->cap)) {
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

ଶୂନ୍ୟ obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_cnode_cap_get_capCNodeRadix(cnode));
}

ଶୂନ୍ୟ obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    ଯଦି (seen(cnode)) {
        ଫେରନ୍ତୁ;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (ଶୂନ୍ୟ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    ପାଇଁ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        ଯଦି (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } ଅନ୍ୟଥା ଯଦି (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } ଅନ୍ୟଥା ଯଦି (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

ଶୂନ୍ୟ cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

ଶୂନ୍ୟ cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

ଶୂନ୍ୟ cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

ଶୂନ୍ୟ obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    ଯଦି (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (ଶୂନ୍ୟ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    ଯଦି (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_ମୂଳ_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    ଯଦି (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    ଯଦି (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (ଶୂନ୍ୟ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    ଯଦି (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    ଯଦି (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (ଶୂନ୍ୟ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# ଅନ୍ୟଥା
    /* Reply cap slot */
    ଯଦି (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (ଶୂନ୍ୟ *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    ଯଦି (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
ଶୂନ୍ୟ obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t ମୂଳ = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    ଯଦି (cap_get_capType(ମୂଳ) != cap_cnode_cap) {
        ଫେରନ୍ତୁ;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(ମୂଳ);
    ଯଦି (seen(ମୂଳ)) {
        ଫେରନ୍ତୁ;
    }
    add_to_seen(ମୂଳ);

    printf("%p_cnode {\n", (ଶୂନ୍ୟ *)cap_cnode_cap_get_capCNodePtr(ମୂଳ));

    ପାଇଁ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        ଯଦି (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    ପାଇଁ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        ଯଦି (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

ଶୂନ୍ୟ obj_irq_print_maps(ଶୂନ୍ୟ)
{
    printf("irq maps {\n");

    ପାଇଁ (seL4_Word target = 0; target < CONFIG_MAX_ଅଙ୍କ_NODES; target++) {
        ପାଇଁ (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            ଯଦି (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                ଯଦି (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#ଯଦି defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)irq.irq,
#ଅନ୍ୟଥା
                           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)irq,
#endif
                           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)target);
                }
            }
        }
    }
    printf("}\n");
}

ଶୂନ୍ୟ obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    ଯଦି (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#ଯଦି defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)irq.irq,
#ଅନ୍ୟଥା
               (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)irq,
#endif
               (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        ଯଦି (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

ଶୂନ୍ୟ print_objects(ଶୂନ୍ୟ)
{
    ପାଇଁ (tcb_t *curr = ଗଣ୍ଠି_STATE(ksDebugTCBs); curr != ଖାଲି; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        ଯଦି (ମୂଳ_or_idle_tcb(curr)) {
            ଜାରି;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    ପାଇଁ (tcb_t *curr = ଗଣ୍ଠି_STATE(ksDebugTCBs); curr != ଖାଲି; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        ଯଦି (ମୂଳ_or_idle_tcb(curr)) {
            ଜାରି;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        ଯଦି (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

ଶୂନ୍ୟ print_caps(ଶୂନ୍ୟ)
{
    ପାଇଁ (tcb_t *curr = ଗଣ୍ଠି_STATE(ksDebugTCBs); curr != ଖାଲି; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        ଯଦି (ମୂଳ_or_idle_tcb(curr)) {
            ଜାରି;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

ଶୂନ୍ୟ print_cap(cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_endpoint_cap: {
        printf("%p_ep ",
               (ଶୂନ୍ୟ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_notification_cap: {
        printf("%p_notification ",
               (ଶୂନ୍ୟ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_untyped_cap: {
        printf("%p_untyped\n",
               (ଶୂନ୍ୟ *)cap_untyped_cap_get_capPtr(cap));
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_thread_cap: {
        printf("%p_tcb\n",
               (ଶୂନ୍ୟ *)cap_thread_cap_get_capTCBPtr(cap));
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_cnode_cap: {
        printf("%p_cnode ",
               (ଶୂନ୍ୟ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_reply_cap: {
        printf("%p_reply\n",
               (ଶୂନ୍ୟ *)cap_reply_cap_get_capReplyPtr(cap));
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_sched_context_cap: {
        printf("%p_sc\n",
               (ଶୂନ୍ୟ *)cap_sched_context_cap_get_capSCPtr(cap));
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_sched_control_cap_get_core(cap));
        ଭାଙ୍ଗ;
    }
#endif
    ସ୍ଥିତି cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (ଶୂନ୍ୟ *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
        print_cap_arch(cap);
        ଭାଙ୍ଗ;
    }
    }
}

ଶୂନ୍ୟ print_object(cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (ଶୂନ୍ୟ *)cap_endpoint_cap_get_capEPPtr(cap));
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_notification_cap: {
        printf("%p_notification = notification\n",
               (ଶୂନ୍ୟ *)cap_notification_cap_get_capNtfnPtr(cap));
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_cnode_cap: {
        ପ୍ରତିଜ୍ଞା(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (ଶୂନ୍ୟ *)cap_reply_cap_get_capReplyPtr(cap));
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (ଶୂନ୍ୟ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        ଭାଙ୍ଗ;
    }
#endif
    ସ୍ଥିତି cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (ଶୂନ୍ୟ *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        print_object_arch(cap);
        ଭାଙ୍ଗ;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
